#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    csv2xml.py
@author  Jakob Erdmann
@date    2013-12-08
@version $Id$

Convert csv files to selected xml input files for SUMO

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,sys
import csv

from collections import defaultdict
from optparse import OptionParser

import xsd

def get_options():
    optParser = OptionParser(usage=os.path.basename(sys.argv[0])+" [<options>] <input_file_or_port>")
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("-q", "--quotechar", default="",
             help="the quoting character for fields")
    optParser.add_option("-d", "--delimiter", 
             help="the field separator of the input file")
    optParser.add_option("-t", "--type", 
             help="convert the given csv-file into the specified format")
    optParser.add_option("-x", "--xsd", help="xsd schema to use")
    optParser.add_option("-o", "--output", help="name for generic output file")
    options, args = optParser.parse_args()
    if len(args) != 1:
        optParser.print_help()
        sys.exit()
    options.source = args[0]
    return options


def row2xml(row, tag):
    return ('    <%s %s/>\n' % (tag,
        ' '.join(['%s="%s"' % (a[len(tag)+1:], v) for a,v in row.items() if v != ""])))

def row2vehicle_and_route(row, tag):
    if "route" in row:
        return row2xml(row, tag)
    else:
        edges = row.get("edges", "MISSING_VALUE")
        return ('    <%s %s>\n        <route edges="%s"/>\n    </%s>\n' % (
            tag, 
            ' '.join(['%s="%s"' % (a, v) for a,v in row.items() if v != ""]),
            edges, tag))

def write_xml(csvfile, toptag, tag, ext, options, printer=row2xml):
    outfilename = os.path.splitext(csvfile)[0] + ext
    dialect = csv.Sniffer().sniff(open(csvfile).read(1024))
    with open(outfilename, 'w') as outputf:
        outputf.write('<%s>\n' % toptag)
        with open(csvfile, 'rb') as inputf:
            for row in csv.DictReader(inputf, dialect=dialect):
                outputf.write(printer(row, tag))
        outputf.write('</%s>\n' % toptag)


def main():
    options = get_options()
    out = sys.stdout
    if options.output:
        out = open(options.output, 'w')
    if options.type in ["nodes", "node", "nod"]:
        write_xml(options.source, 'nodes', 'node', '.nod.xml', options)
    elif options.type in ["edges", "edge", "edg"]:
        write_xml(options.source, 'edges', 'edge', '.edg.xml', options)
    elif options.type in ["connections", "connection", "con"]:
        write_xml(options.source, 'connections', 'connection', '.con.xml', options)
    elif options.type in ["routes", "vehicles", "vehicle", "rou"]:
        write_xml(options.source, 'routes', 'vehicle', '.rou.xml', options,
            row2vehicle_and_route)
    elif options.type in ["flows", "flow"]:
        write_xml(options.source, 'routes', 'flow', '.rou.xml', options,
            row2vehicle_and_route)
    elif options.xsd:
        xsdStruc = xsd.XsdStructure(options.xsd)
        write_xml(options.source, xsdStruc.root.tagText, xsdStruc.root.children[0].tagText, '.xml', options)


if __name__ == "__main__":
    main()
