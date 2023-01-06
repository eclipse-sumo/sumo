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
from lxml import etree as ET

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    import sumolib
    from sumolib.xml import _open, parse_fast
    from sumolib.miscutils import Statistics
    from sumolib.statistics import setPrecision
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("datafiles", nargs="+", help="XML files to analyze")
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("-e", "--element", help="element to analyze")
    optParser.add_option("-a", "--attribute", help="attribute to analyze")
    optParser.add_option("-i", "--id-attribute", dest="idAttr",
                         default="id", help="attribute to identify data elements")
    optParser.add_option("-b", "--binwidth", type=float,
                         default=50, help="binning width of result histogram")
    optParser.add_option("--hist-output", help="output file for histogram (gnuplot compatible)")
    optParser.add_option("-o", "--full-output", help="output file for full data dump")
    optParser.add_option("-x", "--xml-output", help="output statistic to xml file")
    optParser.add_option("--xml-output.flat", action="store_true", dest="xmlFlat",
                         default=False, help="legacy xml output")
    optParser.add_option("-q", "--fast", action="store_true",
                         default=False, help="use fast parser (does not track missing data)")
    optParser.add_option("-p", "--precision", type=int,
                         default=2, help="Set output precision")
    options = optParser.parse_args()

    if options.attribute:
        options.attribute = options.attribute.split(',')
    if options.element:
        options.element = options.element.split(',')

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

    if options.fast:
        assert(len(options.element) == 1)
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

                for _, node in ET.iterparse(_open(datafile, None)):
                    if options.element is not None and node.tag not in options.element:
                        continue
                    elementID = node.get(options.idAttr, defaultID)
                    if options.attribute is None:
                        for k, v in node.items():
                            if k != options.idAttr:
                                yield node.tag, k, v, elementID
                    else:
                        for attr in options.attribute:
                            yield node.tag, attr, node.get(attr), elementID

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
                                               printDev=True)

                stats = allStats[key]
                stats.add(val, elementID)
            except Exception:
                invalidType[attr].add(stringVal)
        else:
            if elementID is not None:
                missingAttr[attr].add(elementID)

    histStyle = 1 if len(allStats) == 1 else 0
    for key in sorted(allStats.keys()):
        print(allStats[key].toString(options.precision, histStyle=histStyle))

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
                    f.write(allStats[key].toXML(options.precision))

            else:
                elemKeys = defaultdict(list)
                for key in allStats.keys():
                    elemKeys[key[0]].append(key)
                for elem in sorted(elemKeys.keys()):
                    f.write('    <%s>\n' % elem)
                    for key in sorted(elemKeys[elem]):
                        attr = key[1]
                        stats = allStats[key]
                        f.write(stats.toXML(options.precision, tag=attr, indent=8, label=''))
                    f.write('    </%s>\n' % elem)
            f.write('</attributeStats>\n')


if __name__ == "__main__":
    main()
