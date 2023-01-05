#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tracemapper.py
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2013-10-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
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
    """Reads traces from a file in SUMO's fcd-output format.
    The file needs to be sorted by vehicle id rather than by time!"""
    trace = []
    last = None
    for v in sumolib.xml.parse_fast(traceFile, "vehicle", ("id", "x", "y")):
        if last is None:
            last = v.id
        if last != v.id:
            yield last, trace
            trace = []
            last = v.id
        if geo:
            trace.append(net.convertLonLat2XY(float(v.x), float(v.y)))
        else:
            trace.append((float(v.x), float(v.y)))
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
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-n", "--net", help="SUMO network to use", metavar="FILE", required=True)
    optParser.add_option("-t", "--trace",
                         help="trace files to use, separated by comma", metavar="FILE", required=True)
    optParser.add_option("-d", "--delta", default=1., type=float,
                         help="maximum distance between edge and trace points")
    optParser.add_option("-a", "--air-dist-factor", default=2., type=float,
                         help="maximum factor between airline and route distance between successive trace points")
    optParser.add_option("-o", "--output", help="route output", metavar="FILE", required=True)
    optParser.add_option("-p", "--poi-output",
                         help="generate POI output for the trace", metavar="FILE")
    optParser.add_option("-y", "--polygon-output",
                         help="generate polygon output for the mapped edges", metavar="FILE")
    optParser.add_option("--geo", action="store_true",
                         default=False, help="read trace with geo-coordinates")
    optParser.add_option("--direction", action="store_true",
                         default=False, help="try to use direction of consecutive points when mapping")
    optParser.add_option("--vehicle-class", default=None,
                         help="filters the edges by the vehicle class the route is meant for")
    optParser.add_option("--fill-gaps", default=0., type=float,
                         help="repair disconnected routes bridging gaps of up to x meters")
    optParser.add_option("-g", "--gap-penalty", default=-1, type=float,
                         help="penalty to add for disconnected routes " +
                              "(default of -1 adds the distance between the two endpoints as penalty)")
    optParser.add_option("--internal", action="store_true",
                         default=False, help="include internal edges in generated shapes")
    optParser.add_option("--spread", type=float, help="spread polygons laterally to avoid overlap")
    optParser.add_option("--blur", type=float,
                         default=0, help="maximum random disturbance to route geometry")
    optParser.add_option("-l", "--layer", default=100, help="layer for generated polygons")
    optParser.add_option("-b", "--debug", action="store_true",
                         default=False, help="print out the debugging messages")
    options = optParser.parse_args()

    if options.verbose:
        print("Reading net ...")
    net = sumolib.net.readNet(options.net, withInternal=True)

    if options.verbose:
        print("Reading traces ...")

    tracefiles = options.trace.split(',')
    for t in tracefiles:
        if len(tracefiles) == 1:
            outfile = options.output
        else:
            outfile = os.path.basename(t).split('.')[0] + '.tc.xml'
        with open(outfile, "w") as outf:
            sumolib.xml.writeHeader(outf, root='routes')
            poiOut = None
            if options.poi_output is not None:
                if len(tracefiles) == 1:
                    poi_output = options.poi_output
                else:
                    poi_output = os.path.basename(t).split('.')[0] + '.poi.xml'
                poiOut = open(poi_output, "w")
                sumolib.xml.writeHeader(poiOut, root='additional')
            polyOut = None
            if options.polygon_output is not None:
                if len(tracefiles) == 1:
                    polygon_output = options.polygon_output
                else:
                    polygon_output = os.path.basename(t).split('.')[0] + '.poly.xml'
                polyOut = open(polygon_output, "w")
                sumolib.xml.writeHeader(polyOut, root='additional')
                colorgen = sumolib.miscutils.Colorgen(('random', 1, 1))
            # determine file type by reading the first 10000 bytes
            with open(t) as peek:
                head = peek.read(10000)
            if "<poi" in head:
                traces = readPOI(t, net)
            elif "<fcd" in head:
                traces = readFCD(t, net, options.geo)
            else:
                traces = readLines(t, net, options.geo)
            mapOpts = (options.delta, options.verbose, options.air_dist_factor,
                       options.fill_gaps, options.gap_penalty, options.debug, options.direction, options.vehicle_class)
            for tid, trace in traces:
                if poiOut is not None:
                    for idx, pos in enumerate(trace):
                        poiOut.write('<poi id="%s:%s" x="%s" y="%s"/>\n' % (tid, idx, pos[0], pos[1]))
                edges = [e.getID() for e in sumolib.route.mapTrace(
                    trace, net, *mapOpts) if e.getFunction() != "internal"]
                if polyOut is not None and edges:
                    route2poly.generate_poly(options, net, tid, colorgen(), edges, polyOut)
                if edges:
                    outf.write('    <route id="%s" edges="%s"/>\n' % (tid, " ".join(edges)))
                elif options.verbose:
                    print("No edges are found for %s." % (tid))

            outf.write('</routes>\n')
            if poiOut is not None:
                poiOut.write('</additional>\n')
                poiOut.close()
            if polyOut is not None:
                polyOut.write('</additional>\n')
                polyOut.close()
