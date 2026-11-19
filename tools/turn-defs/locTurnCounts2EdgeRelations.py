#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    locTurnCounts2EdgeRelations.py
# @author  Jakob Erdmann
# @date    2026-11-19

"""
Import location-based turn-count CSV records (e.g. "lon,lat, from South, turning right, count")
into SUMO edgeRelation data XML.
"""
from __future__ import absolute_import
from __future__ import print_function

from collections import defaultdict
import csv
import math
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import sumolib  # noqa
from sumolib import geomhelper as gh  # noqa


COMPASS_ORIGIN_DEG = {
    "n": 0.0, "north": 0.0,
    "ne": 45.0, "northeast": 45.0, "north-east": 45.0,
    "e": 90.0, "east": 90.0,
    "se": 135.0, "southeast": 135.0, "south-east": 135.0,
    "s": 180.0, "south": 180.0,
    "sw": 225.0, "southwest": 225.0, "south-west": 225.0,
    "w": 270.0, "west": 270.0,
    "nw": 315.0, "northwest": 315.0, "north-west": 315.0,
}

BOUND_TRAVEL_DEG = {
    "nb": 0.0, "northbound": 0.0,
    "neb": 45.0, "northeastbound": 45.0,
    "eb": 90.0, "eastbound": 90.0,
    "seb": 135.0, "southeastbound": 135.0,
    "sb": 180.0, "southbound": 180.0,
    "swb": 225.0, "southwestbound": 225.0,
    "wb": 270.0, "westbound": 270.0,
    "nwb": 315.0, "northwestbound": 315.0,
}

TURN_CODES = {
    "r": ("r", "R"),
    "right": ("r", "R"),
    "turn right": ("r", "R"),
    "turning right": ("r", "R"),
    "partright": ("R", "r"),
    "slight right": ("R", "r"),
    "turning slight right": ("R", "r"),
    "l": ("l", "L"),
    "left": ("l", "L"),
    "turn left": ("l", "L"),
    "turning left": ("l", "L"),
    "partleft": ("L", "l"),
    "slight left": ("L", "l"),
    "turning slight left": ("L", "l"),
    "s": ("s",),
    "straight": ("s",),
    "through": ("s",),
    "thru": ("s",),
    "ahead": ("s",),
    "going straight": ("s",),
    "t": ("t",),
    "turn": ("t",),
    "uturn": ("t",),
    "u-turn": ("t",),
    "turning around": ("t",),
}


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Import location-based turn counts into edgeRelation XML")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input", required=True, type=ap.net_file,
                    help="Input SUMO network file")
    ap.add_argument("-t", "--turn-file", dest="turnFile", category="input", required=True, type=ap.file,
                    help="Input CSV file with location-based turn counts")
    ap.add_argument("-o", "--output-file", dest="out", category="output", required=True, type=ap.edgedata_file,
                    help="Output edgeRelation XML file")
    ap.add_argument("--geo", action="store_true", default=False,
                    help="Interpret input coordinates as lon,lat instead of network x,y")
    ap.add_argument("--max-distance", dest="maxDist", type=float, default=100.0,
                    help="Maximum distance in meters for matching coordinates to a junction")
    ap.add_argument("--max-angle", dest="maxAngle", type=float, default=67.5,
                    help="Maximum angular deviation in degrees for matching approach edge")
    ap.add_argument("--delimiter", default=",",
                    help="CSV field delimiter (default ',')")
    ap.add_argument("--turn-attribute", dest="turnAttr", default="count",
                    help="Attribute name written on <edgeRelation> elements (default 'count')")
    ap.add_argument("-b", "--begin", default=0, type=ap.time,
                    help="Default interval begin time")
    ap.add_argument("-e", "--end", default=3600, type=ap.time,
                    help="Default interval end time")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Print matching summary")
    return ap.parse_args(args=args)


def parse_travel_bearing(spec):
    cleaned = " ".join(spec.strip().lower().split())
    if cleaned.startswith("from "):
        cleaned = cleaned[5:].strip()
        if cleaned in COMPASS_ORIGIN_DEG:
            return (COMPASS_ORIGIN_DEG[cleaned] + 180.0) % 360.0
    if cleaned in BOUND_TRAVEL_DEG:
        return BOUND_TRAVEL_DEG[cleaned]
    if cleaned in COMPASS_ORIGIN_DEG:
        return (COMPASS_ORIGIN_DEG[cleaned] + 180.0) % 360.0
    try:
        return float(cleaned) % 360.0
    except ValueError:
        return None


def parse_turn_dirs(spec):
    cleaned = " ".join(spec.strip().lower().split())
    return TURN_CODES.get(cleaned)


def edge_approach_bearing(edge):
    shape = edge.getShape()
    for i in range(len(shape) - 1, 0, -1):
        p1, p2 = shape[i - 1], shape[i]
        if p1 != p2:
            return gh.naviDegree(math.atan2(p2[1] - p1[1], p2[0] - p1[0]))
    return None


def find_nearest_junction(candidates, x, y, max_dist):
    best_node = None
    best_dist = max_dist
    for node, nx, ny in candidates:
        d = math.hypot(nx - x, ny - y)
        if d <= best_dist and (best_node is None or (d, node.getID()) < (best_dist, best_node.getID())):
            best_dist = d
            best_node = node
    return best_node, best_dist


def _is_number(val):
    try:
        float(val)
        return True
    except (TypeError, ValueError):
        return False


