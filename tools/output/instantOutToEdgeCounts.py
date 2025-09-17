#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    instantOutToEdgeCounts.py
# @author  Jakob Erdmann
# @date    2025-09-06

"""
Convert instantInductionLoop output to edge counts
"""

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.net import lane2edge  # noqa


def get_options(args=None):
    ap = ArgumentParser()
    ap.add_argument("-i", "--detector-data", category="input", dest="detdata", required=True, type=ap.file,
                    help="the detector data to read")
    ap.add_argument("-d", "--detector-file", category="input", dest="detfile", type=ap.additional_file,
                    help="the detector definitions to read")
    ap.add_argument("-o", "--output-file", category="output", dest="outfile", type=ap.additional_file,
                    help="define the output filename")
    ap.add_argument("-b", "--begin", type=ap.time,
                    help="begin time")
    ap.add_argument("-e", "--end", type=ap.time,
                    help="end time")
    ap.add_argument("-p", "--period", metavar="FLOAT",
                    help="The interval duration for grouping counts")
    ap.add_argument("-t", "--event-type", dest="eType", default="leave",
                    help="The event type to read (default 'leavel')")
    options = ap.parse_args(args=args)
    return options


def parseDetectors(options):
    det2edge = dict()
    for det in sumolib.xml.parse(options.detfile, "instantInductionLoop"):
        det2edge[det.id] = lane2edge(det.lane)
    return det2edge


def parseTimes(options):
    times = Statistics("times")
    for event in sumolib.xml.parse_fast(options.detdata, 'instantOut', ['id', 'time', 'state']):
        if event.state == options.eType:
            times.add(parseTime(event.time), event.id)
    return times


def writeEdges(outf, edgeCounts):
    for edge in sorted(edgeCounts.keys()):
        outf.write(' ' * 8 + '<edge id="%s" count="%s"/>\n' % (edge, edgeCounts[edge]))
    edgeCounts.clear()
    outf.write(' ' * 4 + '</interval>\n')


def main(options):
    times = parseTimes(options)
    det2edge = parseDetectors(options)

    begin = times.min if options.begin is None else options.begin
    absEnd = times.max if options.end is None else options.end
    period = absEnd - begin if options.period is None else options.period

    end = -1

    with sumolib.openz(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "edgeData", options=options, rootAttrs=None)

        edgeCounts = defaultdict(lambda: 0)

        for event in sumolib.xml.parse_fast(options.detdata, 'instantOut', ['id', 'time', 'state']):
            if event.state == options.eType:
                t = parseTime(event.time)
                if t > end:
                    if end > 0:
                        writeEdges(outf, edgeCounts)
                        begin = end
                    if t > absEnd:
                        break
                    end = begin + period
                    outf.write(' ' * 4 + '<interval id="%s" begin="%s" end="%s">\n' % (
                        options.detdata, begin, begin + period))
                edgeCounts[det2edge[event.id]] += 1
        writeEdges(outf, edgeCounts)
        outf.write('</edgeData>\n')


if __name__ == "__main__":
    options = get_options()
    main(options)
