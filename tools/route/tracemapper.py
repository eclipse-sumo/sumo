#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    tracemapper.py
# @author  Michael Behrisch
# @date    2013-10-23
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

import sumolib  # noqa
import route2poly  # noqa


def readPOI(traceFile, net):
    trace = []
    for poi in sumolib.xml.parse(traceFile, "poi"):
        if poi.lon is None:
            trace.append((poi.x, poi.y))
        else:
            trace.append(net.convertLonLat2XY(poi.lon, poi.lat))
    yield "blub", trace

def readFCD(traceFile, net, geo):
    trace = []
    last = None
    for v in sumolib.xml.parse_fast(traceFile, "vehicle", ("id", "x", "y")):
        if trace and last != v.id:
            yield last, trace
            trace = []
            last = v.id
        if geo:
            trace.append(net.convertLonLat2XY(v.x, v.y))
        else:
            trace.append((v.x, v.y))
    if trace:
        yield last, trace

def readLines(traceFile, net, geo):
    with open(traceFile) as traces:
        for line in traces:
            tid, traceString = line.split(":")
            trace = [tuple(map(float, pos.split(","))) for pos in traceString.split()]
            if geo:
                trace = [net.convertLonLat2XY(*pos) for pos in trace]
            yield tid, trace


if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-n", "--net",
                         help="SUMO network to use (mandatory)", metavar="FILE")
    optParser.add_option("-t", "--trace",
                         help="trace file to use (mandatory)", metavar="FILE")
    optParser.add_option("-d", "--delta", default=1,
                         type="float", help="maximum distance between edge and trace points")
    optParser.add_option("-a", "--air-dist-factor", default=2,
                         type="float", help="maximum factor between airline and route distance between successive trace points")
    optParser.add_option("-o", "--output",
                         help="route output (mandatory)", metavar="FILE")
    optParser.add_option("-p", "--poi-output",
                         help="generate POI output for the trace", metavar="FILE")
    optParser.add_option("-l", "--polygon-output",
                         help="generate polygon output for the mapped edges", metavar="FILE")
    optParser.add_option("--geo", action="store_true",
                         default=False, help="read trace with geo-coordinates")
    optParser.add_option("--fill-gaps", action="store_true",
                         default=False, help="use internal dijkstra to repair disconnected routes")
    optParser.add_option("-g", "--gap-penalty", default=-1,
                         type="float", help="penalty to add for disconnected routes (default of -1 adds the distance between the two endpoints as penalty)")
    (options, args) = optParser.parse_args()

    if not options.output or not options.net:
        optParser.exit("missing input or output")

    if options.verbose:
        print("Reading net ...")
    net = sumolib.net.readNet(options.net, withInternal=True)

    if options.verbose:
        print("Reading traces ...")

    with open(options.output, "w") as outf:
        outf.write('<routes>\n')
        poiOut = None
        if options.poi_output is not None:
            poiOut = open(options.poi_output, "w")
            poiOut.write('<pois>\n')
        polyOut = None
        if options.polygon_output is not None:
            polyOut = open(options.polygon_output, "w")
            polyOut.write('<polygons>\n')
            colorgen = sumolib.miscutils.Colorgen(('random', 1, 1))
        # determine file type by reading the first 10000 bytes
        head = open(options.trace).read(10000)
        if "<poi" in head:
            traces = readPOI(options.trace, net)
        elif "<fcd" in head:
            traces = readFCD(options.trace, net, options.geo)
        else:
            traces = readLines(options.trace, net, options.geo)
        for tid, trace in traces:
            if poiOut is not None:
                for idx, pos in enumerate(trace):
                    poiOut.write('<poi id="%s:%s" x="%s" y="%s"/>\n' % (tid, idx, pos[0], pos[1]))
            edges = [e.getID() for e in sumolib.route.mapTrace(
                trace, net, options.delta, options.verbose, options.air_dist_factor, options.fill_gaps, options.gap_penalty) if e.getFunction() != "internal"]
            if polyOut is not None:
                route2poly.generate_poly(net, tid, colorgen(), 10, True, edges, False, polyOut)
            outf.write('    <route id="%s" edges="%s"/>\n' % (tid, " ".join(edges)))
        outf.write('</routes>\n')
        if poiOut is not None:
            poiOut.write('</pois>\n')
            poiOut.close()
        if polyOut is not None:
            polyOut.write('</polygons>\n')
            polyOut.close()
