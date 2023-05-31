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

# @file    attributeCompare.py
# @author  Jakob Erdmann
# @date    2023-04-13

"""
compute statistics on attributes over multiple files files
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
    from sumolib.statistics import Statistics
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    op = sumolib.options.ArgumentParser()
    op.add_option("datafiles", nargs="+", category="input", type=op.data_file,
                  help="XML files to analyze")
    op.add_option("-v", "--verbose", action="store_true", default=False,
                  help="Give more output")
    op.add_option("-e", "--element", help="element to analyze")
    op.add_option("-a", "--attribute", help="attribute to analyze")
    op.add_option("-i", "--id-attribute", dest="idAttribute",
                  help="extra attribute to distinguish values")
    op.add_option("-o", "--xml-output", help="write differences to xml file")
    op.add_option("-p", "--precision", type=int, default=2, help="Set output precision")
    options = op.parse_args()

    if options.attribute:
        options.attribute = options.attribute.split(',')
    if options.idAttribute:
        options.idAttribute = options.idAttribute.split(',')
    if options.element:
        options.element = options.element.split(',')

    return options


def main():
    options = get_options()

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
            jointID = None
            if options.idAttribute:
                for ids in idStack:
                    if ids:
                        jointID = '|'.join(ids)
                        elementDescription += '|' + jointID

            if options.attribute is None:
                for k, v in node.items():
                    yield elementDescription, k, v, jointID
            else:
                for attr in options.attribute:
                    if node.get(attr) is not None or options.element is not None:
                        yield elementDescription, attr, node.get(attr), jointID

    values = defaultdict(lambda: defaultdict(lambda: Statistics(printDev=True)))  # elem->attr->values
    invalidType = defaultdict(set)

    for fname in options.datafiles:
        for tag, attr, stringVal, jointID in elements(fname):
            if stringVal is not None:
                try:
                    if '_' in stringVal:
                        # float() accepts '_' but this doesn't play nice with lane ids
                        raise Exception
                    val = sumolib.miscutils.parseTime(stringVal)
                    values[tag][attr].add(val, jointID)
                except Exception:
                    invalidType[attr].add(stringVal)

    if options.verbose or options.xml_output is None:
        for tag in sorted(values.keys()):
            for attr, stats in sorted(values[tag].items()):
                stats.label = tag + " " + attr
                print(stats)

    if invalidType and options.attribute is not None:
        for attr in sorted(invalidType.keys()):
            sys.stderr.write(("%s distinct values of attribute '%s' could not be interpreted " +
                             "as numerical value or time. Example values: '%s'\n") %
                             (len(invalidType[attr]), attr, "', '".join(sorted(invalidType[attr])[:10])))

    if options.xml_output is not None:
        with open(options.xml_output, 'w') as f:
            root = "attributeCompare"
            sumolib.writeXMLHeader(f, root=root)
            for elem_id in sorted(values.keys()):
                parts = elem_id.split('|')
                elem = parts[0]
                f.write('    <%s' % elem)
                if len(parts) > 1:
                    f.write(' id="%s"' % '|'.join(parts[1:]))
                f.write('>\n')
                for attr, stats in sorted(values[elem_id].items()):
                    f.write(stats.toXML(tag=attr, indent=8, label=''))
                f.write('     </%s>\n' % elem)
            f.write('</%s>\n' % root)


if __name__ == "__main__":
    main()