def iter_records(options):
    with sumolib.openz(options.turnFile) as inf:
        rows = [r for r in csv.reader(inf, delimiter=options.delimiter) if r and any(c.strip() for c in r)]
    if not rows:
        return
    first = [c.strip().lower() for c in rows[0]]
    has_header = not (_is_number(first[0]) and len(first) >= 2 and _is_number(first[1]))
    if has_header:
        col_x = next((i for i, n in enumerate(first) if n in ("x", "lon", "lng", "longitude")), 0)
        col_y = next((i for i, n in enumerate(first) if n in ("y", "lat", "latitude")), 1)
        from_names = ("from", "approach", "origin", "direction", "from_dir")
        cnt_names = (options.turnAttr.lower(), "count", "flows", "volume", "value")
        col_from = next((i for i, n in enumerate(first) if n in from_names), 2)
        col_turn = next((i for i, n in enumerate(first) if n in ("turn", "movement", "turning", "turn_dir")), 3)
        col_cnt = next((i for i, n in enumerate(first) if n in cnt_names), 4)
        col_beg = next((i for i, n in enumerate(first) if n == "begin"), None)
        col_end = next((i for i, n in enumerate(first) if n == "end"), None)
        data_rows = enumerate(rows[1:], 2)
    else:
        col_x, col_y, col_from, col_turn, col_cnt = 0, 1, 2, 3, 4
        col_beg = 5 if len(rows[0]) >= 7 else None
        col_end = 6 if len(rows[0]) >= 7 else None
        data_rows = enumerate(rows, 1)

    for line_no, r in data_rows:
        if len(r) <= max(col_x, col_y, col_from, col_turn, col_cnt):
            print("Warning: Skipping incomplete row at line %d." % line_no, file=sys.stderr)
            continue
        beg = r[col_beg].strip() if col_beg is not None and len(r) > col_beg and r[col_beg].strip() else options.begin
        end = r[col_end].strip() if col_end is not None and len(r) > col_end and r[col_end].strip() else options.end
        yield (
            line_no,
            float(r[col_x].strip()),
            float(r[col_y].strip()),
            r[col_from].strip(),
            r[col_turn].strip(),
            float(r[col_cnt].strip()),
            float(sumolib.miscutils.parseTime(beg)),
            float(sumolib.miscutils.parseTime(end)),
        )


def format_num(val):
    return "%g" % val if float(val).is_integer() else ("%.2f" % val).rstrip("0").rstrip(".")


def main(args=None):
    options = get_options(args=args)
    net = sumolib.net.readNet(options.netfile)
    candidates = [
        (node, node.getCoord()[0], node.getCoord()[1])
        for node in net.getNodes()
        if node.getIncoming() and node.getOutgoing()
    ]

    intervals = defaultdict(lambda: defaultdict(float))
    matched = 0
    skipped = 0

    for line_no, rx, ry, from_spec, turn_spec, count_val, beg, end in iter_records(options):
        if options.geo:
            if not net.hasGeoProj():
                sys.exit("Network '%s' does not provide geo-projection for --geo." % options.netfile)
            x, y = net.convertLonLat2XY(rx, ry)
        else:
            x, y = rx, ry

        node, dist = find_nearest_junction(candidates, x, y, options.maxDist)
        if node is None:
            print("Warning: No junction within %.2fm of (%s, %s) at line %d." % (
                options.maxDist, rx, ry, line_no), file=sys.stderr)
            skipped += 1
            continue

        bearing = parse_travel_bearing(from_spec)
        if bearing is None:
            print("Warning: Unknown approach direction '%s' at line %d." % (from_spec, line_no), file=sys.stderr)
            skipped += 1
            continue

        turn_dirs = parse_turn_dirs(turn_spec)
        if not turn_dirs:
            print("Warning: Unknown turning direction '%s' at line %d." % (turn_spec, line_no), file=sys.stderr)
            skipped += 1
            continue

        best_in = None
        for in_edge in node.getIncoming():
            if in_edge.isSpecial():
                continue
            eb = edge_approach_bearing(in_edge)
            if eb is None:
                continue
            adiff = gh.minAngleDegreeDiff(eb, bearing)
            if adiff <= options.maxAngle:
                key = (adiff, in_edge.getID())
                if best_in is None or key < best_in[0]:
                    best_in = (key, in_edge)

        if best_in is None:
            print("Warning: No incoming edge matching '%s' at junction '%s' (line %d)." % (
                from_spec, node.getID(), line_no), file=sys.stderr)
            skipped += 1
            continue

        in_edge = best_in[1]
        chosen_out = None
        for pref_dir in turn_dirs:
            matches = sorted(
                {conn.getTo().getID() for conns in in_edge.getOutgoing().values()
                 for conn in conns if conn.getDirection() == pref_dir and not conn.getTo().isSpecial()}
            )
            if matches:
                chosen_out = matches[0]
                break

        if chosen_out is None:
            print("Warning: No '%s' connection from edge '%s' at junction '%s' (line %d)." % (
                turn_spec, in_edge.getID(), node.getID(), line_no), file=sys.stderr)
            skipped += 1
            continue

        intervals[(beg, end)][(in_edge.getID(), chosen_out)] += count_val
        matched += 1

    if options.verbose:
        print("Matched %d turn-count records (%d skipped)." % (matched, skipped))

    with sumolib.openz(options.out, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", schemaPath="datamode_file.xsd", options=options)
        for idx, (beg, end) in enumerate(sorted(intervals.keys())):
            outf.write('    <interval id="loc_%d" begin="%s" end="%s">\n' % (
                idx, format_num(beg), format_num(end)))
            for (from_id, to_id), total in sorted(intervals[(beg, end)].items()):
                outf.write('        <edgeRelation from=%s to=%s %s="%s"/>\n' % (
                    sumolib.xml.quoteattr(from_id), sumolib.xml.quoteattr(to_id),
                    options.turnAttr, format_num(total)))
            outf.write('    </interval>\n')
        outf.write('</data>\n')


if __name__ == "__main__":
    main()
