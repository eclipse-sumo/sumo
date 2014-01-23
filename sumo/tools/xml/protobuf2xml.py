#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    protobuf2xml.py
@author  Michael Behrisch
@date    2014-01-23
@version $Id$

Convert protobuf files to selected xml input files for SUMO

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, struct

from optparse import OptionParser
import google.protobuf.descriptor
import xsd, xml2csv, xml2protobuf

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0])+" [<options>] <input_file_or_port>")
    optParser.add_option("-p", "--protodir", default=".",
                         help="where to put and read .proto files")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-o", "--output", help="name for generic output file")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    options.source = args[0]
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".xml"
    return options


def row2xml(row, tag, close="/>\n", depth=1):
    attrString = ' '.join(['%s="%s"' % (a[len(tag)+1:], v) for a,v in row.items() if v != "" and a.startswith(tag)])
    return ('%s<%s %s%s' % ((depth * '    '), tag, attrString, close))

def read_n(inputf, n):
    """ Read exactly n bytes from the input.
        Raise RuntimeError if the stream andedbefore
        n bytes were read.
    """
    buf = ''
    while n > 0:
        data = inputf.read(n)
        if data == '':
            raise RuntimeError('unexpected connection close')
        buf += data
        n -= len(data)
    return buf

def msg2xml(obj, out, depth=1):
    for desc, cont in obj.ListFields():
        out.write("%s<%s" % (depth*'    ', desc.name))
        for attr, value in cont.ListFields():
            print attr.name, attr.type, attr.label
            if attr.type == google.protobuf.descriptor.FieldDescriptor.TYPE_MESSAGE:
                if attr.label == google.protobuf.descriptor.FieldDescriptor.LABEL_REPEATED:
                    for item in value:
                        msg2xml(item, out, depth+1)
            else:
                out.write(" %s=%s" % (attr.name, value))

def writeXml(root, module, options):
    with open(options.output, 'w') as outputf:
        outputf.write('<%s>\n' % root)
        if (options.source.isdigit()):
            inputf = xml2csv.getSocketStream(int(options.source))
        else:
            inputf = open(options.source, 'rb')
        while True:
            length = struct.unpack('>L', read_n(inputf, 4))[0]
            if length == 0:
                break
            obj = vars(module)[root.capitalize()]()
            obj.ParseFromString(read_n(inputf, length))
            msg2xml(obj, outputf)
        inputf.close()
        outputf.write("</%s>\n" % root)


def main():
    options = get_options()
    # get attributes
    attrFinder = xml2csv.AttrFinder(options.xsd, options.source)
    base = os.path.basename(options.source).split('.')[0]
    # generate proto format description
    module = xml2protobuf.generateProto(attrFinder.xsdStruc.root.name, attrFinder.tagAttrs, attrFinder.depthTags, options.protodir, base)
    writeXml(attrFinder.xsdStruc.root.name, module, options)


if __name__ == "__main__":
    main()
