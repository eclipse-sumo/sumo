#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    csv2xml.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2013-12-08
@version $Id$

Convert csv files to selected xml input files for SUMO

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys, csv

from collections import defaultdict
from optparse import OptionParser

import xsd, xml2csv

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0])+" [<options>] <input_file_or_port>")
    optParser.add_option("-q", "--quotechar", default="",
             help="the quoting character for fields")
    optParser.add_option("-d", "--delimiter", default=";",
             help="the field separator of the input file")
    optParser.add_option("-t", "--type", 
             help="convert the given csv-file into the specified format")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-o", "--output", help="name for generic output file")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    if not options.xsd and not options.type:
        print("either a schema or a type needs to be specified", file=sys.stderr)
        sys.exit()
    options.source = args[0]
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".xml"
    return options


def row2xml(row, tag, close="/>\n", depth=1):
    attrString = ' '.join(['%s="%s"' % (a[len(tag)+1:], v) for a,v in row.items() if v != "" and a.startswith(tag)])
    return ('%s<%s %s%s' % ((depth * '    '), tag, attrString, close))

def row2vehicle_and_route(row, tag):
    if "vehicle_route" in row:
        return row2xml(row, tag)
    else:
        edges = row.get("route_edges", "MISSING_VALUE")
        return ('    <%s %s>\n        <route edges="%s"/>\n    </%s>\n' % (
            tag, 
            ' '.join(['%s="%s"' % (a, v) for a,v in row.items() if v != ""]),
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

def checkChanges(out, old, new, currEle, tagStack, depth=1):
    if depth >= len(tagStack):
        for ele in currEle.children:
            found = False
            for attr in ele.attributes:
                name = "%s_%s" % (ele.name, attr.name)
                if new.get(name, "") != "":
                    found = True
                    break
            if found:
                out.write(">\n")
                out.write(row2xml(new, ele.name, "", depth))
                tagStack.append(ele.name)
                break
    else:
        for ele in currEle.children:
            changed = False
            found = False
            for attr in ele.attributes:
                name = "%s_%s" % (ele.name, attr.name)
                if old.get(name, "") != new.get(name, ""):
                    changed = True
                    break
                if new.get(name, "") != "":
                    found = True
            if changed:
                out.write("/>\n")
                tagStack = tagStack[:-1]
                while len(tagStack) > depth:
                    out.write("%s</%s>\n" % ((len(tagStack)-1) * '    ', tagStack[-1]))
                    tagStack = tagStack[:-1]
                out.write(row2xml(new, ele.name, "", depth))
                tagStack.append(ele.name)
                break
            if found:
                break
    if ele.children:
        checkChanges(out, old, new, ele, tagStack, depth+1)


def writeHierarchicalXml(struct, options):
    with xml2csv.getOutStream(options.output) as outputf:
        outputf.write('<%s' % struct.root.name)
        if options.source.isdigit():
            inputf = xml2csv.getSocketStream(int(options.source))
        else:
            inputf = open(options.source)
        lastRow = {}
        tagStack = [struct.root.name]
        for row in csv.DictReader(inputf, delimiter=options.delimiter):
            checkChanges(outputf, lastRow, row, struct.root, tagStack)
            lastRow = row
        outputf.write("/>\n")
        for tag in reversed(tagStack[:-1]):
            outputf.write("</%s>\n" % tag)


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
