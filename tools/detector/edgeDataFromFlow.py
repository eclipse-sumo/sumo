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
DEBUG = False


def get_options(args=None):
    parser = ArgumentParser(description="Convert detector flow file to edgeData format")
    parser.add_argument("-d", "--detector-file", dest="detfile", category="input", type=ArgumentParser.additional_file,
                        help="read detectors from FILE", metavar="FILE")
    parser.add_argument("-f", "--detector-flow-file", dest="flowfile", category="input", type=ArgumentParser.file,
                        help="read detector flows to compare to from FILE (mandatory)", metavar="FILE")
    parser.add_argument("-n", "--net-file", dest="netfile", category="input", type=ArgumentParser.net_file,
                        help="read network to determine detectors that provide turn counts", metavar="FILE")
    parser.add_argument("-o", "--output-file", dest="output", category="output", type=ArgumentParser.edgedata_file,
                        help="output edgeData FILE (mandatory)", metavar="FILE")
    parser.add_argument("-t", "--turn-output", dest="turnOut", category="output", type=ArgumentParser.edgedata_file,
                        help="output edgeData FILE (mandatory)", metavar="FILE")
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
    parser.add_argument("-q", "--flow-columns", dest="flowcols", default="qPKW,qLKW", type=str,
                        help="which columns contains flows (specified via column header)", metavar="STRING")
    parser.add_argument("-b", "--begin", default=0, type=ArgumentParser.time,
                        help="custom begin time (minutes or H:M:S)")
    parser.add_argument("-e", "--end", default=1440, type=ArgumentParser.time,
                        help="custom end time (minutes or H:M:S)")
    parser.add_argument("-i", "--interval", default=1440, type=ArgumentParser.time,
                        help="custom aggregation interval (minutes or H:M:S)")
    parser.add_argument("-s", "--skip-incomplete", dest="skipIncomplete", action="store_true",
                        default=False, help="Only write edge data if all vehicular lanes have a detector")
    parser.add_argument("--cadyts", action="store_true",
                        default=False, help="generate output in cadyts format")
    parser.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                        default=False, help="tell me what you are doing")
    options = parser.parse_args(args=args)
    if not options.flowfile or not options.output:
        parser.print_help()
        sys.exit()

    if options.turnOut and not options.netfile:
        print("Option --net-file must be set to compute --turn-output", file=sys.stderr)
        sys.exit()
    if options.turnOut and not options.detfile:
        print("Option --net-file must be set to compute --detector-file", file=sys.stderr)
        sys.exit()
    if options.skipIncomplete and not options.netfile:
        print("Option --net-file must be set when using --skip-incomplete", file=sys.stderr)
        sys.exit()

    return options


class LaneMap:
    def get(self, key, default):
        return key[0:-2]


def getDetectorTurns(net, detfile):
    det2turn = {}  # detID -> (fromEdge, toEdge)
    for det in sumolib.xml.parse(detfile, ['e1Detector', 'inductionLoop']):
        lane = net.getLane(det.lane)
        edge = lane.getEdge()
        if len(edge.getOutgoing()) > 1:
            out = lane.getOutgoingEdges()
            if len(out) == 1:
                det2turn[det.id] = (edge.getID(), out[0].getID())
    return det2turn


def expectedLanes(net, edgeID):
    lanes = net.getEdge(edgeID).getLanes()
    return len([lane for lane in lanes if len(lane.getPermissions() & sumolib.net.lane.SUMO_ROAD_MOTOR_CLASSES) > 0])


