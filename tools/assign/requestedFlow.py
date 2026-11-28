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

# @file    requestedFlow.py
# @author  Mark Colley
# @date    2026-04-12

"""
Compute unrestricted ("requested") traffic demand per edge and time interval from
route files (using exitTimes if available, or free-flow travel times otherwise)
and compare it against edge capacity.
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
from sumolib.statistics import Statistics  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Compute requested flow and volume-to-capacity ratio per edge and interval")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input", type=ap.net_file,
                    required=True, help="SUMO network file")
    ap.add_argument("-r", "--route-files", dest="routefiles", category="input",
                    type=ap.file_list, required=True,
                    help="comma-separated input route file(s) (optionally with exitTimes)")
    ap.add_argument("-o", "--output-file", dest="outfile", category="output",
                    type=ap.edgedata_file, required=True,
                    help="output edgeData XML file")
    ap.add_argument("-i", "--interval", default="3600", type=ap.time, category="time",
                    help="time aggregation interval in seconds or H:M:S (default 3600)")
    ap.add_argument("-b", "--begin", default="0", type=ap.time, category="time",
                    help="begin time in seconds or H:M:S")
    ap.add_argument("-e", "--end", type=ap.time, category="time",
                    help="end time in seconds or H:M:S")
    ap.add_argument("--id", default="requestedFlow", category="output",
                    help="interval id in output edgeData")
    ap.add_argument("--lane-capacity", dest="laneCap", type=float, default=1800.0,
                    category="processing",
                    help="capacity per usable lane in veh/h (default 1800)")
    ap.add_argument("--vclass", default="passenger", category="processing",
                    help="vClass for counting usable lanes on each edge (empty string for all)")
    ap.add_argument("--speed-factor", dest="speedFactor", type=float, default=1.0,
                    category="processing",
                    help="speed factor when extrapolating free-flow entry times without exitTimes")
    ap.add_argument("--min-ratio", dest="minRatio", type=float, default=0.0,
                    category="processing",
                    help="only output edges with vcRatio >= FLOAT")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)

    options.routeFileList = [f.strip() for f in options.routefiles.split(",") if f.strip()]
    options.beginSec = parseTime(options.begin)
    options.endSec = parseTime(options.end) if options.end is not None else None
    options.intervalSec = parseTime(options.interval)
    return options


def expand_flow_departures(elem):
    begin = parseTime(elem.getAttributeSecure("begin", "0"))
    end_raw = elem.getAttributeSecure("end", None)
    end = parseTime(end_raw) if end_raw is not None else begin + 3600.0
    number = elem.getAttributeSecure("number", None)
    period = elem.getAttributeSecure("period", None)
    vph = elem.getAttributeSecure("vehsPerHour", None)

    if number is not None:
        n = int(number)
        if n <= 0:
            return []
        step = (end - begin) / n if n > 1 and end > begin else 0.0
        return [begin + i * step for i in range(n)]
    if period is not None and parseTime(period) > 0:
        dt = parseTime(period)
        res = []
        t = begin
        while t < end:
            res.append(t)
            t += dt
        return res
    if vph is not None and float(vph) > 0:
        dt = 3600.0 / float(vph)
        res = []
        t = begin
        while t < end:
            res.append(t)
            t += dt
        return res
    return [begin]


def extract_route_and_exittimes(elem, standalone_routes):
    if isinstance(elem.route, list) and elem.route:
        r = elem.route[0]
        edges = r.edges.split() if r.edges else []
        exits = [parseTime(t) for t in r.exitTimes.split()] if r.getAttributeSecure("exitTimes", None) else None
        return edges, exits
    if isinstance(elem.routeDistribution, list) and elem.routeDistribution:
        rlist = elem.routeDistribution[0].route or []
        if rlist:
            last_idx = int(elem.routeDistribution[0].last or 0)
            r = rlist[min(last_idx, len(rlist) - 1)]
            edges = r.edges.split() if r.edges else []
            exits = [parseTime(t) for t in r.exitTimes.split()] if r.getAttributeSecure("exitTimes", None) else None
            return edges, exits
    r_id = elem.route if isinstance(elem.route, str) else elem.getAttributeSecure("attr_route", None)
    if r_id and r_id in standalone_routes:
        return list(standalone_routes[r_id]), None
    return [], None


def compute_edge_capacities(net, options):
    vc = options.vclass if options.vclass else None
    caps = {}
    tt = {}
    sf = max(1e-6, options.speedFactor)
    for edge in net.getEdges():
        if edge.getFunction() == "internal":
            continue
        lanes = [ln for ln in edge.getLanes() if vc is None or ln.allows(vc)]
        n_lanes = len(lanes) if lanes else edge.getLaneNumber()
        caps[edge.getID()] = n_lanes * options.laneCap
        spd = max(0.1, edge.getSpeed() * sf)
        tt[edge.getID()] = edge.getLength() / spd
    return caps, tt


def main(options):
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile)
    edge_cap, edge_tt = compute_edge_capacities(net, options)

    standalone_routes = {}
    # iv_begin -> edgeID -> count
    req_counts = defaultdict(lambda: defaultdict(int))
    max_seen_time = options.beginSec

    for rfile in options.routeFileList:
        if options.verbose:
            print("Reading routes '%s'" % rfile)
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "route":
                    if elem.id and elem.edges:
                        standalone_routes[elem.id] = elem.edges.split()
                elif elem.name in ("vehicle", "flow"):
                    if elem.getAttributeSecure("depart", None) in ("triggered", "containerTriggered"):
                        continue
                    edges, exit_times = extract_route_and_exittimes(elem, standalone_routes)
                    if not edges:
                        continue
                    if elem.name == "flow":
                        departs = expand_flow_departures(elem)
                    else:
                        departs = [parseTime(elem.getAttributeSecure("depart", "0"))]

                    for dep_t in departs:
                        cur_t = dep_t
                        for idx, eid in enumerate(edges):
                            if eid not in edge_cap:
                                continue
                            if exit_times and idx > 0 and idx - 1 < len(exit_times):
                                entry_t = exit_times[idx - 1]
                            elif idx == 0:
                                entry_t = dep_t
                            else:
                                entry_t = cur_t

                            if entry_t >= options.beginSec and (options.endSec is None or entry_t < options.endSec):
                                bucket = int((entry_t - options.beginSec) // options.intervalSec)
                                iv_b = options.beginSec + bucket * options.intervalSec
                                req_counts[iv_b][eid] += 1
                                max_seen_time = max(max_seen_time, entry_t)

                            if exit_times and idx < len(exit_times):
                                cur_t = exit_times[idx]
                            else:
                                cur_t = entry_t + edge_tt.get(eid, 0.0)

    vc_stat = Statistics("Requested vcRatio")
    overloaded = 0

    with sumolib.openz(options.outfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "data", schemaPath="datamode_file.xsd", options=options)
        for iv_b in sorted(req_counts.keys()):
            iv_e = iv_b + options.intervalSec
            if options.endSec is not None:
                iv_e = min(iv_e, options.endSec)
            iv_dur = iv_e - iv_b
            if iv_dur <= 0:
                continue
            edge_rows = []
            for eid in sorted(req_counts[iv_b].keys()):
                cnt = req_counts[iv_b][eid]
                req_flow = cnt * 3600.0 / iv_dur
                cap = edge_cap[eid]
                vc_ratio = (req_flow / cap) if cap > 0 else 0.0
                if vc_ratio < options.minRatio:
                    continue
                excess = max(0.0, req_flow - cap)
                edge_rows.append((eid, cnt, req_flow, cap, excess, vc_ratio))
                vc_stat.add(vc_ratio, "%s@%s" % (eid, intIfPossible(iv_b)))
                if vc_ratio > 1.0:
                    overloaded += 1
            if edge_rows:
                outf.write('    <interval id="%s" begin="%s" end="%s">\n' % (
                    options.id, intIfPossible(iv_b), intIfPossible(iv_e)))
                for eid, cnt, req_flow, cap, excess, vc_ratio in edge_rows:
                    outf.write(
                        '        <edge id="%s" requestedCount="%d" requestedFlow="%.2f" '
                        'capacity="%.2f" excessFlow="%.2f" vcRatio="%.4f"/>\n' % (
                            eid, cnt, req_flow, cap, excess, vc_ratio)
                    )
                outf.write("    </interval>\n")
        outf.write("</data>\n")

    print("Overloaded edge-intervals (vcRatio > 1): %d" % overloaded)
    if vc_stat.count() > 0:
        print(vc_stat)


if __name__ == "__main__":
    main(get_options())
