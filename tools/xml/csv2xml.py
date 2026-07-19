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

from collections import OrderedDict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import xsd  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Convert a CSV file to a XML file.")
    optParser.add_argument("source", category="input", type=optParser.data_file,
                           help="the input CSV file")
    optParser.add_argument("-o", "--output", category="output", required=True, type=optParser.file,
                           help="name for generic output file")
    optParser.add_argument("-q", "--quotechar", default="",
                           help="the quoting character for fields")
    optParser.add_argument("-d", "--delimiter", default=";",
                           help="the field separator of the input file")
    optParser.add_argument("-p", "--skip-root", action="store_true", default=False,
                           help="the root element is not contained")
    group = optParser.add_mutually_exclusive_group(required=True)
    group.add_argument("-t", "--type",
                       help="convert the given csv-file into the specified format")
    group.add_argument("-x", "--xsd",
                       help="xsd schema to use")
    group.add_argument("--flat", action="store_true", default=False,
                       help="use csv header as flat structure instead of a schema")
    options = optParser.parse_args(args)
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
    with sumolib.openz(options.output, 'w') as outputf:
        outputf.write(u'<%s>\n' % toptag)
        inputf = sumolib.openz(options.source, trySocket=True)
        reader = csv.DictReader(inputf, delimiter=options.delimiter)
        for row in reader:
            orderedRow = OrderedDict([(key, row[key]) for key in reader.fieldnames])
            outputf.write(printer(orderedRow, tag))
        outputf.write(u'</%s>\n' % toptag)
        inputf.close()


def getAttrValue(row, tagStack, depth, tagName, attrName, used=None):
    # Prefer the fully qualified column (ancestorPath_tag_attr).
    longKey = "_".join(tagStack[:depth] + [tagName]) + "_" + attrName
    if longKey in row:
        return row[longKey]
    # Fall back to the unqualified short form (tag_attr).
    # If this value was already "consumed" by another position
    # in this row (see markUsed), treat it as absent here.
    shortKey = tagName + "_" + attrName
    if used is not None and shortKey in used:
        return ""
    return row.get(shortKey, "")


def markUsed(row, tagStack, depth, ele, used):
    # Mark every attribute of ele that was only resolvable via the
    # ambiguous short form (no qualified column present) as consumed, so
    # the same value can't be attributed to another, structurally
    # different position later in this row.
    for attr in ele.attributes:
        longKey = "_".join(tagStack[:depth] + [ele.name]) + "_" + attr.name
        if longKey not in row:
            used.add(ele.name + "_" + attr.name)


def openElement(out, row, ele, tagStack, depth, used):
    if len(tagStack) > depth:
        # Something else is currently open at this depth (or deeper); close
        # it - and everything nested below it - before ele can be opened here.
        out.write(u"/>\n")
        del tagStack[-1]
        while len(tagStack) > depth:
            out.write(u"%s</%s>\n" % ((len(tagStack) - 1) * '    ', tagStack[-1]))
            del tagStack[-1]
    elif depth > 0:
        # Nothing open at this depth, but the parent element's start tag is
        # still unclosed (no ">" written yet) - close it before writing a child.
        out.write(u">\n")
    # Collect only the attributes that actually have a value for this row;
    # attribute values are looked up per-position via tagStack/depth so
    # that qualified columns win over ambiguous short-form ones.
    attrs = ['%s="%s"' % (attr.name, getAttrValue(row, tagStack, depth, ele.name, attr.name))
             for attr in ele.attributes
             if getAttrValue(row, tagStack, depth, ele.name, attr.name) != ""]
    out.write(u'%s<%s %s' % ((depth * '    '), ele.name, ' '.join(attrs)))
    # Record which short-form keys this element consumed, then push it onto
    # the stack as the currently open element at this depth.
    markUsed(row, tagStack, depth, ele, used)
    tagStack.append(ele.name)


def elementPresent(row, tagStack, depth, ele, used):
    # True if at least one attribute of ele has a (non-empty, not already
    # consumed) value in this row - i.e. whether ele should exist at all for this row.
    return any(getAttrValue(row, tagStack, depth, ele.name, attr.name, used) != "" for attr in ele.attributes)