def main(options):
    flowcols = options.flowcols.split(',')
    tMin = None
    tMax = None
    for i, flowcol in enumerate(flowcols):
        detReader = detector.DetectorReader(options.detfile, LaneMap(),
                                            warnDoubleLane=(i == 0))
        tMin, tMax = detReader.findTimes(
            options.flowfile, tMin, tMax,
            options.detcol, options.timecol,
            options.timeFormat, options.timeOffset)
        hasData = detReader.readFlows(options.flowfile, flow=flowcol, det=options.detcol,
                                      time=options.timecol, timeVal=tMin,
                                      timeMax=tMin + 1000,
                                      timeFormat=options.timeFormat,
                                      timeOffset=options.timeOffset)
        if options.verbose:
            print("flowColumn: %s hasData: %s" % (flowcol, hasData))

    if options.verbose:
        print("found data from minute %s to %s" % (int(tMin), int(tMax)))

    ts = options.timescale
    beginM = int(sumolib.miscutils.parseTime(options.begin, ts) / ts)
    intervalM = int(sumolib.miscutils.parseTime(options.interval, ts) / ts)
    endM = min(int(sumolib.miscutils.parseTime(options.end, ts) / ts), tMax)

    net = None
    if options.netfile:
        net = sumolib.net.readNet(options.netfile)

    skipped = set()
    outf_turn = None
    det2turn = None
    root = "measurements" if options.cadyts else "data"
    if options.turnOut:
        det2turn = getDetectorTurns(net, options.detfile)
        outf_turn = sumolib.openz(options.turnOut, "w")
        sumolib.writeXMLHeader(outf_turn, "$Id$", root, options=options)

    with sumolib.openz(options.output, "w") as outf:
        root = "measurements" if options.cadyts else "data"
        sumolib.writeXMLHeader(outf, "$Id$", root, options=options)
        while beginM <= endM:
            iEndM = beginM + intervalM
            edges = defaultdict(dict)  # edge : {attr:val}
            usedGroups = defaultdict(lambda: 0)
            maxGroups = defaultdict(lambda: 0)  # edge : nGroups

            for flowcol in flowcols:
                detReader = detector.DetectorReader(options.detfile, LaneMap())
                detReader.readFlows(options.flowfile, flow=flowcol, det=options.detcol, time=options.timecol,
                                    timeVal=beginM, timeMax=iEndM,
                                    addDetectors=(options.detfile is None),
                                    timeFormat=options.timeFormat,
                                    timeOffset=options.timeOffset)
                for edge, detData in detReader._edge2DetData.items():
                    maxFlow = 0
                    nGroups = 0
                    for group in detData:
                        if group.isValid:
                            if options.skipIncomplete:
                                if expectedLanes(net, edge) > len(group.lanes):
                                    if (edge, group.pos) not in skipped:
                                        skipped.add((edge, group.pos))
                                        if options.verbose:
                                            print("Skipped group on edge '%s' at pos %s because only %s of %s lanes have detectors" % (  # noqa
                                                edge, group.pos, len(group.lanes), expectedLanes(net, edge)))
                                    continue
                            maxFlow = max(maxFlow, group.totalFlow)
                            nGroups += 1
                    # if options.verbose:
                    #    print("flowColumn: %s edge: %s flow: %s groups: %s" % (
                    #        flowcol, edge, maxFlow, nGroups))
                    if nGroups > 0:
                        edges[edge][flowcol] = maxFlow
                    maxGroups[edge] = max(maxGroups[edge], nGroups)
                    usedGroups[edge] = nGroups

            if options.cadyts:
                for edge in sorted(edges.keys()):
                    print('    <singlelink link="%s" start="%s" end="%s" value="%s" stddev="8" type="COUNT_VEH"/>' %
                          (edge, beginM * ts, iEndM * ts, sum(edges[edge].values())), file=outf)
            else:
                outf.write('    <interval id="flowdata" begin="%s" end="%s">\n' % (beginM * ts, iEndM * ts))
                for edge in sorted(edges.keys()):
                    attrs = ' '.join(['%s="%s"' % (k, v) for k, v in sorted(edges[edge].items())])
                    outf.write('        <edge id="%s" %s groups="%s"/>\n' % (edge, attrs, usedGroups[edge]))
                outf.write('    </interval>\n')

            if outf_turn is not None:
                detVals = defaultdict(lambda: defaultdict(lambda: 0))  # detID : {attr:val}
                for flowcol in flowcols:
                    values = detector.parseFlowFile(options.flowfile,
                                                    detCol=options.detcol,
                                                    timeCol=options.timecol, flowCol=flowcol,
                                                    begin=beginM, end=iEndM,
                                                    timeFormat=options.timeFormat,
                                                    timeOffset=options.timeOffset)
                    for det, time, flow, speed in values:
                        if det in det2turn:
                            detVals[det][flowcol] += flow

                if detVals:
                    outf_turn.write('    <interval id="flowdata" begin="%s" end="%s">\n' % (beginM * ts, iEndM * ts))
                    for det in sorted(detVals.keys()):
                        fromEdge, toEdge = det2turn[det]
                        attrs = ' '.join(['%s="%s"' % (k, v) for k, v in sorted(detVals[det].items())])
                        outf_turn.write('        <edgeRelation from="%s" to="%s" %s/>\n' % (fromEdge, toEdge, attrs))
                    outf_turn.write('    </interval>\n')

            beginM += intervalM
        outf.write('</%s>\n' % root)

    if outf_turn is not None:
        outf_turn.write('</%s>\n' % root)
        outf_turn.close()

    if options.verbose and skipped:
        print("Skipped %s incomplete groups" % (len(skipped)))


if __name__ == "__main__":
    main(get_options())
