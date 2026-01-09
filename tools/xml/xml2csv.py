#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    xml2csv.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2013-12-08


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import gzip
import collections
import xml.sax
try:
    import lxml.etree
    import lxml.sax
    haveLxml = True
except ImportError:
    haveLxml = False

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


class CSVWriter(sumolib.xml.NestingHandler):

    def __init__(self, attrFinder, options):
        sumolib.xml.NestingHandler.__init__(self)
        self.attrFinder = attrFinder
        self.options = options
        self.currentValues = collections.defaultdict(lambda: "")
        self.haveUnsavedValues = False
        self.outfiles = {}
        self.rootDepth = 1 if options.split else 0
        for root in sorted(attrFinder.depthTags):
            if options.output:
                if not options.output.isdigit() and not options.output.endswith(".csv"):
                    options.output += ".csv"
            else:
                if isinstance(options.source, str):
                    options.output = os.path.splitext(options.source)[0] + ".csv"
                else:
                    options.output = options.source.name + ".csv"
            if len(attrFinder.depthTags) == 1:
                self.outfiles[root] = sumolib.openz(options.output, "w", trySocket=True)
            else:
                outfilename = options.output + "%s.csv" % root
                self.outfiles[root] = sumolib.openz(outfilename, "w", trySocket=True)
            self.outfiles[root].write(
                options.separator.join(map(self.quote, attrFinder.attrs[root])) + u"\n")

    def quote(self, s):
        return "%s%s%s" % (self.options.quotechar, s, self.options.quotechar)

# the following two are needed for the lxml saxify to work
    def startElementNS(self, name, qname, attrs):
        self.startElement(qname, attrs)

    def endElementNS(self, name, qname):
        self.endElement(qname)

    def startElement(self, name, attrs):
        sumolib.xml.NestingHandler.startElement(self, name, attrs)
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            # print("start", name, root, self.depth(), self.attrFinder.depthTags[root][self.depth()])
            if name in self.attrFinder.depthTags[root][self.depth()]:
                for a, v in attrs.items():
                    if isinstance(a, tuple):
                        a = a[1]
                    # print(a, dict(self.attrFinder.tagAttrs[name]))
                    if a in self.attrFinder.tagAttrs[name]:
                        if self.attrFinder.xsdStruc:
                            enum = self.attrFinder.xsdStruc.getEnumeration(
                                self.attrFinder.tagAttrs[name][a].type)
                            if enum:
                                v = enum.index(v)
                        a2 = self.attrFinder.renamedAttrs.get((name, a), a)
                        self.currentValues[a2] = v
                        self.haveUnsavedValues = True

    def endElement(self, name):
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            # print("end", name, root, self.depth(), self.attrFinder.depthTags[root][self.depth()],
            # self.haveUnsavedValues)
            if name in self.attrFinder.depthTags[root][self.depth()]:
                if self.haveUnsavedValues:
                    self.outfiles[root].write(self.options.separator.join(
                        [self.quote(self.currentValues[a]) for a in self.attrFinder.attrs[root]]) + u"\n")
                    self.haveUnsavedValues = False
                for a in self.attrFinder.tagAttrs[name]:
                    a2 = self.attrFinder.renamedAttrs.get((name, a), a)
                    del self.currentValues[a2]
        if self.depth() == 0:
            for f in self.outfiles.values():
                f.close()
        sumolib.xml.NestingHandler.endElement(self, name)


def get_options(arglist=None):
    optParser = sumolib.options.ArgumentParser(description="Convert a XML file to a CSV file")
    # input
    optParser.add_argument("source", category="input", type=optParser.file,
                           help="the input data (stream given by digits or file")
    # output
    optParser.add_argument("-o", "--output", category="output", type=optParser.file,
                           help="base name for output")
    # processing
    optParser.add_argument("-s", "--separator", default=";",
                           help="separating character for fields")
    optParser.add_argument("-q", "--quotechar", default='',
                           help="quoting character for fields")
    optParser.add_argument("-x", "--xsd", category="processing",
                           help="xsd schema to use")
    optParser.add_argument("-a", "--validation", action="store_true", default=False,
                           help="enable schema validation")
    optParser.add_argument("--keep-attributes", dest="keepAttrs",
                           help="Only keep the given attributes")
    optParser.add_argument("-p", "--split", action="store_true", default=False,
                           help="split in different files for the first hierarchy level")
    options = optParser.parse_args(arglist)
    if options.validation and not haveLxml:
        print("lxml not available, skipping validation", file=sys.stderr)
        options.validation = False
    if options.source.isdigit():
        if not options.xsd:
            print("a schema is mandatory for stream parsing", file=sys.stderr)
            sys.exit()
        options.source = sumolib.miscutils.getSocketStream(int(options.source))
    elif options.source.endswith(".gz"):
        options.source = gzip.open(options.source)
    if options.output and options.output.isdigit() and options.split:
        print("it is not possible to use splitting together with stream output", file=sys.stderr)
        sys.exit()
    if options.keepAttrs:
        options.keepAttrs = set(options.keepAttrs.split(','))
    return options


def main(args=None):
    options = get_options(args)
    # get attributes
    attrFinder = sumolib.xml.AttrFinder(options.xsd, options.source, options.split, options.keepAttrs)
    # write csv
    handler = CSVWriter(attrFinder, options)
    if options.validation:
        schema = lxml.etree.XMLSchema(file=options.xsd)
        parser = lxml.etree.XMLParser(schema=schema, resolve_entities=False, no_network=True)
        tree = lxml.etree.parse(options.source, parser)
        lxml.sax.saxify(tree, handler)
    else:
        if not options.xsd and hasattr(options.source, "name") and options.source.name.endswith(".gz"):
            # we need to reopen the file because the AttrFinder already read and closed it
            options.source = gzip.open(options.source.name)
        xml.sax.parse(options.source, handler)


if __name__ == "__main__":
    main()
