#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    xml2csv.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2013-12-08
@version $Id$

Convert hierarchical xml files to csv. This only makes sense if the hierarchy has low depth.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, sys, socket
from collections import defaultdict
from optparse import OptionParser
import xml.sax
try:
    import lxml.etree
    import lxml.sax
    haveLxml = True
except ImportError:
    haveLxml = False

import xsd

class NestingHandler(xml.sax.handler.ContentHandler):
    """A handler which knows the current nesting of tags"""
    def __init__(self):
        self.tagstack = []

    def startElement(self, name, attrs):
        self.tagstack.append(name)

    def endElement(self, name):
        self.tagstack.pop()

    def depth(self):
        # do not count the root element
        return len(self.tagstack) - 1

class AttrFinder(NestingHandler):
    def __init__(self, xsdFile, source):
        NestingHandler.__init__(self)
        self.tagDepths = {} # tag -> depth of appearance
        self.ignoredTags = set()
        self.tagAttrs = defaultdict(dict) # tag -> set of attrs
        self.renamedAttrs = {} # (name, attr) -> renamedAttr
        self.attrs = {}
        self.depthTags = {} # child of root: depth of appearance -> tag
        if xsdFile:
            self.xsdStruc = xsd.XsdStructure(xsdFile)
            for ele in self.xsdStruc.root.children:
                self.attrs[ele.name] = []
                self.depthTags[ele.name] = [None]
                self.recursiveAttrFind(ele, ele, 1)
        else:
            self.xsdStruc = None
            xml.sax.parse(source, self)

    def recursiveAttrFind(self, root, currEle, depth):
        if len(self.depthTags[root.name]) == depth:
            self.tagDepths[currEle.name] = depth
            self.depthTags[root.name].append(currEle.name)
        else:
            print("Ignoring tag %s at depth %s" % (currEle.name, depth), file=sys.stderr)
            return
        for a in currEle.attributes:
            self.tagAttrs[currEle.name][a.name] = a
            anew = "%s_%s" % (currEle.name, a.name)
            self.renamedAttrs[(currEle.name, a.name)] = anew
            self.attrs[root.name].append(anew)
        for ele in currEle.children:
            self.recursiveAttrFind(root, ele, depth + 1)

    def startElement(self, name, attrs):
        NestingHandler.startElement(self, name, attrs)
        if self.depth() > 0:
            root = self.tagstack[1]
            if self.depth() == 1 and root not in self.attrs:
                self.attrs[root] = []
                self.depthTags[root] = [None]
            if not name in self.tagDepths:
                if len(self.depthTags[root]) == self.depth():
                    self.tagDepths[name] = self.depth()
                    self.depthTags[root].append(name)
                else:
                    print("Ignoring tag %s at depth %s" % (name, self.depth()), file=sys.stderr)
                    return
            elif self.depthTags[root][self.depth()] != name:
                print("Ignoring tag %s at depth %s" % (name, self.depth()), file=sys.stderr)
                return
            # collect attributes
            for a in attrs.keys():
                if not a in self.tagAttrs[name]:
                    self.tagAttrs[name][a] = xsd.XmlAttribute(a)
                    if not (name, a) in self.renamedAttrs:
                        anew = "%s_%s" % (name, a)
                        self.renamedAttrs[(name, a)] = anew
                        self.attrs[root].append(anew)


class CSVWriter(NestingHandler):
    def __init__(self, attrs, renamedAttrs, depthTags, tagAttrs, options):
        NestingHandler.__init__(self)
        self.attrs = attrs
        self.renamedAttrs = renamedAttrs
        self.depthTags = depthTags
        self.tagAttrs = tagAttrs
        self.options = options
        self.currentValues = defaultdict(lambda: "")
        self.haveUnsavedValues = False
        self.outfiles = {}
        for root, depths in depthTags.iteritems():
            suffix = ""
            if len(depthTags) > 1:
                suffix = root
            if options.output:
                outfilename = options.output + "%s.csv" % suffix
            else:
                outfilename = os.path.splitext(options.source)[0] + "%s.csv" % suffix
            self.outfiles[root] = open(outfilename, 'w')
            self.outfiles[root].write(options.separator.join(map(self.quote,attrs[root])) + "\n")

    def quote(self, s):
        return "%s%s%s" % (self.options.quotechar, s, self.options.quotechar)

    def startElementNS(self, name, qname, attrs):
        self.startElement(qname, attrs)

    def endElementNS(self, name, qname):
        self.endElement(qname)

    def startElement(self, name, attrs):
        NestingHandler.startElement(self, name, attrs)
        if self.depth() > 0:
            root = self.tagstack[1]
            if self.depthTags[root][self.depth()] == name:
                for a, v in attrs.items():
                    if isinstance(a, tuple):
                        a = a[1]
                    a2 = self.renamedAttrs.get((name, a), a)
                    self.currentValues[a2] = v
                    self.haveUnsavedValues = True

    def endElement(self, name):
        if self.depth() > 0:
            root = self.tagstack[1]
            if self.depthTags[root][self.depth()] == name:
                if self.haveUnsavedValues:
                    self.outfiles[root].write(self.options.separator.join(
                        [self.quote(self.currentValues[a]) for a in self.attrs[root]]) + "\n")
                    self.haveUnsavedValues = False
                for a in self.tagAttrs[name]:
                    a2 = self.renamedAttrs.get((name, a), a)
                    del self.currentValues[a2]
        NestingHandler.endElement(self, name)

def getSocketStream(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("localhost", port))
    s.listen(1)
    conn, addr = s.accept()
    return conn.makefile()

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0]) + " [<options>] <input_file_or_port>")
    optParser.add_option("-s", "--separator", default=";",
                         help="separating character for fields")
    optParser.add_option("-q", "--quotechar", default='',
                         help="quoting character for fields")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-a", "--validation", action="store_true",
                         default=False, help="enable schema validation")
    optParser.add_option("-o", "--output", help="base name for output")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if options.validation and not haveLxml:
        print("lxml not available, skipping validation", file=sys.stderr)
        options.validation = False
    options.source = args[0]
    return options 

def main():
    options = get_options()
    # get attributes
    attrFinder = AttrFinder(options.xsd, options.source)
    # write csv
    handler = CSVWriter(attrFinder.attrs, attrFinder.renamedAttrs,
            attrFinder.depthTags, attrFinder.tagAttrs, options)
    if options.validation:
        schema = lxml.etree.XMLSchema(file=options.xsd)
        parser = lxml.etree.XMLParser(schema=schema)
        if options.source.isdigit():
            tree = lxml.etree.parse(getSocketStream(int(options.source)), parser)
        else:
            tree = lxml.etree.parse(options.source, parser)
        lxml.sax.saxify(tree, handler)
    else:
        if options.source.isdigit():
            xml.sax.parse(getSocketStream(int(options.source)), handler)
        else:
            xml.sax.parse(options.source, handler)

if __name__ == "__main__":
    main()
