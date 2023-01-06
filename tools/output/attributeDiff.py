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
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("-e", "--element", help="element to analyze")
    optParser.add_option("-a", "--attribute", help="attribute to analyze")
    optParser.add_option("-o", "--xml-output", help="output statistic to xml file")
    optParser.add_option("-p", "--precision", type=int,
                         default=2, help="Set output precision")
    options = optParser.parse_args()

    if options.attribute:
        options.attribute = options.attribute.split(',')
    if options.element:
        options.element = options.element.split(',')

    return options


def main():
    options = get_options()

    oldValues = defaultdict(dict)  # elem->attr->difference
    differences = defaultdict(dict)  # elem->attr->difference
    missingAttr = defaultdict(set)
    invalidType = defaultdict(set)

    def elements(fname):
        stack = []
        for event, node in ET.iterparse(sumolib.xml._open(fname, None), events=('start', 'end')):
            if options.element is not None and node.tag not in options.element:
                continue
            if event == 'start':
                stack.append(node.tag)
            else:
                stack.pop()
                continue

            tags = tuple(stack[1:]) if options.element is None else tuple(stack)  # exclude root
            if options.attribute is None:
                for k, v in node.items():
                    yield '.'.join(tags), k, v
            else:
                for attr in options.attribute:
                    if node.get(attr) is not None or options.element is not None:
                        yield '.'.join(tags), attr, node.get(attr)

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
                    missingAttr[attr].add(tag)
            except Exception:
                invalidType[attr].add(stringVal)
        else:
            missingAttr[attr].add(tag)

    if options.verbose or options.xml_output is None:
        for tag in sorted(differences.keys()):
            print("%s: %s" % (tag, ' '.join(["%s=%s" % av for av in sorted(differences[tag].items())])))

    if missingAttr:
        for attr in sorted(missingAttr.keys()):
            print("Elements %s did not provide attribute '%s'" % (','.join(sorted(missingAttr[attr])), attr))

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
