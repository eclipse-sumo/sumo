#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkBidiUsage.py
# @author  Mark Colley
# @date    2026-04-12

"""
Discover network sections (edge / bidiEdge pairs) that are used in both directions
by loaded routes, writing edgeData for visualization and printing summary statistics.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys
from collections import defaultdict

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import intIfPossible, parseTime  # noqa
from sumolib.net import lane2edge  # noqa
from sumolib.statistics import Statistics  # noqa

STOPPING_PLACES = ("busStop", "trainStop", "containerStop", "chargingStation", "parkingArea")


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Discover network sections used in both directions and write edgeData")
    ap.add_argument("-n", "--net-file", "--network", category="input", dest="netfile",
                    required=True, type=ap.net_file, help="SUMO network file")
    ap.add_argument("-r", "--route-files", category="input", dest="routeFiles",
                    required=True, type=ap.route_file_list,
                    help="comma-separated input route or vehroute files")
    ap.add_argument("-o", "--output-file", category="output", dest="output",
                    required=True, type=ap.edgedata_file,
                    help="output edgeData XML file")
    ap.add_argument("-a", "--additional-files", category="input", dest="addFiles",
                    type=ap.additional_file_list,
                    help="additional files for resolving stopping place lanes")
    ap.add_argument("--id", default="bidiUsage", category="output",
                    help="interval id in output edgeData")
    ap.add_argument("-b", "--begin", default="0", type=ap.time, category="time",
                    help="interval begin time")
    ap.add_argument("-e", "--end", type=ap.time, category="time",
                    help="interval end time")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)

    options.routeFileList = [f.strip() for f in options.routeFiles.split(",") if f.strip()]
    options.addFileList = [f.strip() for f in (options.addFiles or "").split(",") if f.strip()]
    options.beginSec = parseTime(options.begin)
    options.endSec = parseTime(options.end) if options.end is not None else None
    return options


def load_stopping_places(add_files):
    stop_edge_map = {}
    for afile in add_files:
        for elem in sumolib.xml.parse(afile, list(STOPPING_PLACES)):
            if elem.id and elem.lane:
                stop_edge_map[elem.id] = lane2edge(elem.lane)
    return stop_edge_map


def resolve_stop_edge(stop, stop_edge_map):
    if stop.edge:
        return stop.edge
    if stop.lane:
        return lane2edge(stop.lane)
    for attr in STOPPING_PLACES:
        sp_id = stop.getAttributeSecure(attr, None)
        if sp_id and sp_id in stop_edge_map:
            return stop_edge_map[sp_id]
    return None


def flow_count(elem):
    number = elem.getAttributeSecure("number", None)
    if number is not None:
        return int(number)
    b = parseTime(elem.getAttributeSecure("begin", "0"))
    e = parseTime(elem.getAttributeSecure("end", str(b + 3600.0)))
    dur = max(0.0, e - b)
    vph = elem.getAttributeSecure("vehsPerHour", None)
    if vph is not None and dur > 0:
        return int(round(float(vph) * dur / 3600.0))
    period = elem.getAttributeSecure("period", None)
    if period is not None and parseTime(period) > 0 and dur > 0:
        return int(dur // parseTime(period))
    return 1


def extract_veh_edges_and_stops(elem, standalone_routes):
    stops = list(elem.stop) if isinstance(elem.stop, list) else []
    if isinstance(elem.route, list) and elem.route:
        r = elem.route[0]
        edges = r.edges.split() if r.edges else []
        if isinstance(r.stop, list):
            stops = list(r.stop) + stops
        return edges, stops
    if isinstance(elem.routeDistribution, list) and elem.routeDistribution:
        rlist = elem.routeDistribution[0].route or []
        if rlist:
            last_idx = int(elem.routeDistribution[0].last or 0)
            r = rlist[min(last_idx, len(rlist) - 1)]
            edges = r.edges.split() if r.edges else []
            if isinstance(r.stop, list):
                stops = list(r.stop) + stops
            return edges, stops
    r_id = elem.route if isinstance(elem.route, str) else elem.getAttributeSecure("attr_route", None)
    if r_id and r_id in standalone_routes:
        r_edges, r_stops = standalone_routes[r_id]
        return list(r_edges), list(r_stops) + stops
    return [], stops


def main(options):
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile)
    stop_edge_map = load_stopping_places(options.addFileList)

    standalone_routes = {}
    pass_count = defaultdict(int)
    stop_count = defaultdict(int)
    n_vehs = 0
    max_depart = options.beginSec

    for rfile in options.routeFileList:
        if options.verbose:
            print("Reading routes '%s'" % rfile)
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "route":
                    if elem.id and elem.edges:
                        r_stops = list(elem.stop) if isinstance(elem.stop, list) else []
                        standalone_routes[elem.id] = (elem.edges.split(), r_stops)
                elif elem.name in ("vehicle", "flow"):
                    dep_raw = elem.getAttributeSecure("depart", elem.getAttributeSecure("begin", "0"))
                    if dep_raw not in ("triggered", "containerTriggered"):
                        dep_t = parseTime(dep_raw)
                        if dep_t < options.beginSec:
                            continue
                        if options.endSec is not None and dep_t >= options.endSec:
                            continue
                        max_depart = max(max_depart, dep_t)
                    weight = flow_count(elem) if elem.name == "flow" else 1
                    if weight <= 0:
                        continue
                    edges, stops = extract_veh_edges_and_stops(elem, standalone_routes)
                    if not edges:
                        continue
                    n_vehs += weight
                    for eid in edges:
                        pass_count[eid] += weight
                    for st in stops:
                        st_edge = resolve_stop_edge(st, stop_edge_map)
                        if st_edge:
                            stop_count[st_edge] += weight

    end_out = options.endSec if options.endSec is not None else max(max_depart + 1.0, options.beginSec + 3600.0)

    total_stat = Statistics("Bidi pairs totalCount")
    min_stat = Statistics("Bidi pairs minCount")
    stops_stat = Statistics("Bidi pairs totalStops")
    min_stops_stat = Statistics("Bidi pairs minStops")

    active_edges = []
    seen_pairs = set()
    for edge in sorted(net.getEdges(), key=lambda e: e.getID()):
        if edge.getFunction() == "internal":
            continue
        bidi = edge.getBidi()
        if bidi is None:
            continue
        eid = edge.getID()
        bid = bidi.getID()
        c1 = pass_count[eid]
        c2 = pass_count[bid]
        if c1 > 0 and c2 > 0:
            s1 = stop_count[eid]
            s2 = stop_count[bid]
            tot_c = c1 + c2
            min_c = min(c1, c2)
            tot_s = s1 + s2
            min_s = min(s1, s2)
            active_edges.append((eid, c1, c2, tot_c, min_c, s1, s2, tot_s, min_s))
            pair_key = (min(eid, bid), max(eid, bid))
            if pair_key not in seen_pairs:
                seen_pairs.add(pair_key)
                label = "%s/%s" % pair_key
                total_stat.add(tot_c, label)
                min_stat.add(min_c, label)
                stops_stat.add(tot_s, label)
                min_stops_stat.add(min_s, label)

    with sumolib.openz(options.output, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", schemaPath="datamode_file.xsd", options=options)
        outf.write('    <interval id="%s" begin="%s" end="%s">\n' % (
            options.id, intIfPossible(options.beginSec), intIfPossible(end_out)))
        for eid, c1, c2, tot_c, min_c, s1, s2, tot_s, min_s in active_edges:
            outf.write(
                '        <edge id="%s" count="%d" bidiCount="%d" totalCount="%d" minCount="%d" '
                'stops="%d" bidiStops="%d" totalStops="%d" minStops="%d"/>\n' % (
                    eid, c1, c2, tot_c, min_c, s1, s2, tot_s, min_s)
            )
        outf.write("    </interval>\n")
        outf.write("</data>\n")

    print("Loaded %d vehicles" % n_vehs)
    print("Found %d bidirectional edge pairs used in both directions" % len(seen_pairs))
    if seen_pairs:
        print(total_stat)
        print(min_stat)
        print(stops_stat)
        print(min_stops_stat)


if __name__ == "__main__":
    main(get_options())