def checkChanges(out, old, new, currEle, tagStack, depth, used):
    for ele in currEle.children:
        if len(tagStack) > depth and tagStack[depth] == ele.name:
            # ele is already open at this depth from the previous row.
            # Determine whether any of its attribute values changed
            # (requiring the element to be closed and reopened) and
            # whether it's present at all in the new row (needed below to
            # decide whether to recurse into its children).
            changed = False
            present = False
            for attr in ele.attributes:
                oldValue = getAttrValue(old, tagStack, depth, ele.name, attr.name)
                newValue = getAttrValue(new, tagStack, depth, ele.name, attr.name, used)
                if oldValue != newValue and newValue != "":
                    changed = True
                if newValue != "":
                    present = True
            if changed:
                openElement(out, new, ele, tagStack, depth, used)
            elif present:
                # Element stays open unchanged; still needs its short-form
                # keys marked as consumed, since openElement (which would
                # normally do this) isn't called in this branch.
                markUsed(new, tagStack, depth, ele, used)
            if present and ele.children:
                checkChanges(out, old, new, ele, tagStack, depth + 1, used)
        elif ele.name not in tagStack[:depth]:
            # ele is not currently open at this depth (and isn't one of
            # its own ancestors, which would indicate a recursive schema).
            # Evaluate presence once and reuse the result for both the
            # open decision and the recursion decision, since openElement
            # mutates `used` as a side effect and re-querying afterwards
            # would give a different (wrong) answer.
            present = elementPresent(new, tagStack, depth, ele, used)
            if present:
                openElement(out, new, ele, tagStack, depth, used)
                if ele.children:
                    checkChanges(out, old, new, ele, tagStack, depth + 1, used)


def writeHierarchicalXml(struct, options):
    if not struct.root.attributes:
        options.skip_root = True
    with contextlib.closing(sumolib.openz(options.output, "w", trySocket=True)) as outputf:
        inputf = sumolib.openz(options.source, trySocket=True)
        lastRow = OrderedDict()
        tagStack = []
        if options.skip_root:
            outputf.write(u'<%s' % struct.root.name)
            tagStack.append(struct.root.name)
        fields = None
        enums = {}
        first = True
        for raw in csv.reader(inputf, delimiter=options.delimiter):
            if not fields:
                fields = raw
                for f in fields:
                    if '_' not in f:
                        continue
                    parts = f.rsplit('_', 2)
                    enum = struct.getEnumerationByAttr(parts[-2], parts[-1])
                    if enum:
                        enums[f] = enum
            else:
                row = OrderedDict()
                for field, entry in zip(fields, raw):
                    if field in enums and entry.isdigit():
                        entry = enums[field][int(entry)]
                    row[field] = entry
                used = set()
                if first and not options.skip_root:
                    openElement(outputf, row, struct.root, tagStack, 0, used)
                    first = False
                checkChanges(outputf, lastRow, row, struct.root, tagStack, 1, used)
                lastRow = row
        outputf.write(u"/>\n")
        for idx in range(len(tagStack) - 2, -1, -1):
            outputf.write(u"%s</%s>\n" % (idx * '    ', tagStack[idx]))
        inputf.close()


def writeFlatXml(options):
    with contextlib.closing(sumolib.openz(options.output, "w", trySocket=True)) as outputf:
        inputf = sumolib.openz(options.source, trySocket=True)
        outputf.write('<data>\n')
        fields = None
        for raw in csv.reader(inputf, delimiter=options.delimiter):
            if not fields:
                fields = raw
            else:
                outputf.write('    <record %s/>\n' % (
                    ' '.join(['%s="%s"' % av for av in zip(fields, raw)])))
        outputf.write('</data>\n')
        inputf.close()


def main(args=None):
    options = get_options(args)
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
    elif options.flat:
        writeFlatXml(options)
    elif options.xsd:
        writeHierarchicalXml(xsd.XsdStructure(options.xsd), options)


if __name__ == "__main__":
    main()
