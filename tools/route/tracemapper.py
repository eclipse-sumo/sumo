#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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
            yield tid.strip(), trace


if __name__ == "__main__":
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-v", "--verbose", action="store_true",
                    default=False, help="tell me what you are doing")
    ap.add_argument("-n", "--net", help="SUMO network to use", category="input",
                    type=ap.net_file, metavar="FILE", required=True)
    ap.add_argument("-t", "--trace", category="input", type=ap.file,
                    help="trace files to use, separated by comma", metavar="FILE", required=True)
    ap.add_argument("-d", "--delta", default=1., type=float,
                    help="maximum distance between edge and trace points")
    ap.add_argument("-a", "--air-dist-factor", default=2., type=float,
                    help="maximum factor between airline and route distance between successive trace points")
    ap.add_argument("-o", "--output", help="route output", metavar="FILE", required=True)
    ap.add_argument("-p", "--poi-output", category="output", type=ap.file,
                    help="generate POI output for the trace", metavar="FILE")
    ap.add_argument("-y", "--polygon-output", category="output", type=ap.file,
                    help="generate polygon output for the mapped edges", metavar="FILE")
    ap.add_argument("--geo", action="store_true",
                    default=False, help="read trace with geo-coordinates")
    ap.add_argument("--direction", action="store_true",
                    default=False, help="try to use direction of consecutive points when mapping")
    ap.add_argument("--vehicle-class", default=None,
                    help="filters the edges by the vehicle class the route is meant for")
    ap.add_argument("--fill-gaps", default=0., type=float,
                    help="repair disconnected routes bridging gaps of up to x meters")
    ap.add_argument("-g", "--gap-penalty", default=-1, type=float,
                    help="penalty to add for disconnected routes " +
                    "(default of -1 adds the distance between the two endpoints as penalty)")
    ap.add_argument("--internal", action="store_true",
                    default=False, help="include internal edges in generated shapes")
    ap.add_argument("--spread", type=float, help="spread polygons laterally to avoid overlap")
    ap.add_argument("--blur", type=float,
                    default=0, help="maximum random disturbance to route geometry")
    ap.add_argument("-l", "--layer", default=100, help="layer for generated polygons")
    ap.add_argument("-b", "--debug", action="store_true",
                    default=False, help="print out the debugging messages")
    options = ap.parse_args()

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
                        poiOut.write('    <poi id="%s:%s" x="%s" y="%s"/>\n' % (tid, idx, pos[0], pos[1]))
                edges = [e.getID() for e in sumolib.route.mapTrace(
                    trace, net, *mapOpts) if e.getFunction() != "internal"]
                edges = [edge for i, edge in enumerate(edges) if i == 0 or edge != edges[i-1]]
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
