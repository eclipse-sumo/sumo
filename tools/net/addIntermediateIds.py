#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2025-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    addIntermediateIds.py
# @author  Mark Colley
# @date    2026-11-17

"""
Add edge parameter 'removedNodeIds' to a simplified network (A) from an
unsimplified raw network (B) via origId, ID-substring, or geometry matching.
"""
from __future__ import absolute_import
from __future__ import print_function
import math
import os
import subprocess
import sys
from collections import defaultdict

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import sumolib  # noqa
import sumolib.geomhelper as gh  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Add edge param removedNodeIds from a raw unsimplified network")
    ap.add_argument("-n", "--net-file", dest="targetNet", required=True, category="input",
                    type=ap.net_file, help="Simplified SUMO network to receive removedNodeIds params", metavar="FILE")
    ap.add_argument("--raw-net", dest="rawNet", required=True, category="input",
                    type=ap.net_file, help="Unsimplified raw SUMO network with intermediate nodes", metavar="FILE")
    ap.add_argument("-o", "--output-file", dest="output", required=True, category="output",
                    type=ap.net_file, help="Output patched network (or .edg.xml if --patch-only)", metavar="FILE")
    ap.add_argument("-p", "--patch-file", dest="patchFile", category="output", default="patch.edg.xml",
                    type=ap.file, help="Temporary or output .edg.xml patch file", metavar="FILE")
    ap.add_argument("--patch-only", dest="patchOnly", action="store_true", default=False,
                    category="processing", help="Only write the .edg.xml patch file without running netconvert")
    ap.add_argument("--mode", choices=("auto", "origId", "id", "geometry"), default="auto",
                    category="processing", help="Matching strategy between target-net and raw-net")
    ap.add_argument("--max-distance", dest="maxDist", type=float, default=10.0,
                    category="processing", help="Maximum distance in m for geometry matching")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Tell me what you are doing")
    return ap.parse_args(args=args)


def _tangent_at_offset(shape, offset):
    acc = 0.0
    for (x1, y1), (x2, y2) in zip(shape[:-1], shape[1:]):
        seg = math.hypot(x2 - x1, y2 - y1)
        if seg > 0 and (acc + seg >= offset or (x2, y2) == shape[-1]):
            return math.degrees(math.atan2(y2 - y1, x2 - x1))
        acc += seg
    return 0.0


def _angle_diff(a, b):
    return abs((a - b + 180.0) % 360.0 - 180.0)


def _edge_aligned(edge_a, edge_b, max_angle=45.0):
    shape_a = edge_a.getShape(True)
    mid_b = gh.positionAtShapeOffset(edge_b.getShape(True), edge_b.getLength() * 0.5)
    off_a = gh.polygonOffsetWithMinimumDistanceToPoint(mid_b, shape_a, perpendicular=False)
    ang_a = _tangent_at_offset(shape_a, off_a)
    sb = edge_b.getShape(True)
    ang_b = _tangent_at_offset(sb, edge_b.getLength() * 0.5)
    return _angle_diff(ang_a, ang_b) <= max_angle, off_a


def _base_way_id(edge_id):
    return edge_id.split('#', 1)[0]


def _order_chain_nodes(edge_a, matched_edges):
    if len(matched_edges) < 2:
        return []
    shape_a = edge_a.getShape(True)
    ordered = sorted(
        matched_edges,
        key=lambda eb: gh.polygonOffsetWithMinimumDistanceToPoint(
            gh.positionAtShapeOffset(eb.getShape(True), eb.getLength() * 0.5), shape_a, perpendicular=False
        ),
    )
    end_ids = {edge_a.getFromNode().getID(), edge_a.getToNode().getID()}
    from_coord = edge_a.getFromNode().getCoord()
    to_coord = edge_a.getToNode().getCoord()
    nodes = []
    seen = set()
    for e1, e2 in zip(ordered[:-1], ordered[1:]):
        cand_nodes = [e1.getToNode()]
        if e2.getFromNode() != e1.getToNode():
            cand_nodes.append(e2.getFromNode())
        for n in cand_nodes:
            nid = n.getID()
            if nid in end_ids or nid in seen:
                continue
            if gh.distance(n.getCoord(), from_coord) < 1.0 or gh.distance(n.getCoord(), to_coord) < 1.0:
                continue
            seen.add(nid)
            nodes.append(nid)
    return nodes


