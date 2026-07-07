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

# @file    edgeDataFromFlow.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2020-02-27

"""
This script converts a flow file in csv-format to XML
(generalized meandata format : http://sumo.dlr.de/xsd/meandata_file.xsd)
"""
from __future__ import absolute_import
from __future__ import print_function
import sys
import os

from collections import defaultdict
import detector

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.xml import parse  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    parser = ArgumentParser(description="Convert detector flow file to edgeData format")
    parser.add_argument("-d", "--detector-file", dest="detfile", category="input", required=True,
                        type=ArgumentParser.additional_file,
                        help="read detectors from FILE", metavar="FILE")
    parser.add_argument("-f", "--detector-flow-file", dest="flowfile", category="input", required=True,
                        type=ArgumentParser.file,
                        help="read detector flows to compare to from FILE (mandatory)", metavar="FILE")
    parser.add_argument("--output-file", dest="poiOut", category="output", required=True,
                        type=ArgumentParser.additional_file,
                        help="output flow values as poi params", metavar="FILE")
    parser.add_argument("--id-column", default="Detector", dest="detcol",
                        help="Read detector ids from the given column")
    parser.add_argument("--time-column", default="Time", dest="timecol",
                        help="Read detector time from the given column")
    parser.add_argument("--time-scale", type=int, default=60, dest="timescale",
                        help="Interpretation of time units in seconds (default 60)")
    parser.add_argument("--time-format", dest="timeFormat",
                        help="Format string for interpreting custom time values")
    parser.add_argument("--time-offset", dest="timeOffset",
                        help="time value to subbtract from parsed times")
    parser.add_argument("-q", "--flow-column", dest="flowcol", default="qPKW", type=str,
                        help="which columns contains flows (specified via column header)", metavar="STRING")
    parser.add_argument("-b", "--begin", default=0, type=ArgumentParser.time,
                        help="custom begin time (minutes or H:M:S)")
    parser.add_argument("-e", "--end", default=1440, type=ArgumentParser.time,
                        help="custom end time (minutes or H:M:S)")
    parser.add_argument("-i", "--interval", default=1440, type=ArgumentParser.time,
                        help="custom aggregation interval (minutes or H:M:S)")
    parser.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                        default=False, help="tell me what you are doing")
    options = parser.parse_args(args=args)
    return options


class LaneMap:
    def get(self, key, default):
        return key[0:-2]


def getDetectorPositions(detfile):
    det2pos = {}  # detID -> (laneID, pos)
    for det in sumolib.xml.parse(detfile, ['e1Detector', 'inductionLoop']):
        det2pos[det.id] = (det.lane, det.pos)
    return det2pos


def main(options):
    tMin = None
    tMax = None
    detReader = detector.DetectorReader(options.detfile, LaneMap())
    tMin, tMax = detReader.findTimes(
            options.flowfile, tMin, tMax,
            options.detcol, options.timecol,
            options.timeFormat, options.timeOffset)
    hasData = detReader.readFlows(options.flowfile, flow=options.flowcol, det=options.detcol,
                                  time=options.timecol, timeVal=tMin,
                                  timeMax=tMin + 1000,
                                  timeFormat=options.timeFormat,
                                  timeOffset=options.timeOffset)
    if options.verbose:
        print("found data from minute %s to %s" % (int(tMin), int(tMax)))

    ts = options.timescale
    beginM = int(sumolib.miscutils.parseTime(options.begin, ts) / ts)
    intervalM = int(sumolib.miscutils.parseTime(options.interval, ts) / ts)
    endM = min(int(sumolib.miscutils.parseTime(options.end, ts) / ts), tMax)
    detVals = defaultdict(dict)  # detID : begin : val

    while beginM <= endM:
        iEndM = beginM + intervalM
        edges = defaultdict(dict)  # edge : {attr:val}
        usedGroups = defaultdict(lambda: 0)
        maxGroups = defaultdict(lambda: 0)  # edge : nGroups

        values = detector.parseFlowFile(options.flowfile,
                                        detCol=options.detcol,
                                        timeCol=options.timecol, flowCol=options.flowcol,
                                        begin=beginM, end=iEndM,
                                        timeFormat=options.timeFormat,
                                        timeOffset=options.timeOffset)
        for det, time, flow, speed in values:
            detVals[det][beginM] = flow
        beginM += intervalM

    det2pos = getDetectorPositions(options.detfile)
    with sumolib.openz(options.poiOut, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for det, timeFlows in detVals.items():
            if det in det2pos:
                lane, pos = det2pos[det]
                outf.write('    <poi id="%s" type="%s" lane="%s" pos="%s">\n' % (det, sum(timeFlows.values()), lane, pos))
                for time in sorted(timeFlows.keys()):
                    outf.write('        <param key="%s" value="%s"/>\n' % (time, timeFlows[time]))
                outf.write('    </poi>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
