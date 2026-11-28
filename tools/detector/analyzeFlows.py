#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    analyzeFlows.py
# @author  Jakob Erdmann
# @date    2026-06-10

"""
This script computes statistics on detector flow files. It's main use is to find
anomalies (i.e. individual data points that are suspiciously high or detectors
that are high on average). The following statistics are collected:

- all data received on all detectors
- the total flow counted for each detector
- a sorted list of total flow by each detector (if option --xml-output is given)
"""

from __future__ import absolute_import
from __future__ import print_function
import sys
import os
import csv
from collections import defaultdict

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
# the following patches matplotlib for python 3.14
from sumolib.visualization import helpers  # noqa
from sumolib.statistics import Statistics  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("flowfiles", nargs="+", category="input", type=ap.file, metavar="FILE+",
                    help="read flows from FILE(s) (mandatory)")
    ap.add_argument("-d", "--detector-file", dest="detfile", category="input", type=ap.additional_file,
                    help="read detectors from FILE", metavar="FILE")
    ap.add_argument("-i", "--id-column", default="Detector", dest="id",
                    help="Read detector ids from the given column")
    ap.add_argument("--flow-column", dest="count", default="qPKW", type=str,
                    help="which column contains flows", metavar="STRING")
    ap.add_argument("--delimiter", default=";",
                    help="the field separator of the detector input file")
    ap.add_argument("-s", "--show", action="store_true", default=False,
                    help="show plot directly")
    ap.add_argument("-b", "--binwidth", type=float, default=1000, category="processing",
                    help="binning width of result histogram")
    ap.add_argument("-o", "--xml-output", category="output", type=ap.file,
                    help="output statistic to xml file")
    options = ap.parse_args(args=args)
    return options


def main(options):
    counts = defaultdict(lambda: 0)
    knownDetectors = None
    if options.detfile is not None:
        knownDetectors = set()
        for detfile in options.detfile.split(','):
            for det in sumolib.xml.parse(detfile, ['detectorDefinition', 'inductionLoop', 'e1Detector']):
                knownDetectors.add(det.id)

    stats = Statistics("Total detector counts",
                       printDev=True,
                       histogram=options.binwidth > 0,
                       scale=options.binwidth,
                       printSum=True)

    intervalCounts = Statistics("interval counts",
                                printDev=True,
                                histogram=options.binwidth > 0,
                                scale=options.binwidth,
                                printSum=True)

    for file in options.flowfiles:
        with sumolib.openz(file) as inputf:
            reader = csv.DictReader(inputf, delimiter=options.delimiter)
            checkedFields = False
            for lineNo, row in enumerate(reader, 2):
                if not checkedFields:
                    checkedFields = True
                    for attr in ["id", "count"]:
                        colName = getattr(options, attr)
                        if colName not in row:
                            sys.exit("Required column %s not found. Available columns are %s" % (
                                colName, ",".join(row.keys())))

                detID = row[options.id]
                rawCount = row[options.count]
                if rawCount is None or rawCount.strip() == "":
                    print("Warning: Missing flow value for detector '%s' in '%s' line %d." % (
                        detID, file, lineNo), file=sys.stderr)
                    continue
                try:
                    count = float(rawCount)
                except ValueError:
                    print("Warning: Invalid flow value '%s' for detector '%s' in '%s' line %d." % (
                        rawCount, detID, file, lineNo), file=sys.stderr)
                    continue
                counts[detID] += count
                intervalCounts.add(count, detID)

    if knownDetectors is not None:
        unknown = sorted(set(counts.keys()) - knownDetectors)
        if unknown:
            print("Warning: %d detectors in flow file are not in detector file: %s" % (
                len(unknown), ",".join(unknown)), file=sys.stderr)
        unused = sorted(knownDetectors - set(counts.keys()))
        if unused:
            print("Warning: %d detectors in detector file have no flow data: %s" % (
                len(unused), ",".join(unused)), file=sys.stderr)

    for detID, totalCount in counts.items():
        stats.add(totalCount, detID)

    highestCounts = sorted([(c, i) for i, c in counts.items()])

    print(intervalCounts)
    print(stats)

    if options.xml_output is not None:
        with open(options.xml_output, 'w') as f:
            sumolib.xml.writeHeader(f, root="flowStats", rootAttrs=None, options=options)
            f.write(intervalCounts.toXML())
            f.write(stats.toXML())
            for c, i in highestCounts:
                f.write('    <detector ids=%s count="%s"/>\n' % (sumolib.xml.quoteattr(i), c))
            f.write('</flowStats>\n')


if __name__ == "__main__":
    main(get_options())
