#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    csv2xml.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2013-12-08


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import csv
import contextlib
import io

from collections import OrderedDict

import xsd
import xml2csv

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options():
    optParser = sumolib.options.ArgumentParser(description="Convert a CSV file to a XML file.")
    optParser.add_argument("source", category="input", type=optParser.data_file,
                           help="the input CSV file")
    optParser.add_argument("-q", "--quotechar", category="processing", default="",
                           help="the quoting character for fields")
    optParser.add_argument("-d", "--delimiter", category="processing", default=";",
                           help="the field separator of the input file")
    optParser.add_argument("-p", "--skip-root", category="processing", action="store_true", default=False,
                           help="the root element is not contained")
    optParser.add_argument("-o", "--output", category="output",
                           help="name for generic output file")
    group = optParser.add_mutually_exclusive_group(required=True)
    group.add_argument("-t", "--type", category="processing",
                       help="convert the given csv-file into the specified format")
    group.add_argument("-x", "--xsd", category="processing",
                       help="xsd schema to use")
    options = optParser.parse_args()
    if not options.output:
        options.output = os.path.splitext(options.source)[0] + ".xml"
    return options


def row2xml(row, tag, close="/>\n", depth=1):
    attrString = ' '.join(['%s="%s"' % (a[len(tag) + 1:], v)
                           for a, v in row.items() if v != "" and a.startswith(tag + "_")])
    return (u'%s<%s %s%s' % ((depth * '    '), tag, attrString, close))


def row2vehicle_and_route(row, tag):
    if "vehicle_route" in row:
        return row2xml(row, tag)
    else:
        edges = row.get("route_edges", "MISSING_VALUE")
        attrs = ' '.join(['%s="%s"' % (a[len(tag) + 1:], v)
                          for a, v in sorted(row.items()) if v != "" and a != "route_edges"])
        return (u'    <%s %s>\n        <route edges="%s"/>\n    </%s>\n' % (tag, attrs, edges, tag))


def write_xml(toptag, tag, options, printer=row2xml):
    with io.open(options.output, 'w', encoding="utf8") as outputf:
        outputf.write(u'<%s>\n' % toptag)
        if options.source.isdigit():
            inputf = xml2csv.getSocketStream(int(options.source))
        else:
            inputf = io.open(options.source, encoding="utf8")
        reader = csv.DictReader(inputf, delimiter=options.delimiter)
        for row in reader:
            orderedRow = OrderedDict([(key, row[key]) for key in reader.fieldnames])
            outputf.write(printer(orderedRow, tag))
        outputf.write(u'</%s>\n' % toptag)
        inputf.close()


def checkAttributes(out, old, new, ele, tagStack, depth):
    for attr in ele.attributes:
        name = "%s_%s" % (ele.name, attr.name)
        if new.get(name, "") != "":
            if depth > 0:
                out.write(u">\n")
            out.write(row2xml(new, ele.name, "", depth))
            return True
    return False


def checkChanges(out, old, new, currEle, tagStack, depth):
    # print(depth, currEle.name, tagStack)
    if depth >= len(tagStack):
        for ele in currEle.children:
            # print(depth, "try", ele.name)
            if ele.name not in tagStack and checkAttributes(out, old, new, ele, tagStack, depth):
                # print(depth, "adding", ele.name, ele.children)
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
            # print(depth, "seeing", ele.name, changed, tagStack)
            if changed:
                out.write(u"/>\n")
                del tagStack[-1]
                while len(tagStack) > depth:
                    out.write(u"%s</%s>\n" % ((len(tagStack) - 1) * '    ', tagStack[-1]))
                    del tagStack[-1]
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
            inputf = io.open(options.source, encoding="utf8")
        lastRow = OrderedDict()
        tagStack = [struct.root.name]
        if options.skip_root:
            outputf.write(u'<%s' % struct.root.name)
        fields = None
        enums = {}
        first = True
        for raw in csv.reader(inputf, delimiter=options.delimiter):
            if not fields:
                fields = raw
                for f in fields:
                    if '_' not in f:
                        continue
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
                    checkAttributes(outputf, lastRow, row, struct.root, tagStack, 0)
                    first = False
                checkChanges(outputf, lastRow, row, struct.root, tagStack, 1)
                lastRow = row
        outputf.write(u"/>\n")
        for idx in range(len(tagStack) - 2, -1, -1):
            outputf.write(u"%s</%s>\n" % (idx * '    ', tagStack[idx]))
        inputf.close()


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
