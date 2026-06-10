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

from __future__ import absolute_import
from __future__ import print_function
import math
import sys
import os
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

import detector

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
                    help="read detectors from FILE (mandatory)", metavar="FILE")
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
    options = ap.parse_args()
    return options


def main(options):
    counts = defaultdict(lambda : 0)

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
        inputf = sumolib.openz(file)
        reader = csv.DictReader(inputf, delimiter=options.delimiter)
        checkedFields = False
        for row in reader:
            if not checkedFields:
                checkedFields = True
                for attr in ["id", "count"]:
                    colName = getattr(options, attr)
                    if colName not in row:
                        sys.exit("Required column %s not found. Available columns are %s" % (
                            colName, ",".join(row.keys())))

            detID = row[options.id]
            count = float(row[options.count])
            counts[detID] += count
            intervalCounts.add(count, detID)

    for detID, totalCount in counts.items():
        stats.add(totalCount, detID)

    highestCounts = sorted([(c, i) for i, c in counts.items()])

    print(intervalCounts)
    print(stats)

    if options.xml_output is not None:
        with open(options.xml_output, 'w') as f:
            sumolib.writeXMLHeader(f, "$Id$", "flowStats")  # noqa
            f.write(intervalCounts.toXML())
            f.write(stats.toXML())
            for c, i in highestCounts:
                f.write('    <detector ids="%s" count="%s"/>\n' % (i, c))
            f.write('<flowStats>\n')

if __name__ == "__main__":
    main(get_options())
