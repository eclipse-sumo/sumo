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
    def __init__(self, xsdFile, source, split):
        NestingHandler.__init__(self)
        self.tagDepths = {} # tag -> depth of appearance
        self.tagAttrs = defaultdict(dict) # tag -> set of attrs
        self.renamedAttrs = {} # (name, attr) -> renamedAttr
        self.attrs = {}
        self.depthTags = {} # child of root: depth of appearance -> tag list
        self.rootDepth = 1 if split else 0
        if xsdFile:
            self.xsdStruc = xsd.XsdStructure(xsdFile)
            if split:
                for ele in self.xsdStruc.root.children:
                    self.attrs[ele.name] = []
                    self.depthTags[ele.name] = [[]]
                    self.recursiveAttrFind(ele, ele, 1)
            else:
                self.attrs[self.xsdStruc.root.name] = []
                self.depthTags[self.xsdStruc.root.name] = []
                self.recursiveAttrFind(self.xsdStruc.root, self.xsdStruc.root, 0)
        else:
            self.xsdStruc = None
            xml.sax.parse(source, self)

    def addElement(self, root, name, depth):
        if name not in self.tagDepths:
            if len(self.depthTags[root]) == depth:
                self.tagDepths[name] = depth
                self.depthTags[root].append([])
            self.depthTags[root][depth].append(name)
            return True
        if name not in self.depthTags[root][depth]:
            print("Ignoring tag %s at depth %s" % (name, depth), file=sys.stderr)
        return False

    def recursiveAttrFind(self, root, currEle, depth):
        if not self.addElement(root.name, currEle.name, depth):
            return
        for a in currEle.attributes:
            if ":" not in a.name: # no namespace support yet
                self.tagAttrs[currEle.name][a.name] = a
                anew = "%s_%s" % (currEle.name, a.name)
                self.renamedAttrs[(currEle.name, a.name)] = anew
                self.attrs[root.name].append(anew)
        for ele in currEle.children:
            self.recursiveAttrFind(root, ele, depth + 1)

    def startElement(self, name, attrs):
        NestingHandler.startElement(self, name, attrs)
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            if self.depth() == self.rootDepth and root not in self.attrs:
                self.attrs[root] = []
                self.depthTags[root] = [[]] * self.rootDepth
            if not self.addElement(root, name, self.depth()):
                return
            # collect attributes
            for a in attrs.keys():
                if a not in self.tagAttrs[name] and ":" not in a:
                    self.tagAttrs[name][a] = xsd.XmlAttribute(a)
                    if not (name, a) in self.renamedAttrs:
                        anew = "%s_%s" % (name, a)
                        self.renamedAttrs[(name, a)] = anew
                        self.attrs[root].append(anew)


class CSVWriter(NestingHandler):
    def __init__(self, attrFinder, options):
        NestingHandler.__init__(self)
        self.attrFinder = attrFinder
        self.options = options
        self.currentValues = defaultdict(lambda: "")
        self.haveUnsavedValues = False
        self.outfiles = {}
        self.rootDepth = 1 if options.split else 0
        for root in attrFinder.depthTags.iterkeys():
            if len(attrFinder.depthTags) == 1:
                if not options.output:
                    outfilename = os.path.splitext(options.source)[0]
                if not options.output.endswith(".csv"):
                    options.output += ".csv"
                self.outfiles[root] = getOutStream(options.output)
            else:
                if options.output:
                    outfilename = options.output + "%s.csv" % root
                else:
                    outfilename = os.path.splitext(options.source)[0] + "%s.csv" % root
                self.outfiles[root] = open(outfilename, 'w')
            self.outfiles[root].write(options.separator.join(map(self.quote,attrFinder.attrs[root])) + "\n")

    def quote(self, s):
        return "%s%s%s" % (self.options.quotechar, s, self.options.quotechar)

# the following two are needed for the lxml saxify to work
    def startElementNS(self, name, qname, attrs):
        self.startElement(qname, attrs)

    def endElementNS(self, name, qname):
        self.endElement(qname)

    def startElement(self, name, attrs):
        NestingHandler.startElement(self, name, attrs)
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            if name in self.attrFinder.depthTags[root][self.depth()]:
                for a, v in attrs.items():
                    if isinstance(a, tuple):
                        a = a[1]
                    if a in self.attrFinder.tagAttrs[name]:
                        if self.attrFinder.xsdStruc:
                            enum = self.attrFinder.xsdStruc.getEnumeration(self.attrFinder.tagAttrs[name][a].type)
                            if enum:
                                v = enum.index(v)
                        a2 = self.attrFinder.renamedAttrs.get((name, a), a)
                        self.currentValues[a2] = v
                        self.haveUnsavedValues = True

    def endElement(self, name):
        if self.depth() >= self.rootDepth:
            root = self.tagstack[self.rootDepth]
            if name in self.attrFinder.depthTags[root][self.depth()]:
                if self.haveUnsavedValues:
                    self.outfiles[root].write(self.options.separator.join(
                        [self.quote(self.currentValues[a]) for a in self.attrFinder.attrs[root]]) + "\n")
                    self.haveUnsavedValues = False
                for a in self.attrFinder.tagAttrs[name]:
                    a2 = self.attrFinder.renamedAttrs.get((name, a), a)
                    del self.currentValues[a2]
        NestingHandler.endElement(self, name)

def getSocketStream(port, mode='rb'):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("localhost", port))
    s.listen(1)
    conn, addr = s.accept()
    return conn.makefile(mode)

def getOutStream(output):
    if output.isdigit():
        return getSocketStream(int(output), 'wb')
    return open(output, 'w')
    
def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0]) + " [<options>] <input_file_or_port>")
    optParser.add_option("-s", "--separator", default=";",
                         help="separating character for fields")
    optParser.add_option("-q", "--quotechar", default='',
                         help="quoting character for fields")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-a", "--validation", action="store_true",
                         default=False, help="enable schema validation")
    optParser.add_option("-p", "--split", action="store_true",
                         default=False, help="split in different files for the first hierarchy level")
    optParser.add_option("-o", "--output", help="base name for output")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if options.validation and not haveLxml:
        print("lxml not available, skipping validation", file=sys.stderr)
        options.validation = False
    if args[0].isdigit():
        if not options.xsd:
            print("a schema is mandatory for stream parsing", file=sys.stderr)
            sys.exit()
        options.source = getSocketStream(int(args[0]))
    else:
        options.source = args[0]
    if options.output and options.output.isdigit() and options.split:
        print("it is not possible to use splitting together with stream output", file=sys.stderr)
        sys.exit()
    return options 

def main():
    options = get_options()
    # get attributes
    attrFinder = AttrFinder(options.xsd, options.source, options.split)
    # write csv
    handler = CSVWriter(attrFinder, options)
    if options.validation:
        schema = lxml.etree.XMLSchema(file=options.xsd)
        parser = lxml.etree.XMLParser(schema=schema)
        tree = lxml.etree.parse(options.source, parser)
        lxml.sax.saxify(tree, handler)
    else:
        xml.sax.parse(options.source, handler)

if __name__ == "__main__":
    main()
