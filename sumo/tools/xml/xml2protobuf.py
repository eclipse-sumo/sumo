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
import os, sys, subprocess, importlib, struct
from optparse import OptionParser
import xml.sax
try:
    import lxml.etree
    import lxml.sax
    haveLxml = True
except ImportError:
    haveLxml = False

import xsd, xml2csv

class ProtoWriter(xml.sax.handler.ContentHandler):
    def __init__(self, module, tagAttrs, output):
        self.module = module
        self.tagAttrs = tagAttrs
        self.out = xml2csv.getOutStream(output)
        self.msgStack = []
        self.emptyRootMsg = None

    def convert(self, attr, value):
        typ = getProtobufType(attr.type)
        if typ == "float" or typ == "double":
            return float(value)
        if typ == "int32" or typ == "uint32":
            return int(value)
        return value

    def startElementNS(self, name, qname, attrs):
        self.startElement(qname, attrs)

    def endElementNS(self, name, qname):
        self.endElement(qname)

    def startElement(self, name, attrs):
        if len(self.msgStack) == 0:
            self.emptyRootMsg = vars(self.module)[name.capitalize()]()
            obj = vars(self.module)[name.capitalize()]()
        else:
            obj = getattr(self.msgStack[-1], name).add()
        for a, v in attrs.items():
            setattr(obj, a, self.convert(self.tagAttrs[name][a], v))
        if len(self.msgStack) == 0:
            self.emptyRootMsg.CopyFrom(obj)
        self.msgStack.append(obj)

    def endElement(self, name):
        if len(self.msgStack) == 2:
            self.out.write(struct.pack('>L', self.msgStack[0].ByteSize()))
            self.out.write(self.msgStack[0].SerializeToString())
            self.msgStack[0].Clear()
            self.msgStack[0].CopyFrom(self.emptyRootMsg)
        self.msgStack = self.msgStack[:-1]

    def endDocument(self):
        self.out.write(struct.pack('>L', 0))
        self.out.close()

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0]) + " [<options>] <input_file_or_port>")
    optParser.add_option("-p", "--protodir", default=".",
                         help="where to put and read .proto files")
    optParser.add_option("-x", "--xsd", help="xsd schema to use (mandatory)")
    optParser.add_option("-a", "--validation", action="store_true",
                         default=False, help="enable schema validation")
    optParser.add_option("-o", "--output", help="output file name")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if not options.xsd:
        print("a schema is mandatory", file=sys.stderr)
        sys.exit()
    if options.validation and not haveLxml:
        print("lxml not available, skipping validation", file=sys.stderr)
        options.validation = False
    if args[0].isdigit():
        options.source = xml2csv.getSocketStream(int(args[0]))
    else:
        options.source = args[0]
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".protomsg"
    return options 

def getProtobufType(typ):
    typ = typ.lower()
    if ":" in typ:
        typ = typ.split(":")[-1]
    if typ == "decimal" or "double" in typ:
        return "double"
    if "float" in typ:
        return "float"
    if "unsigned" in typ:
        return "uint32"
    if "int" in typ or "short" in typ or "byte" in typ:
        return "int32"
    if typ in ["double", "string"]:
        return typ
    return typ.capitalize()
    
def writeField(protof, use, typ, name, tagNumber):
    if use == "":
        use = "optional"
    protof.write("  %s %s %s = %s;\n" % (use, getProtobufType(typ), name, tagNumber))

def generateProto(root, tagAttrs, depthTags, enums, protodir, base):
    with open(os.path.join(protodir, "%s.proto" % base), 'w') as protof:
        protof.write("package %s;\n" % base)
        for name, enum in enums.iteritems():
            protof.write("\nenum %s {\n" % name.capitalize())
            for idx, entry in enumerate(enum):
                protof.write("  %s = %s;\n" % (entry.upper(), idx))
            protof.write("}\n")
        for tagList in depthTags.itervalues():
            next = 1
            for tags in tagList:
                for tag in tags:
                    protof.write("\nmessage %s {\n" % tag.capitalize())
                    count = 1
                    for a in tagAttrs[tag].itervalues():
                        writeField(protof, a.use, a.type, a.name, count)
                        count += 1
                    if next < len(tagList):
                        for n in tagList[next]:
                            writeField(protof, "repeated", n, n, count)
                            count += 1
                    next += 1
                    protof.write("}\n")
    subprocess.call(["protoc", "%s.proto" % base, "--python_out=%s" % protodir])
    sys.path.append(protodir)
    return importlib.import_module("%s_pb2" % base)

def main():
    options = get_options()
    # get attributes
    attrFinder = xml2csv.AttrFinder(options.xsd, options.source, False)
    base = os.path.basename(options.xsd).split('.')[0]
    # generate proto format description
    module = generateProto(attrFinder.xsdStruc.root.name, attrFinder.tagAttrs, attrFinder.depthTags,
                           attrFinder.xsdStruc._namedEnumerations, options.protodir, base)
    # write proto message
    handler = ProtoWriter(module, attrFinder.tagAttrs, options.output)
    if options.validation:
        schema = lxml.etree.XMLSchema(file=options.xsd)
        parser = lxml.etree.XMLParser(schema=schema)
        tree = lxml.etree.parse(options.source, parser)
        lxml.sax.saxify(tree, handler)
    else:
        xml.sax.parse(options.source, handler)

if __name__ == "__main__":
    main()
