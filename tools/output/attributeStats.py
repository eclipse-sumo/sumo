#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2014-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    attributeStats.py
# @author  Jakob Erdmann
# @date    2019-04-11

"""
compute statistics for a specific xml attribute (e.g. timeLoss in tripinfo-output)
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from collections import defaultdict
try:
    from lxml import etree as ET
except ImportError:
    import xml.etree.ElementTree as ET

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa
from sumolib.xml import parse_fast  # noqa
from sumolib.miscutils import Statistics, humanReadableTime  # noqa
from sumolib.statistics import setPrecision, identity  # noqa


def find_elements(datafiles, attributes):
    attr2elem = {}
    elem2level = {}
    for fname in datafiles:
        level = 0
        with sumolib.openz(fname, 'rb') as xmlf:
            for event, elem in ET.iterparse(xmlf, events=("start", "end")):
                if event == "start":
                    level += 1
                    for attr in attributes:
                        if attr in elem.keys():
                            elem2level[elem.tag] = level
                            if attr in attr2elem:
                                oldTag = attr2elem[attr]
                                if oldTag != elem.tag:
                                    if elem2level[oldTag] < level:
                                        attr2elem[attr] = elem.tag
                                    print("Warning: found attribute '%s' in element '%s' (level %s) and "
                                          "element '%s' (level %s). Using '%s'." %
                                          (attr, oldTag, elem2level[oldTag], elem.tag, level, attr2elem[attr]),
                                          file=sys.stderr)
                            else:
                                attr2elem[attr] = elem.tag
                    if len(attr2elem) == len(attributes):
                        return attr2elem
                elif event == "end":
                    level -= 1
                    elem.clear()
    return attr2elem


def get_options():
    op = sumolib.options.ArgumentParser()
    op.add_argument("datafiles", nargs="+", category="input", type=op.file,
                    help="XML files to analyze")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Give more output")
    op.add_argument("-e", "--element", category="input",
                    help="element to analyze")
    op.add_argument("-a", "--attribute", category="input",
                    help="attribute to analyze")
    op.add_argument("-i", "--id-attribute", dest="idAttr", default="id", category="input",
                    help="attribute to identify data elements")
    op.add_argument("-b", "--binwidth", type=float, default=50, category="processing",
                    help="binning width of result histogram")
    op.add_argument("--hist-output", category="output",
                    help="output file for histogram (gnuplot compatible)")
    op.add_argument("-o", "--full-output", category="output",
                    help="output file for full data dump")
    op.add_argument("-x", "--xml-output", category="output", type=op.file,
                    help="output statistic to xml file")
    op.add_argument("--xml-output.flat", action="store_true", dest="xmlFlat", default=False, category="output",
                    help="legacy xml output")
    op.add_argument("-q", "--fast", action="store_true", default=False, category="processing",
                    help="use fast parser (does not track missing data)")
    op.add_argument("-p", "--precision", type=int, default=2, category="output",
                    help="Set output precision")
    op.add_argument("--abs", default=False, action="store_true",
                    help="include statistics on absolute values")
    op.add_argument("--sum", default=False, action="store_true",
                    help="include sum of values")
    op.add_argument("-H", "--human-readable-time", dest="hrTime", default=False, action="store_true", category="output",
                    help="interpret values as times and write them as h:m:s")
    options = op.parse_args()

    if options.attribute:
        options.attribute = options.attribute.split(',')
    options.attr2elem = None
    if options.element:
        options.element = options.element.split(',')
    elif options.attribute:
        options.attr2elem = find_elements(options.datafiles, options.attribute)
        if options.attr2elem:
            options.element = sorted(set(options.attr2elem.values()))

    if options.fast:
        if options.attribute is None:
            print("Parsing all attributes is not supported when using option --fast")
            sys.exit()
        if options.element is None:
            print("Parsing all elements is not supported when using option --fast")
            sys.exit()
        if len(options.element) > 1:
            print("Parsing multiple elements is not supported when using option --fast")
            sys.exit()

    return options


def main():
    options = get_options()

    vals = defaultdict(lambda: defaultdict(list))
    allStats = dict()
    missingAttr = defaultdict(set)
    invalidType = defaultdict(set)
    formatter = humanReadableTime if options.hrTime else identity

    if options.fast:
        assert len(options.element) == 1
        elem = options.element[0]

        def elements():
            for datafile in options.datafiles:
                for element in parse_fast(datafile, elem, [options.idAttr] + options.attribute):
                    for attr in options.attribute:
                        yield elem, attr, getattr(element, attr), getattr(element, options.idAttr),
    else:
        def elements():
            for datafile in options.datafiles:
                defaultID = None if len(options.datafiles) == 1 else datafile

                with sumolib.openz(datafile, 'rb') as f:
                    for _, node in ET.iterparse(f):
                        if options.element is not None and node.tag not in options.element:
                            node.clear()
                            continue
                        elementID = node.get(options.idAttr, defaultID)
                        if options.attribute is None:
                            for k, v in node.items():
                                if k != options.idAttr:
                                    yield node.tag, k, v, elementID
                        else:
                            for attr in options.attribute:
                                if (options.attr2elem and attr in options.attr2elem
                                        and options.attr2elem[attr] != node.tag):
                                    continue
                                yield node.tag, attr, node.get(attr), elementID
                        node.clear()

    for tag, attr, stringVal, elementID in elements():
        if stringVal is not None:
            try:
                if '_' in stringVal:
                    # float() accepts '_' but this doesn't play nice with lane ids
                    raise Exception
                val = sumolib.miscutils.parseTime(stringVal)
                vals[elementID][attr].append(val)
                key = (tag, attr)
                if key not in allStats:
                    allStats[key] = Statistics("%s %s" % (tag, attr),
                                               histogram=options.binwidth > 0, scale=options.binwidth,
                                               printDev=True, abs=options.abs, printSum=options.sum)

                stats = allStats[key]
                stats.add(val, elementID)
            except Exception:
                invalidType[attr].add(stringVal)
        else:
            if elementID is not None:
                missingAttr[attr].add(elementID)

    histStyle = 1 if len(allStats) == 1 else 0
    for key in sorted(allStats.keys()):
        print(allStats[key].toString(options.precision, histStyle=histStyle, fmt=formatter))

    if missingAttr:
        for attr in sorted(missingAttr.keys()):
            print("%s elements did not provide attribute '%s' Example ids: '%s'" %
                  (len(missingAttr[attr]), attr, "', '".join(sorted(missingAttr[attr])[:10])))

    if invalidType and options.attribute is not None:
        for attr in sorted(invalidType.keys()):
            print(("%s distinct values of attribute '%s' could not be interpreted " +
                   "as numerical value or time. Example values: '%s'") %
                  (len(invalidType[attr]), attr, "', '".join(sorted(invalidType[attr])[:10])))

    if options.hist_output is not None:
        for key in sorted(allStats.keys()):
            fname = options.hist_output if len(allStats) == 1 else options.hist_output + ".%s.%s" % key
            with open(fname, 'w') as f:
                for bin, count in allStats[key].histogram():
                    f.write("%s %s\n" % (bin, count))

    if options.full_output is not None:
        with open(options.full_output, 'w') as f:
            for elementID in sorted(vals.keys()):
                for attr, data in vals[elementID].items():
                    if len(vals[elementID]) > 1:
                        f.write("# %s\n" % attr)
                    for x in data:
                        f.write(setPrecision("%.2f %s\n", options.precision) % (x, elementID))

    if options.xml_output is not None:
        with open(options.xml_output, 'w') as f:
            sumolib.writeXMLHeader(f, "$Id$", "attributeStats")  # noqa
            if options.xmlFlat:
                for key in sorted(allStats.keys()):
                    f.write(allStats[key].toXML(options.precision, fmt=formatter))

            else:
                elemKeys = defaultdict(list)
                for key in allStats.keys():
                    elemKeys[key[0]].append(key)
                for elem in sorted(elemKeys.keys()):
                    f.write('    <%s>\n' % elem)
                    for key in sorted(elemKeys[elem]):
                        attr = key[1]
                        stats = allStats[key]
                        f.write(stats.toXML(options.precision, tag=attr, indent=8, label='', fmt=formatter))
                    f.write('    </%s>\n' % elem)
            f.write('</attributeStats>\n')


if __name__ == "__main__":
    main()
