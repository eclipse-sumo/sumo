#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    attributeDiff.py
# @author  Jakob Erdmann
# @date    2022-12-20

"""
compute difference between attributes in two files
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
from lxml import etree as ET

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("old", help="the first xml file")
    optParser.add_option("new", help="the second xml file")
    optParser.add_option("-v", "--verbose", action="store_true", default=False,
                         help="Give more output")
    optParser.add_option("-e", "--element", help="element to analyze")
    optParser.add_option("-a", "--attribute", help="attribute to analyze")
    optParser.add_option("-i", "--id-attribute", dest="idAttribute",
                         help="extra attribute to distinguish values")
    optParser.add_option("-o", "--xml-output", help="output statistic to xml file")
    optParser.add_option("-p", "--precision", type=int, default=2,
                         help="Set output precision")
    options = optParser.parse_args()

    if options.attribute:
        options.attribute = options.attribute.split(',')
    if options.idAttribute:
        options.idAttribute = options.idAttribute.split(',')
    if options.element:
        options.element = options.element.split(',')

    return options


def main():
    options = get_options()

    oldValues = defaultdict(dict)  # elem->attr->difference
    differences = defaultdict(dict)  # elem->attr->difference
    missingAttr = defaultdict(set)
    missingAttr2 = defaultdict(set)
    invalidType = defaultdict(set)

    def elements(fname):
        stack = []
        idStack = []
        for event, node in ET.iterparse(sumolib.xml._open(fname, None), events=('start', 'end')):
            if options.element is not None and node.tag not in options.element:
                continue
            if event == 'start':
                stack.append(node.tag)
                if options.idAttribute:
                    idStack.append([])
                    for attr in options.idAttribute:
                        if node.get(attr) is not None:
                            idStack[-1].append(node.get(attr))

            else:
                stack.pop()
                if options.idAttribute:
                    idStack.pop()
                continue

            tags = tuple(stack[1:]) if options.element is None else tuple(stack)  # exclude root
            elementDescription = '.'.join(tags)
            if options.idAttribute:
                for ids in idStack:
                    if ids:
                        elementDescription += '.' + '.'.join(ids)

            if options.attribute is None:
                for k, v in node.items():
                    yield elementDescription, k, v
            else:
                for attr in options.attribute:
                    if node.get(attr) is not None or options.element is not None:
                        yield elementDescription, attr, node.get(attr)

    # parse old
    for tag, attr, stringVal in elements(options.old):
        if stringVal is not None:
            try:
                if '_' in stringVal:
                    # float() accepts '_' but this doesn't play nice with lane ids
                    raise Exception
                val = sumolib.miscutils.parseTime(stringVal)
                oldValues[tag][attr] = val
            except Exception:
                invalidType[attr].add(stringVal)
        else:
            missingAttr[attr].add(tag)

    # parse new and compute difference
    for tag, attr, stringVal in elements(options.new):
        if stringVal is not None:
            try:
                if '_' in stringVal:
                    # float() accepts '_' but this doesn't play nice with lane ids
                    raise Exception
                val = sumolib.miscutils.parseTime(stringVal)
                if tag in oldValues and attr in oldValues[tag]:
                    differences[tag][attr] = val - oldValues[tag][attr]
                else:
                    missingAttr2[attr].add(tag)
            except Exception:
                invalidType[attr].add(stringVal)
        else:
            missingAttr2[attr].add(tag)

    if options.verbose or options.xml_output is None:
        for tag in sorted(differences.keys()):
            print("%s: %s" % (tag, ' '.join(["%s=%s" % av for av in sorted(differences[tag].items())])))

    if missingAttr:
        for attr in sorted(missingAttr.keys()):
            print("In file %s, Elements %s did not provide attribute '%s'" % (
                options.old, ','.join(sorted(missingAttr[attr])), attr))

    if missingAttr2:
        for attr in sorted(missingAttr2.keys()):
            print("In file %s, Elements %s did not provide attribute '%s'" % (
                options.new, ','.join(sorted(missingAttr2[attr])), attr))

    if invalidType and options.attribute is not None:
        for attr in sorted(invalidType.keys()):
            print(("%s distinct values of attribute '%s' could not be interpreted " +
                   "as numerical value or time. Example values: '%s'") %
                  (len(invalidType[attr]), attr, "', '".join(sorted(invalidType[attr])[:10])))

    if options.xml_output is not None:
        with open(options.xml_output, 'w') as f:
            sumolib.writeXMLHeader(f, root="attributeDiff")
            for elem in sorted(differences.keys()):
                f.write('    <%s' % elem)
                for attr, d in sorted(differences[elem].items()):
                    f.write(' %s="%s"' % (attr, d))
                f.write('/>\n')
            f.write('</attributeDiff>\n')


if __name__ == "__main__":
    main()
