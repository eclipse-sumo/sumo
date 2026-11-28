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

# @file    junctionDistricts.py
# @author  Mark Colley
# @date    2026-04-12

"""
Generate TAZ definitions aggregating incoming edges (and optionally internal edges)
around network junctions within a configurable radius.
"""
from __future__ import absolute_import
from __future__ import print_function

import heapq
import os
import random
import sys

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.miscutils import Colorgen  # noqa
from sumolib.net.connection import Connection  # noqa
from sumolib.options import ArgumentParser  # noqa


def get_options(args=None):
    ap = ArgumentParser(description="Generate TAZ definitions around network junctions")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    ap.add_argument("-n", "--net-file", dest="netfile", category="input", type=ap.net_file,
                    required=True, help="the SUMO network file to read")
    ap.add_argument("-o", "--output", category="output", type=ap.additional_file,
                    required=True, help="output taz file")
    ap.add_argument("-r", "--radius", type=float, default=100.0, category="processing",
                    help="maximum upstream distance in m around each junction")
    ap.add_argument("-i", "--internal", action="store_true", default=False, category="processing",
                    help="include internal edges of the junction (and traversed upstream junctions)")
    ap.add_argument("--tls-only", dest="tlsOnly", action="store_true", default=False,
                    category="processing", help="only generate TAZ for traffic light junctions")
    ap.add_argument("-j", "--junctions", type=str, category="processing",
                    help="comma-separated list of junction IDs to include")
    ap.add_argument("--vclass", type=str, category="processing",
                    help="only include edges allowing VCLASS")
    ap.add_argument("--stop-on-tls", dest="stopOnTLS", action="store_true", default=True,
                    category="processing", help="stop upstream search at preceding traffic light junctions")
    ap.add_argument("--no-stop-on-tls", dest="stopOnTLS", action="store_false",
                    category="processing", help="continue upstream search past preceding traffic lights")
    ap.add_argument("--follow-turnaround", dest="followTurnaround", action="store_true", default=False,
                    category="processing", help="extend upstream search across turnaround connections")
    ap.add_argument("-u", "--hue", default="random", type=str, category="processing",
                    help="hue for taz (float from [0,1] or 'random')")
    ap.add_argument("-s", "--saturation", default=1, type=str, category="processing",
                    help="saturation for taz (float from [0,1] or 'random')")
    ap.add_argument("-b", "--brightness", default=1, type=str, category="processing",
                    help="brightness for taz (float from [0,1] or 'random')")
    ap.add_argument("--seed", type=int, default=42, category="processing", help="random seed")
    options = ap.parse_args(args=args)
    options.colorgen = Colorgen((options.hue, options.saturation, options.brightness))
    options.junctionSet = set(options.junctions.split(",")) if options.junctions else None
    return options


def collect_internal_chain(net, viaLaneID, vclass, result):
    if not viaLaneID:
        return
    edgeID = viaLaneID.rsplit("_", 1)[0]
    if not net.hasEdge(edgeID):
        return
    stack = [net.getEdge(edgeID)]
    while stack:
        cur = stack.pop()
        if cur.getFunction() != "internal" or cur.getID() in result:
            continue
        if vclass is not None and not cur.allows(vclass):
            continue
        result.add(cur.getID())
        for nxt in cur.getOutgoing():
            if nxt.getFunction() == "internal" and nxt.getID() not in result:
                stack.append(nxt)


def collect_junction_edges(net, node, options):
    collected = set()
    best_dist = {}
    heap = []

    for inc in node.getIncoming():
        if inc.getFunction() == "internal":
            continue
        if options.vclass is not None and not inc.allows(options.vclass):
            continue
        best_dist[inc] = 0.0
        heapq.heappush(heap, (0.0, inc.getID(), inc))
        if options.internal:
            for conns in inc.getOutgoing().values():
                for conn in conns:
                    if options.vclass is None or (
                        conn.getFromLane().allows(options.vclass)
                        and conn.getToLane().allows(options.vclass)
                    ):
                        collect_internal_chain(net, conn.getViaLaneID(), options.vclass, collected)

    while heap:
        dist, _, edge = heapq.heappop(heap)
        if dist > best_dist.get(edge, 1e100):
            continue
        collected.add(edge.getID())
        next_dist = dist + edge.getLength()
        if next_dist >= options.radius:
            continue
        for prev_edge, conns in edge.getIncoming().items():
            if prev_edge.getFunction() == "internal":
                continue
            if options.vclass is not None and not prev_edge.allows(options.vclass):
                continue
            valid_conns = [
                c for c in conns
                if (options.followTurnaround or c.getDirection() != Connection.LINKDIR_TURN)
                and (options.vclass is None or (
                    c.getFromLane().allows(options.vclass)
                    and c.getToLane().allows(options.vclass)
                ))
            ]
            if not valid_conns:
                continue
            if options.stopOnTLS and prev_edge.getTLS() is not None:
                continue
            if next_dist < best_dist.get(prev_edge, 1e100):
                best_dist[prev_edge] = next_dist
                heapq.heappush(heap, (next_dist, prev_edge.getID(), prev_edge))
            if options.internal:
                for c in valid_conns:
                    collect_internal_chain(net, c.getViaLaneID(), options.vclass, collected)

    return sorted(collected)


def is_tls_junction(node):
    if node.getType().startswith("traffic_light"):
        return True
    for inc in node.getIncoming():
        if inc.getTLS() is not None:
            return True
    return False


def main(options):
    random.seed(options.seed)
    if options.verbose:
        print("Reading net '%s'" % options.netfile)
    net = sumolib.net.readNet(options.netfile, withInternal=options.internal)

    nodes = sorted(
        [n for n in net.getNodes() if n.getType() != "internal"],
        key=lambda n: n.getID(),
    )

    written = 0
    with open(options.output, "w", encoding="utf-8") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for node in nodes:
            nodeID = node.getID()
            if options.junctionSet is not None and nodeID not in options.junctionSet:
                continue
            if options.tlsOnly and not is_tls_junction(node):
                continue
            edges = collect_junction_edges(net, node, options)
            if not edges:
                continue
            cx, cy = node.getCoord()[:2]
            color = options.colorgen()
            outf.write('    <taz id="%s" center="%.2f,%.2f" color="%s" edges="%s"/>\n' % (
                nodeID, cx, cy, color, " ".join(edges)))
            written += 1
        outf.write("</additional>\n")

    if options.verbose:
        print("Wrote %d junction TAZ definitions to '%s'" % (written, options.output))


if __name__ == "__main__":
    main(get_options())
