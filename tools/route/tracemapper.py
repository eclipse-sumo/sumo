#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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
import csv
import os
import sys
from collections import OrderedDict
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


def isLineTraceFormat(head, options):
    if any(opt is not None for opt in (options.delimiter, options.id_column, options.x_column, options.y_column)):
        return False
    for line in head.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        if ":" not in line:
            return False
        _, rhs = line.split(":", 1)
        tokens = rhs.split()
        if not tokens:
            return False
        try:
            for tok in tokens:
                parts = tok.split(",")
                if len(parts) != 2:
                    return False
                float(parts[0])
                float(parts[1])
            return True
        except ValueError:
            return False
    return False


def _resolveColumn(spec, header, candidates, defaultIdx=None):
    if spec is not None:
        if header is not None and spec in header:
            return header.index(spec)
        try:
            return int(spec)
        except ValueError:
            sys.exit("Column '%s' not found in CSV header %s" % (spec, header))
    if header is not None:
        lowered = [h.strip().lower() for h in header]
        for cand in candidates:
            if cand in lowered:
                return lowered.index(cand)
        for cand in candidates:
            for idx, col in enumerate(lowered):
                if col.endswith("_" + cand):
                    return idx
        return None
    return defaultIdx


def readCSV(traceFile, net, options):
    """Reads point traces from a CSV file with one location per row."""
    with sumolib.openz(traceFile) as f:
        rows = [line.strip() for line in f if line.strip() and not line.lstrip().startswith("#")]
    if not rows:
        return
    delim = options.delimiter
    if delim is None:
        for candidate in (",", ";", "\t"):
            if candidate in rows[0]:
                delim = candidate
                break
    parsed = list(csv.reader(rows, delimiter=delim)) if delim else [r.split() for r in rows]
    first = parsed[0]

    def _isFloat(val):
        try:
            float(val)
            return True
        except ValueError:
            return False

    hasHeader = not all(_isFloat(c) for c in first)
    if hasHeader and options.id_column is None and options.x_column is None and options.y_column is None:
        if len(first) >= 3 and not _isFloat(first[0]) and _isFloat(first[1]) and _isFloat(first[2]):
            # Headerless "id,x,y" where id is a non-numeric vehicle identifier
            hasHeader = False
    header = [c.strip() for c in first] if hasHeader else None
    dataRows = parsed[1:] if hasHeader else parsed

    idCands = ("id", "track_id", "vehicle_id", "veh_id", "agent_id", "trace_id", "object_id")
    if options.geo:
        xCands = ("lon", "longitude", "x", "pos_x", "vehicle_x")
        yCands = ("lat", "latitude", "y", "pos_y", "vehicle_y")
    else:
        xCands = ("x", "lon", "longitude", "pos_x", "vehicle_x")
        yCands = ("y", "lat", "latitude", "pos_y", "vehicle_y")

    if header is not None:
        idIdx = _resolveColumn(options.id_column, header, idCands, None)
        xIdx = _resolveColumn(options.x_column, header, xCands, None)
        yIdx = _resolveColumn(options.y_column, header, yCands, None)
    elif len(first) == 2:
        idIdx = _resolveColumn(options.id_column, None, idCands, None)
        xIdx = _resolveColumn(options.x_column, None, xCands, 0)
        yIdx = _resolveColumn(options.y_column, None, yCands, 1)
    else:
        idIdx = _resolveColumn(options.id_column, None, idCands, 0)
        xIdx = _resolveColumn(options.x_column, None, xCands, 1)
        yIdx = _resolveColumn(options.y_column, None, yCands, 2)

    if xIdx is None or yIdx is None:
        sys.exit("Could not determine x/y columns in '%s'; please specify --x-column and --y-column." % traceFile)

    tracesById = OrderedDict()
    for row in dataRows:
        if max(xIdx, yIdx, idIdx if idIdx is not None else 0) >= len(row):
            continue
        tid = row[idIdx].strip() if idIdx is not None else "0"
        pos = (float(row[xIdx]), float(row[yIdx]))
        if options.geo:
            pos = net.convertLonLat2XY(*pos)
        tracesById.setdefault(tid, []).append(pos)
    for tid, trace in tracesById.items():
        yield tid, trace


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
    ap.add_argument("--delimiter", default=None,
                    help="CSV field delimiter for one-location-per-line trace files (auto-detected by default)")
    ap.add_argument("--id-column", default=None,
                    help="CSV column name or 0-based index for trace/vehicle ID")
    ap.add_argument("--x-column", default=None,
                    help="CSV column name or 0-based index for x (or longitude with --geo)")
    ap.add_argument("--y-column", default=None,
                    help="CSV column name or 0-based index for y (or latitude with --geo)")
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
            elif isLineTraceFormat(head, options):
                traces = readLines(t, net, options.geo)
            else:
                traces = readCSV(t, net, options)
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
