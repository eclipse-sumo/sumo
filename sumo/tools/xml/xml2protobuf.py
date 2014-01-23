#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    xml2protobuf.py
@author  Michael Behrisch
@date    2014-01-22
@version $Id$

Convert hierarchical xml files to binary protobuf.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, sys, socket, subprocess, importlib
from collections import defaultdict
from optparse import OptionParser
import xml.sax
try:
    import lxml.etree
    import lxml.sax
    haveLxml = True
except ImportError:
    haveLxml = False

import xsd, xml2csv

class ProtoWriter(xml2csv.NestingHandler):
    def __init__(self, module, tagAttrs, output):
        xml2csv.NestingHandler.__init__(self)
        self.module = module
        self.tagAttrs = tagAttrs
        self.out = open(output, 'wb')
        self.msgStack = []

    def convert(self, attr, value):
        type = attr.type.lower()
        if "float" in type or "double" in type:
            return float(value)
        if "int" in type or "long" in type:
            return int(value)
        return value

    def startElementNS(self, name, qname, attrs):
        self.startElement(qname, attrs)

    def endElementNS(self, name, qname):
        self.endElement(qname)

    def startElement(self, name, attrs):
        xml2csv.NestingHandler.startElement(self, name, attrs)
        if self.depth() > 0:
            if self.msgStack:
                obj = getattr(self.msgStack[-1], name).add()
            else:
                obj = vars(self.module)[name.capitalize()]()
            for a, v in attrs.items():
                setattr(obj, a, self.convert(self.tagAttrs[name][a], v))
            self.msgStack.append(obj)

    def endElement(self, name):
        if len(self.msgStack) == 1:
            self.out.write(self.msgStack[0].SerializeToString())
        self.msgStack = self.msgStack[:-1]
        xml2csv.NestingHandler.endElement(self, name)

    def endDocument(self):
        self.out.close()

def getSocketStream(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("localhost", port))
    s.listen(1)
    conn, addr = s.accept()
    return conn.makefile()

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0]) + " [<options>] <input_file_or_port>")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("-p", "--protodir", default=".",
                         help="where to put and read .proto files")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-a", "--validation", action="store_true",
                         default=False, help="enable schema validation")
    optParser.add_option("-o", "--output", help="output file name")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if options.validation and not haveLxml:
        print("lxml not available, skipping validation", file=sys.stderr)
        options.validation = False
    options.source = args[0]
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".protomsg"
    return options 

def writeField(protof, use, type, name, tagNumber):
    if type.startswith("xsd:"):
        type = type[4:]
    elif "Float" in type:
        type = "float"
    else:
        type = type.capitalize()
    protof.write("  %s %s %s = %s;\n" % (use, type, name, tagNumber))
    
def generateProto(tagAttrs, depthTags, protodir, base):
    with open(os.path.join(protodir, "%s.proto" % base), 'w') as protof:
        protof.write("package %s;\n" % base)
        for tagList in depthTags.itervalues():
            next = 2
            for tag in tagList[1:]:
                protof.write("\nmessage %s {\n" % tag.capitalize())
                count = 1
                for a in tagAttrs[tag].itervalues():
                    writeField(protof, a.use, a.type, a.name, count)
                    count += 1
                if next != len(tagList):
                    writeField(protof, "repeated", tagList[next], tagList[next], count)
                next += 1
                protof.write("}\n")
    subprocess.call(["protoc", "%s.proto" % base, "--python_out=."])
    return importlib.import_module("%s_pb2" % base)

def main():
    options = get_options()
    # get attributes
    attrFinder = xml2csv.AttrFinder(options.xsd, options.source)
    base = os.path.basename(options.source).split('.')[0]
    # generate proto format description
    module = generateProto(attrFinder.tagAttrs, attrFinder.depthTags, options.protodir, base)
    # write proto message
    handler = ProtoWriter(module, attrFinder.tagAttrs, options.output)
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
