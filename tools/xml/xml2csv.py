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
else:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa

FLOAT_XSD_TYPES = {
    "xsd:float", "xsd:double", "xsd:decimal",
    "floatType", "positiveFloatType", "nonNegativeFloat",
    "nonNegativeFloatType", "nonNegativeFloatTypeWithErrorValue",
    "timeType", "timeTypeWithErrorValue", "signedTimeType",
    "departType", "personDepartType", "departPosType", "departPosLatType",
    "departSpeedType", "arrivalPosType", "arrivalPosLatType",
    "arrivalSpeedType", "accessPosType", "offsetType", "periodType",
    "latAlignmentType", "nonNegativeDistributionType",
}


def _is_float_value(val):
    if "." not in val or "_" in val:
        return False
    try:
        float(val)
        return True
    except ValueError:
        return False


class FilteredAttrFinder(sumolib.xml.AttrFinder):

    def __init__(self, xsdFile, source, split, keepAttrs=None, keepTags=None):
        self.keepTags = keepTags
        self._visitedSkipped = set()
        sumolib.xml.AttrFinder.__init__(self, xsdFile, source, split, keepAttrs)
        if self.keepTags is not None and split:
            for root in list(self.attrs.keys()):
                if not self.attrs[root] and all(not tags for tags in self.depthTags[root]):
                    del self.attrs[root]
                    del self.depthTags[root]

    def addElement(self, root, name, depth):
        while len(self.depthTags[root]) <= depth:
            self.depthTags[root].append([])
        if name not in self.tagDepths:
            self.tagDepths[name] = depth
            self.depthTags[root][depth].append(name)
            return True
        if name in self.depthTags[root][depth]:
            return True
        print("Ignoring tag %s at depth %s" % (name, depth), file=sys.stderr)
        return False

    def recursiveAttrFind(self, root, currEle, depth):
        if self.keepTags is None or currEle.name in self.keepTags:
            if currEle.name in self.tagDepths:
                while len(self.depthTags[root.name]) <= depth:
                    self.depthTags[root.name].append([])
                if currEle.name not in self.depthTags[root.name][depth]:
                    print("Ignoring tag %s at depth %s" % (currEle.name, depth), file=sys.stderr)
                return
            self.addElement(root.name, currEle.name, depth)
            for a in currEle.attributes:
                if ":" not in a.name and (self.keepAttrs is None or a.name in self.keepAttrs):
                    self.tagAttrs[currEle.name][a.name] = a
                    anew = "%s_%s" % (currEle.name, a.name)
                    self.renamedAttrs[(currEle.name, a.name)] = anew
                    attrList = self.attrs[root.name]
                    if anew in attrList:
                        del attrList[attrList.index(anew)]
                    attrList.append(anew)
        else:
            while len(self.depthTags[root.name]) <= depth:
                self.depthTags[root.name].append([])
            if (root.name, currEle.name) in self._visitedSkipped:
                return
            self._visitedSkipped.add((root.name, currEle.name))
        for ele in currEle.children:
            self.recursiveAttrFind(root, ele, depth + 1)

    def startElement(self, name, attrs):
        sumolib.xml.NestingHandler.startElement(self, name, attrs)
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            if self.depth() == self.rootDepth and root not in self.attrs:
                self.attrs[root] = []
                self.depthTags[root] = [[] for _ in range(self.rootDepth + 1)]
            if self.keepTags is not None and name not in self.keepTags:
                return
            if not self.addElement(root, name, self.depth()):
                return
            for a in sorted(list(attrs.keys())):
                if self.keepAttrs is not None and a not in self.keepAttrs:
                    continue
                if a not in self.tagAttrs[name] and ":" not in a:
                    self.tagAttrs[name][a] = sumolib.xml.xsd.XmlAttribute(a)
                    if (name, a) not in self.renamedAttrs:
                        anew = "%s_%s" % (name, a)
                        self.renamedAttrs[(name, a)] = anew
                        self.attrs[root].append(anew)


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
            headerCols = (["tag"] if options.tagColumn else []) + attrFinder.attrs[root]
            self.outfiles[root].write(
                options.separator.join(map(self.quote, headerCols)) + u"\n")

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
            if (root in self.attrFinder.depthTags
                    and self.depth() < len(self.attrFinder.depthTags[root])
                    and name in self.attrFinder.depthTags[root][self.depth()]):
                for a, v in attrs.items():
                    if isinstance(a, tuple):
                        a = a[1]
                    # print(a, dict(self.attrFinder.tagAttrs[name]))
                    if a in self.attrFinder.tagAttrs[name]:
                        if self.attrFinder.xsdStruc:
                            attrType = self.attrFinder.tagAttrs[name][a].type
                            enum = self.attrFinder.xsdStruc.getEnumeration(attrType)
                            if enum:
                                v = enum.index(v)
                            elif (self.options.decimal_separator != "."
                                  and attrType
                                  and (attrType in FLOAT_XSD_TYPES or "float" in attrType.lower())
                                  and _is_float_value(v)):
                                v = v.replace(".", self.options.decimal_separator)
                        elif self.options.decimal_separator != "." and _is_float_value(v):
                            v = v.replace(".", self.options.decimal_separator)
                        a2 = self.attrFinder.renamedAttrs.get((name, a), a)
                        self.currentValues[a2] = v
                        self.haveUnsavedValues = True

    def endElement(self, name):
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            # print("end", name, root, self.depth(), self.attrFinder.depthTags[root][self.depth()],
            # self.haveUnsavedValues)
            if (root in self.attrFinder.depthTags
                    and self.depth() < len(self.attrFinder.depthTags[root])
                    and name in self.attrFinder.depthTags[root][self.depth()]):
                if self.haveUnsavedValues:
                    rowVals = ([name] if self.options.tagColumn else []) + [
                        self.currentValues[a] for a in self.attrFinder.attrs[root]
                    ]
                    self.outfiles[root].write(self.options.separator.join(
                        [self.quote(v) for v in rowVals]) + u"\n")
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
    optParser.add_argument("-d", "--decimal-separator", dest="decimal_separator", default=".",
                           help="decimal separator for float values")
    optParser.add_argument("-q", "--quotechar", default='',
                           help="quoting character for fields")
    optParser.add_argument("-x", "--xsd", category="processing",
                           help="xsd schema to use")
    optParser.add_argument("-a", "--validation", action="store_true", default=False,
                           help="enable schema validation")
    optParser.add_argument("--keep-attributes", dest="keepAttrs",
                           help="Only keep the given attributes")
    optParser.add_argument("--keep-tags", dest="keepTags",
                           help="Only keep the given tags")
    optParser.add_argument("--tag-column", dest="tagColumn", action="store_true", default=False,
                           help="Add a column containing the lowest tag name")
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
    if options.keepTags:
        options.keepTags = set(options.keepTags.split(','))
    return options


def main(args=None):
    options = get_options(args)
    # get attributes
    if options.keepTags is not None:
        attrFinder = FilteredAttrFinder(options.xsd, options.source, options.split,
                                        options.keepAttrs, options.keepTags)
    else:
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
