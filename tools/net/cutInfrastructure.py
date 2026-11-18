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

# @file    cutInfrastructure.py
# @author  Mark Colley
# @date    2026-04-12

"""
Filter SUMO additional/infrastructure files (busStops, trainStops, containerStops,
chargingStations, parkingAreas, E1/E2/E3 detectors, calibrators, routeProbes,
rerouters, variableSpeedSigns, vaporizers, TAZs) to a cut subnetwork.
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

SINGLE_LANE_TAGS = {
    "busStop", "trainStop", "containerStop", "chargingStation", "parkingArea",
    "e1Detector", "inductionLoop", "instantInductionLoop", "overheadWireSegment",
}
AREA_DET_TAGS = {"e2Detector", "laneAreaDetector"}
E3_TAGS = {"e3Detector", "entryExitDetector"}


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Filter stops, detectors, and triggers in additional files to a cut subnetwork")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input",
                    type=ap.net_file, required=True,
                    help="target cut SUMO network file")
    ap.add_argument("-a", "--additional-files", dest="addfiles", category="input",
                    type=ap.additional_file_list, required=True,
                    help="comma-separated input additional/infrastructure files")
    ap.add_argument("-o", "--output-file", dest="outfile", category="output",
                    type=ap.additional_file, required=True,
                    help="output filtered additional file")
    ap.add_argument("--clamp-positions", dest="clampPos", action="store_true",
                    default=True, category="processing",
                    help="clamp startPos/endPos/pos to cut lane lengths (default)")
    ap.add_argument("--no-clamp-positions", dest="clampPos", action="store_false",
                    category="processing",
                    help="drop elements whose positions exceed cut lane lengths instead of clamping")
    ap.add_argument("--keep-partial-e3", dest="keepPartialE3", action="store_true",
                    default=False, category="processing",
                    help="keep e3Detector if at least one detEntry and one detExit remain")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args(args=args)
    options.addFileList = [f.strip() for f in options.addfiles.split(",") if f.strip()]
    return options


def get_attr_str(elem, attr_name):
    val = getattr(elem, attr_name, None)
    if isinstance(val, str):
        return val
    return elem.getAttributeSecure("attr_" + attr_name, None)


def replace_children(elem, child_tag, new_children):
    if child_tag not in elem._child_dict:
        return
    if new_children:
        setattr(elem, child_tag, new_children)
    else:
        delattr(elem, child_tag)


def check_or_clamp_pos(elem, attr_name, lane_len, clamp):
    raw = get_attr_str(elem, attr_name)
    if raw is None:
        return True
    pos = float(raw)
    eff = (lane_len + pos) if pos < 0 else pos
    if 0.0 <= eff <= lane_len:
        return True
    if not clamp:
        return False
    clamped = max(0.0, min(lane_len, eff))
    elem.setAttribute(attr_name, "%.2f" % clamped)
    return True


def filter_single_lane(elem, valid_lanes, options):
    lane_id = get_attr_str(elem, "lane")
    if not lane_id or lane_id not in valid_lanes:
        return False
    lane_len = valid_lanes[lane_id].getLength()
    for p_attr in ("pos", "startPos", "endPos"):
        if not check_or_clamp_pos(elem, p_attr, lane_len, options.clampPos):
            return False
    if elem.hasAttribute("startPos") and elem.hasAttribute("endPos"):
        s_raw = float(get_attr_str(elem, "startPos"))
        e_raw = float(get_attr_str(elem, "endPos"))
        s_eff = (lane_len + s_raw) if s_raw < 0 else s_raw
        e_eff = (lane_len + e_raw) if e_raw < 0 else e_raw
        if e_eff <= s_eff:
            if not options.clampPos or lane_len <= 0.1:
                return False
            s_new = max(0.0, min(s_eff, lane_len - 0.1))
            elem.setAttribute("startPos", "%.2f" % s_new)
            elem.setAttribute("endPos", "%.2f" % lane_len)

    if isinstance(elem.access, list):
        kept_acc = []
        for acc in elem.access:
            acc_lane = get_attr_str(acc, "lane")
            if acc_lane and acc_lane in valid_lanes:
                if check_or_clamp_pos(acc, "pos", valid_lanes[acc_lane].getLength(), options.clampPos):
                    kept_acc.append(acc)
        replace_children(elem, "access", kept_acc)

    if isinstance(elem.space, list):
        pass
    return True


def filter_area_detector(elem, valid_lanes, options):
    lanes_str = get_attr_str(elem, "lanes")
    if lanes_str:
        lanes = lanes_str.split()
        if not lanes or not all(ln in valid_lanes for ln in lanes):
            return False
        first_len = valid_lanes[lanes[0]].getLength()
        return check_or_clamp_pos(elem, "pos", first_len, options.clampPos)

    lane_id = get_attr_str(elem, "lane")
    if not lane_id or lane_id not in valid_lanes:
        return False
    lane_len = valid_lanes[lane_id].getLength()
    if not check_or_clamp_pos(elem, "pos", lane_len, options.clampPos):
        return False
    if not check_or_clamp_pos(elem, "endPos", lane_len, options.clampPos):
        return False
    raw_len = get_attr_str(elem, "length")
    raw_pos = get_attr_str(elem, "pos")
    if raw_len is not None and raw_pos is not None:
        p = float(get_attr_str(elem, "pos"))
        p_eff = (lane_len + p) if p < 0 else p
        det_l = float(raw_len)
        if p_eff + det_l > lane_len:
            if not options.clampPos:
                return False
            new_l = max(0.1, lane_len - p_eff)
            elem.setAttribute("length", "%.2f" % new_l)
    return True


def filter_e3_detector(elem, valid_lanes, options):
    orig_entries = list(elem.detEntry) if isinstance(elem.detEntry, list) else []
    orig_exits = list(elem.detExit) if isinstance(elem.detExit, list) else []
    kept_entries = []
    for de in orig_entries:
        ln = get_attr_str(de, "lane")
        if ln and ln in valid_lanes:
            if check_or_clamp_pos(de, "pos", valid_lanes[ln].getLength(), options.clampPos):
                kept_entries.append(de)
    kept_exits = []
    for dx in orig_exits:
        ln = get_attr_str(dx, "lane")
        if ln and ln in valid_lanes:
            if check_or_clamp_pos(dx, "pos", valid_lanes[ln].getLength(), options.clampPos):
                kept_exits.append(dx)

    if options.keepPartialE3:
        if not kept_entries or not kept_exits:
            return False
    else:
        if len(kept_entries) != len(orig_entries) or len(kept_exits) != len(orig_exits):
            return False
        if not kept_entries or not kept_exits:
            return False
    replace_children(elem, "detEntry", kept_entries)
    replace_children(elem, "detExit", kept_exits)
    return True


def filter_rerouter(elem, valid_edges, valid_lanes):
    edges_str = get_attr_str(elem, "edges")
    if not edges_str:
        return False
    kept_edges = [e for e in edges_str.split() if e in valid_edges]
    if not kept_edges:
        return False
    elem.setAttribute("edges", " ".join(kept_edges))

    if isinstance(elem.interval, list):
        kept_ivs = []
        for iv in elem.interval:
            if isinstance(iv.closingReroute, list):
                replace_children(iv, "closingReroute",
                                 [c for c in iv.closingReroute if get_attr_str(c, "id") in valid_edges])
            if isinstance(iv.closingLaneReroute, list):
                replace_children(iv, "closingLaneReroute",
                                 [c for c in iv.closingLaneReroute if get_attr_str(c, "id") in valid_lanes])
            if isinstance(iv.destProbReroute, list):
                replace_children(iv, "destProbReroute",
                                 [c for c in iv.destProbReroute if get_attr_str(c, "id") in valid_edges])
            if iv._child_list:
                kept_ivs.append(iv)
        if not kept_ivs:
            return False
        replace_children(elem, "interval", kept_ivs)
    return True


def filter_taz(elem, valid_edges):
    changed = False
    edges_str = get_attr_str(elem, "edges")
    kept_edges = []
    if edges_str is not None:
        kept_edges = [e for e in edges_str.split() if e in valid_edges]
        elem.setAttribute("edges", " ".join(kept_edges))
        changed = True
    kept_src = []
    if isinstance(elem.tazSource, list):
        kept_src = [s for s in elem.tazSource if get_attr_str(s, "id") in valid_edges]
        replace_children(elem, "tazSource", kept_src)
        changed = True
    kept_snk = []
    if isinstance(elem.tazSink, list):
        kept_snk = [s for s in elem.tazSink if get_attr_str(s, "id") in valid_edges]
        replace_children(elem, "tazSink", kept_snk)
        changed = True
    if changed and not kept_edges and not kept_src and not kept_snk:
        return False
    return True


def filter_element(elem, valid_edges, valid_lanes, valid_nodes, options):
    tag = elem.name
    if tag in SINGLE_LANE_TAGS:
        return filter_single_lane(elem, valid_lanes, options)
    if tag in AREA_DET_TAGS:
        return filter_area_detector(elem, valid_lanes, options)
    if tag in E3_TAGS:
        return filter_e3_detector(elem, valid_lanes, options)
    if tag == "calibrator":
        lane_id = get_attr_str(elem, "lane")
        if lane_id:
            return filter_single_lane(elem, valid_lanes, options)
        edge_id = get_attr_str(elem, "edge")
        if edge_id:
            if edge_id not in valid_edges:
                return False
            first_lane = valid_edges[edge_id].getLanes()[0]
            return check_or_clamp_pos(elem, "pos", first_lane.getLength(), options.clampPos)
        node_id = get_attr_str(elem, "node")
        return bool(node_id and node_id in valid_nodes)
    if tag == "routeProbe":
        edge_id = get_attr_str(elem, "edge")
        return bool(edge_id and edge_id in valid_edges)
    if tag == "vaporizer":
        edge_id = get_attr_str(elem, "id")
        return bool(edge_id and edge_id in valid_edges)
    if tag == "variableSpeedSign":
        lanes_str = get_attr_str(elem, "lanes")
        if not lanes_str:
            return False
        kept = [ln for ln in lanes_str.split() if ln in valid_lanes]
        if not kept:
            return False
        elem.setAttribute("lanes", " ".join(kept))
        return True
    if tag == "rerouter":
        return filter_rerouter(elem, valid_edges, valid_lanes)
    if tag == "taz":
        return filter_taz(elem, valid_edges)
    return True


def main(options):
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile, withInternal=True)
    valid_edges = {e.getID(): e for e in net.getEdges()}
    valid_lanes = {ln.getID(): ln for e in net.getEdges() for ln in e.getLanes()}
    valid_nodes = {n.getID(): n for n in net.getNodes()}

    totals = defaultdict(int)
    kept_counts = defaultdict(int)
    kept_elements = []

    for afile in options.addFileList:
        if options.verbose:
            print("Filtering '%s'" % afile)
        for root in sumolib.xml.parse(afile, ["additional", "tazs"]):
            for elem in root.getChildList():
                if elem.isComment():
                    continue
                totals[elem.name] += 1
                if filter_element(elem, valid_edges, valid_lanes, valid_nodes, options):
                    kept_counts[elem.name] += 1
                    kept_elements.append(elem)

    with sumolib.openz(options.outfile, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for elem in kept_elements:
            outf.write(elem.toXML(initialIndent="    "))
        outf.write("</additional>\n")

    for tag in sorted(totals.keys()):
        print("Kept %d of %d %ss" % (kept_counts[tag], totals[tag], tag))


if __name__ == "__main__":
    main(get_options())
