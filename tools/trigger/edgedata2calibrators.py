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

# @file    edgedata2calibrators.py
# @author  Mark Colley
# @date    2026-04-12

"""
Generate <calibrator> flow definitions (plus routeDistributions / fallback routes
and optional <routeProbe> elements) from an edgeData XML file.
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


def get_options(args=None):
    op = sumolib.options.ArgumentParser(
        description="Generate calibrator definitions from edgeData")
    op.add_argument("-n", "--net-file", dest="netfile", category="input", type=op.net_file,
                    required=True, help="SUMO network file")
    op.add_argument("-d", "--edgedata-file", dest="edgedata", category="input",
                    type=op.edgedata_file, required=True, help="input edgeData XML file")
    op.add_argument("-o", "--output-file", dest="outfile", category="output",
                    type=op.additional_file, required=True,
                    help="output additional file with calibrators")
    op.add_argument("-r", "--route-files", dest="routefiles", category="input",
                    type=op.route_file_list,
                    help="optional route file(s) to build routeDistributions per calibrator")
    op.add_argument("--count-attribute", dest="countAttr", default="entered",
                    category="processing",
                    help="edgeData attribute for flow counts (fallback: 'count')")
    op.add_argument("--speed-attribute", dest="speedAttr", default="speed",
                    category="processing",
                    help="edgeData attribute for target speed (empty string to omit speed)")
    op.add_argument("--pos", type=float, default=0.0, category="processing",
                    help="position of calibrator on edge")
    op.add_argument("--route-probe", dest="routeProbe", action="store_true", default=False,
                    category="processing",
                    help="write a routeProbe element per calibrated edge and link it")
    op.add_argument("--probe-output", dest="probeFile", default="routeProbe.out.xml",
                    category="output",
                    help="file attribute for generated routeProbe elements")
    op.add_argument("--probe-period", dest="probePeriod", type=op.time,
                    category="processing",
                    help="period attribute for generated routeProbe elements")
    op.add_argument("--calibrator-output", dest="calOutput", type=str,
                    category="output",
                    help="optional output attribute on generated calibrators")
    op.add_argument("--vtypes", dest="vtypes", type=str, category="processing",
                    help="optional vTypes attribute for calibrators and routeProbes")
    op.add_argument("--edges", type=str, category="processing",
                    help="optional comma-separated subset of edge IDs to calibrate")
    op.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = op.parse_args(args=args)
    options.routeFileList = [f.strip() for f in (options.routefiles or "").split(",") if f.strip()]
    options.edgeSubset = set(e.strip() for e in options.edges.split(",") if e.strip()) if options.edges else None
    return options


def flow_weight(elem):
    number = elem.getAttributeSecure("number", None)
    if number is not None:
        return max(1.0, float(number))
    b = parseTime(elem.getAttributeSecure("begin", "0"))
    e = parseTime(elem.getAttributeSecure("end", str(b + 3600.0)))
    dur = max(0.0, e - b)
    vph = elem.getAttributeSecure("vehsPerHour", None)
    if vph is not None and dur > 0:
        return max(1.0, float(vph) * dur / 3600.0)
    period = elem.getAttributeSecure("period", None)
    if period is not None and parseTime(period) > 0 and dur > 0:
        return max(1.0, dur / parseTime(period))
    return 1.0


def load_edge_route_suffixes(route_files, target_edges):
    standalone = {}
    referenced_standalone = set()
    suffix_counts = defaultdict(lambda: defaultdict(float))

    def record_route(edges, weight):
        seen_in_route = set()
        for idx, eid in enumerate(edges):
            if eid in target_edges and eid not in seen_in_route:
                seen_in_route.add(eid)
                suffix = tuple(edges[idx:])
                suffix_counts[eid][suffix] += weight

    for rfile in route_files:
        for root in sumolib.xml.parse(rfile, ["routes", "alternatives"]):
            for elem in root.getChildList():
                if elem.name == "route":
                    if elem.id and elem.edges:
                        prob = float(elem.getAttributeSecure("probability", "1"))
                        standalone[elem.id] = (elem.edges.split(), prob)
                elif elem.name == "routeDistribution":
                    for r in (elem.route or []):
                        if r.edges:
                            prob = float(r.getAttributeSecure("probability", "1"))
                            record_route(r.edges.split(), prob)
                elif elem.name in ("vehicle", "flow"):
                    w = flow_weight(elem) if elem.name == "flow" else 1.0
                    edges = None
                    if isinstance(elem.route, list) and elem.route and elem.route[0].edges:
                        edges = elem.route[0].edges.split()
                    elif isinstance(elem.routeDistribution, list) and elem.routeDistribution:
                        rlist = elem.routeDistribution[0].route or []
                        if rlist:
                            last_idx = int(elem.routeDistribution[0].last or 0)
                            chosen = rlist[min(last_idx, len(rlist) - 1)]
                            if chosen.edges:
                                edges = chosen.edges.split()
                    else:
                        r_id = (
                            elem.route if isinstance(elem.route, str)
                            else elem.getAttributeSecure("attr_route", None)
                        )
                        if r_id and r_id in standalone:
                            referenced_standalone.add(r_id)
                            edges = standalone[r_id][0]
                    if edges:
                        record_route(edges, w)

    for r_id, (edges, prob) in standalone.items():
        if r_id not in referenced_standalone:
            record_route(edges, prob)

    return suffix_counts


def parse_edgedata(edgedata_file, valid_edges, options):
    edge_intervals = defaultdict(list)
    for iv in sumolib.xml.parse(edgedata_file, "interval"):
        begin = parseTime(iv.begin)
        end = parseTime(iv.end)
        dur = end - begin
        if dur <= 0:
            continue
        for edge in (iv.edge or []):
            eid = edge.id
            if eid not in valid_edges:
                continue
            if options.edgeSubset is not None and eid not in options.edgeSubset:
                continue
            raw_cnt = edge.getAttributeSecure(options.countAttr, None)
            if raw_cnt is None and options.countAttr != "count":
                raw_cnt = edge.getAttributeSecure("count", None)
            if raw_cnt is None:
                continue
            cnt = float(raw_cnt)
            vph = cnt * 3600.0 / dur
            spd = None
            if options.speedAttr:
                raw_spd = edge.getAttributeSecure(options.speedAttr, None)
                if raw_spd is not None and float(raw_spd) >= 0:
                    spd = float(raw_spd)
            edge_intervals[eid].append((begin, end, vph, spd))
    return edge_intervals


def main(options):
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile)
    valid_edges = {e.getID() for e in net.getEdges() if e.getFunction() != "internal"}

    edge_intervals = parse_edgedata(options.edgedata, valid_edges, options)
    calibrated_edges = sorted(edge_intervals.keys())

    suffix_counts = {}
    if options.routeFileList and calibrated_edges:
        suffix_counts = load_edge_route_suffixes(options.routeFileList, set(calibrated_edges))

    with sumolib.openz(options.outfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)

        edge_route_ref = {}
        for eid in calibrated_edges:
            routes_for_edge = sorted(suffix_counts.get(eid, {}).items())
            if len(routes_for_edge) > 1:
                dist_id = "cal_%s_dist" % eid
                outf.write('    <routeDistribution id="%s">\n' % dist_id)
                for idx, (edges_tup, prob) in enumerate(routes_for_edge):
                    p_str = str(intIfPossible(round(prob, 4)))
                    outf.write('        <route id="cal_%s_r%d" edges="%s" probability="%s"/>\n' % (
                        eid, idx, " ".join(edges_tup), p_str))
                outf.write("    </routeDistribution>\n")
                edge_route_ref[eid] = dist_id
            elif len(routes_for_edge) == 1:
                r_id = "cal_%s_r0" % eid
                outf.write('    <route id="%s" edges="%s"/>\n' % (r_id, " ".join(routes_for_edge[0][0])))
                edge_route_ref[eid] = r_id
            else:
                r_id = "cal_%s_fallback" % eid
                outf.write('    <route id="%s" edges="%s"/>\n' % (r_id, eid))
                edge_route_ref[eid] = r_id

        if options.routeProbe:
            for eid in calibrated_edges:
                rp_attrs = ['id="rp_%s"' % eid, 'edge="%s"' % eid]
                if options.probePeriod is not None:
                    rp_attrs.append('period="%s"' % intIfPossible(parseTime(options.probePeriod)))
                if options.vtypes:
                    rp_attrs.append('vTypes="%s"' % options.vtypes)
                rp_attrs.append('file="%s"' % options.probeFile)
                outf.write("    <routeProbe %s/>\n" % " ".join(rp_attrs))

        for eid in calibrated_edges:
            cal_attrs = [
                'id="cal_%s"' % eid,
                'edge="%s"' % eid,
                'pos="%.2f"' % options.pos,
            ]
            if options.routeProbe:
                cal_attrs.append('routeProbe="rp_%s"' % eid)
            if options.calOutput:
                cal_attrs.append('output="%s"' % options.calOutput)
            if options.vtypes:
                cal_attrs.append('vTypes="%s"' % options.vtypes)
            outf.write("    <calibrator %s>\n" % " ".join(cal_attrs))
            for begin, end, vph, spd in sorted(edge_intervals[eid]):
                f_attrs = [
                    'begin="%s"' % intIfPossible(begin),
                    'end="%s"' % intIfPossible(end),
                    'route="%s"' % edge_route_ref[eid],
                    'vehsPerHour="%.2f"' % vph,
                ]
                if spd is not None:
                    f_attrs.append('speed="%.2f"' % spd)
                outf.write("        <flow %s/>\n" % " ".join(f_attrs))
            outf.write("    </calibrator>\n")

        outf.write("</additional>\n")

    if options.verbose:
        print("Wrote %d calibrators to '%s'" % (len(calibrated_edges), options.outfile))


if __name__ == "__main__":
    main(get_options())