def deduce_by_orig_id(edge_a, orig_index):
    orig_str = edge_a.getLanes()[0].getParam("origId")
    if not orig_str:
        return []
    candidates = []
    seen_e = set()
    for oid in orig_str.split():
        for eb in orig_index.get(oid, ()):
            if eb not in seen_e:
                aligned, _ = _edge_aligned(edge_a, eb)
                if aligned and gh.distancePointToPolygon(
                    gh.positionAtShapeOffset(eb.getShape(True), eb.getLength() * 0.5), edge_a.getShape(True)
                ) <= 25.0:
                    seen_e.add(eb)
                    candidates.append(eb)
    return _order_chain_nodes(edge_a, candidates)


def deduce_by_id_substring(edge_a, base_index):
    aid = edge_a.getID()
    base = _base_way_id(aid)
    candidates = []
    for eb in base_index.get(base, ()):
        bid = eb.getID()
        if bid == aid or bid.startswith(aid + "#") or _base_way_id(bid) == base:
            aligned, _ = _edge_aligned(edge_a, eb)
            if aligned:
                candidates.append(eb)
    return _order_chain_nodes(edge_a, candidates)


def deduce_by_geometry(edge_a, raw_nodes, max_dist):
    shape_a = edge_a.getShape(True)
    len_a = edge_a.getLength()
    end_ids = {edge_a.getFromNode().getID(), edge_a.getToNode().getID()}
    from_coord = edge_a.getFromNode().getCoord()
    to_coord = edge_a.getToNode().getCoord()
    matched = []
    for nb in raw_nodes:
        nid = nb.getID()
        if nid in end_ids:
            continue
        coord = nb.getCoord()
        if gh.distance(coord, from_coord) < 1.0 or gh.distance(coord, to_coord) < 1.0:
            continue
        dist = gh.distancePointToPolygon(coord, shape_a)
        if dist > max_dist:
            continue
        offset = gh.polygonOffsetWithMinimumDistanceToPoint(coord, shape_a, perpendicular=False)
        if offset <= 0.5 or offset >= len_a - 0.5:
            continue
        ang_a = _tangent_at_offset(shape_a, offset)
        has_aligned = False
        for eb in list(nb.getIncoming()) + list(nb.getOutgoing()):
            sb = eb.getShape(True)
            ang_b = _tangent_at_offset(sb, eb.getLength() * 0.5)
            if _angle_diff(ang_a, ang_b) <= 35.0:
                has_aligned = True
                break
        if has_aligned:
            matched.append((offset, nid))
    matched.sort()
    return [nid for _, nid in matched]


def main(options):
    if options.verbose:
        print("Reading target-net '%s'" % options.targetNet)
    net_a = sumolib.net.readNet(options.targetNet)
    if options.verbose:
        print("Reading raw-net '%s'" % options.rawNet)
    net_b = sumolib.net.readNet(options.rawNet)

    orig_index = defaultdict(list)
    base_index = defaultdict(list)
    for eb in net_b.getEdges(False):
        base_index[_base_way_id(eb.getID())].append(eb)
        oid_str = eb.getLanes()[0].getParam("origId")
        if oid_str:
            for oid in oid_str.split():
                orig_index[oid].append(eb)
        orig_index[eb.getID()].append(eb)

    raw_nodes = list(net_b.getNodes())
    patched = {}

    for ea in sorted(net_a.getEdges(False), key=lambda e: e.getID()):
        existing = ea.getParam("removedNodeIds")
        existing_list = existing.split() if existing else []
        deduced = []
        if options.mode in ("auto", "origId"):
            deduced = deduce_by_orig_id(ea, orig_index)
        if not deduced and options.mode in ("auto", "id"):
            deduced = deduce_by_id_substring(ea, base_index)
        if not deduced and options.mode in ("auto", "geometry"):
            deduced = deduce_by_geometry(ea, raw_nodes, options.maxDist)

        merged = list(existing_list)
        for nid in deduced:
            if nid not in merged:
                merged.append(nid)
        if merged and merged != existing_list:
            patched[ea.getID()] = " ".join(merged)

    if options.verbose:
        print("Added removedNodeIds to %s edges" % len(patched))

    patch_target = options.output if options.patchOnly else options.patchFile
    with sumolib.openz(patch_target, 'w') as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for edge_id in sorted(patched.keys()):
            fout.write('    <edge id="%s">\n' % edge_id)
            fout.write('        <param key="removedNodeIds" value="%s"/>\n' % patched[edge_id])
            fout.write('    </edge>\n')
        fout.write('</edges>\n')

    if not options.patchOnly:
        netconvert = sumolib.checkBinary('netconvert')
        subprocess.check_call([netconvert, '-s', options.targetNet, '-e', patch_target, '-o', options.output])


if __name__ == "__main__":
    main(get_options())
