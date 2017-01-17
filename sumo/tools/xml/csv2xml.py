#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    csv2xml.py
@author  Jakob Erdmann
@author  Michael Behrisch
@author  Laura Bieker
@date    2013-12-08
@version $Id$

Convert csv files to selected xml input files for SUMO

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
PY3 = sys.version_info > (3,)
import csv
import contextlib

from collections import defaultdict, OrderedDict
from optparse import OptionParser

import xsd
import xml2csv


def get_options():
    optParser = OptionParser(
        usage=os.path.basename(sys.argv[0]) + " [<options>] <input_file_or_port>")
    optParser.add_option("-q", "--quotechar", default="",
                         help="the quoting character for fields")
    optParser.add_option("-d", "--delimiter", default=";",
                         help="the field separator of the input file")
    optParser.add_option("-t", "--type",
                         help="convert the given csv-file into the specified format")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-p", "--skip-root", action="store_true",
                         default=False, help="the root element is not contained")
    optParser.add_option("-o", "--output", help="name for generic output file")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if not options.xsd and not options.type:
        print(
            "either a schema or a type needs to be specified", file=sys.stderr)
        sys.exit()
    options.source = args[0]
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".xml"
    return options


def row2xml(row, tag, close="/>\n", depth=1):
    attrString = ' '.join(['%s="%s"' % (a[len(tag) + 1:], v)
                           for a, v in row.items() if v != "" and a.startswith(tag)])
    return ('%s<%s %s%s' % ((depth * '    '), tag, attrString, close))


def row2vehicle_and_route(row, tag):
    if "vehicle_route" in row:
        return row2xml(row, tag)
    else:
        edges = row.get("route_edges", "MISSING_VALUE")
        return ('    <%s %s>\n        <route edges="%s"/>\n    </%s>\n' % (
            tag,
            ' '.join(['%s="%s"' % (a[len(tag) + 1:], v)
                      for a, v in row.items() if v != "" and a != "route_edges"]),
            edges, tag))


def write_xml(toptag, tag, options, printer=row2xml):
    with open(options.output, 'w') as outputf:
        outputf.write('<%s>\n' % toptag)
        if (options.source.isdigit()):
            inputf = getSocketStream(int(options.source))
        else:
            inputf = open(options.source)
        for row in csv.DictReader(inputf, delimiter=options.delimiter):
            outputf.write(printer(row, tag))
        outputf.write('</%s>\n' % toptag)


def checkAttributes(out, old, new, ele, tagStack, depth):
    for attr in ele.attributes:
        name = "%s_%s" % (ele.name, attr.name)
        if new.get(name, "") != "":
            if depth > 0:
                out.write(str.encode(">\n"))
            if(PY3):
                out.write(str.encode(row2xml(new, ele.name, "", depth)))
            else:
                out.write(row2xml(new, ele.name, "", depth))
            return True
    return False


def checkChanges(out, old, new, currEle, tagStack, depth):
    #print(depth, currEle.name, tagStack)
    if depth >= len(tagStack):
        for ele in currEle.children:
            #print(depth, "try", ele.name)
            if ele.name not in tagStack and checkAttributes(out, old, new, ele, tagStack, depth):
                #print(depth, "adding", ele.name, ele.children)
                tagStack.append(ele.name)
                if ele.children:
                    checkChanges(out, old, new, ele, tagStack, depth + 1)
    else:
        for ele in currEle.children:
            if ele.name in tagStack and tagStack.index(ele.name) != depth:
                continue
            changed = False
            present = False
            for attr in ele.attributes:
                name = "%s_%s" % (ele.name, attr.name)
                if old.get(name, "") != new.get(name, "") and new.get(name, "") != "":
                    changed = True
                if new.get(name, "") != "":
                    present = True
            #print(depth, "seeing", ele.name, changed, tagStack)
            if changed:
                out.write(str.encode("/>\n"))
                del tagStack[-1]
                while len(tagStack) > depth:
                    if(PY3):
                        out.write(str.encode("%s</%s>\n" %
                                             ((len(tagStack) - 1) * '    ', tagStack[-1])))
                    else:
                        out.write("%s</%s>\n" %
                                  ((len(tagStack) - 1) * '    ', tagStack[-1]))
                    del tagStack[-1]
                if(PY3):
                    out.write(str.encode(row2xml(new, ele.name, "", depth)))
                else:
                    out.write(row2xml(new, ele.name, "", depth))
                tagStack.append(ele.name)
                changed = False
            if present and ele.children:
                checkChanges(out, old, new, ele, tagStack, depth + 1)


def writeHierarchicalXml(struct, options):
    if not struct.root.attributes:
        options.skip_root = True
    with contextlib.closing(xml2csv.getOutStream(options.output)) as outputf:
        if options.source.isdigit():
            inputf = xml2csv.getSocketStream(int(options.source))
        else:
            inputf = open(options.source)
        lastRow = OrderedDict()
        tagStack = [struct.root.name]
        if options.skip_root:
            if(PY3):
                outputf.write(str.encode('<%s' % struct.root.name))
            else:
                outputf.write('<%s' % struct.root.name)
        fields = None
        enums = {}
        first = True
        for raw in csv.reader(inputf, delimiter=options.delimiter):
            if not fields:
                fields = raw
                for f in fields:
                    enum = struct.getEnumerationByAttr(*f.split('_', 1))
                    if enum:
                        enums[f] = enum
            else:
                row = OrderedDict()
                for field, entry in zip(fields, raw):
                    if field in enums and entry.isdigit():
                        entry = enums[field][int(entry)]
                    row[field] = entry
                if first and not options.skip_root:
                    checkAttributes(
                        outputf, lastRow, row, struct.root, tagStack, 0)
                    first = False
                checkChanges(outputf, lastRow, row, struct.root, tagStack, 1)
                lastRow = row
        outputf.write(str.encode("/>\n"))
        for idx in range(len(tagStack) - 2, -1, -1):
            if(PY3):
                outputf.write(
                    str.encode("%s</%s>\n" % (idx * '    ', tagStack[idx])))
            else:
                outputf.write("%s</%s>\n" % (idx * '    ', tagStack[idx]))


def main():
    options = get_options()
    if options.type in ["nodes", "node", "nod"]:
        write_xml('nodes', 'node', options)
    elif options.type in ["edges", "edge", "edg"]:
        write_xml('edges', 'edge', options)
    elif options.type in ["connections", "connection", "con"]:
        write_xml('connections', 'connection', options)
    elif options.type in ["routes", "vehicles", "vehicle", "rou"]:
        write_xml('routes', 'vehicle', options, row2vehicle_and_route)
    elif options.type in ["flows", "flow"]:
        write_xml('routes', 'flow', options, row2vehicle_and_route)
    elif options.xsd:
        writeHierarchicalXml(xsd.XsdStructure(options.xsd), options)


if __name__ == "__main__":
    main()
