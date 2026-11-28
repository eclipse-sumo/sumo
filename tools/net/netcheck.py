#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    netcheck.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @author  Laura Bieker
# @author  Jakob Erdmann
# @author  Greg Albiston
# @date    2007-03-20

"""
This script performs checks for the network.
It needs one parameter, the SUMO net (.net.xml).

- if either option --source or --destination is given, it checks reachability
- if option --right-of-way is set, it checks for problems with right of way rules
- if option --short-tls-edges is set, a selection file for short edges (< 15m)
  incoming to a traffic light is written (see #16014)
- by default it tests whether the network is (weakly) connected.
"""
from __future__ import absolute_import
from __future__ import print_function
import math
import os
import sys
THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))
from sumolib.options import ArgumentParser  # noqa
import sumolib.net  # noqa


def parse_args():
    op = ArgumentParser()
    op.add_argument("net", category="input", type=op.net_file,
                    help="The network file to be checked")
    op.add_argument("-s", "--source", category="input", default=False, type=op.edge_list,
                    help="List edges reachable from the source")
    op.add_argument("-d", "--destination", category="input", type=op.edge, default=False,
                    help="List edges which can reach the destination")
    op.add_argument("-w", "--right-of-way", action="store_true", default=False,
                    dest="checkRightOfWay",
                    help="Check for problems with right-of-way rules")
    op.add_argument("--short-tls-edges", action="store_true", default=False,
                    dest="shortTlsEdges",
                    help="Check for problems with right-of-way rules")
    op.add_argument("--capacity-reductions", action="store_true", default=False,
                    dest="capacityReductions",
                    help="Check for multi-lane corridors losing connecting lanes across junctions")
    op.add_argument("--max-intermediate-length", type=float, default=50.0,
                    dest="maxIntermediateLength",
                    help="Maximum total length of intermediate edges when checking capacity reductions")
    op.add_argument("--max-turn-angle", type=float, default=45.0,
                    dest="maxTurnAngle",
                    help="Maximum turn angle in degrees to treat an outgoing edge as a continuation")
    op.add_argument("-o", "--selection-output", category="output", type=op.file,
                    help="Write output to file(s) as a loadable selection")
    op.add_argument("--ignore-connections", action="store_true", default=False,
                    help="Assume full connectivity at each node when computing all connected components")
    op.add_argument("-l", "--vclass", help="Include only edges allowing vClass")
    op.add_argument("--component-output", type=op.file, default=None,
                    help=("Write components of disconnected network to file - not compatible " +
                          "with --source or --destination options"))
    op.add_argument("-r", "--results-output", type=op.file, default=None,
                    help=("Write results summary of disconnected network to file - not compatible " +
                          "with --source or --destination options"))
    op.add_argument("-t", "--print-types", action="store_true", default=False,
                    help="Print edge types used in the component")

    options = op.parse_args()
    return options


def getWeaklyConnected(net, vclass=None, ignore_connections=False):
    components = []
    edgesLeft = set(net.getEdges())
    queue = list()
    while len(edgesLeft) != 0:
        component = set()
        queue.append(edgesLeft.pop())
        while not len(queue) == 0:
            edge = queue.pop(0)
            if vclass is None or edge.allows(vclass):
                component.add(edge.getID())
                if ignore_connections:
                    for n in (edge.getFromNode().getOutgoing() +
                              edge.getFromNode().getIncoming() +
                              edge.getToNode().getOutgoing() +
                              edge.getToNode().getIncoming()):
                        if n in edgesLeft:
                            queue.append(n)
                            edgesLeft.remove(n)
                else:
                    for n in edge.getOutgoing():
                        if n in edgesLeft:
                            queue.append(n)
                            edgesLeft.remove(n)
                    for n in edge.getIncoming():
                        if n in edgesLeft:
                            queue.append(n)
                            edgesLeft.remove(n)
        if component:
            components.append(sorted(component))
    return components


def getReachable(net, source_id, options, useIncoming=False):
    if not net.hasEdge(source_id):
        sys.exit("'{}' is not a valid edge id".format(source_id))
    source = net.getEdge(source_id)
    try:
        found = net.getReachable(source, options.vclass, useIncoming)
        if useIncoming:
            print("{} of {} edges can reach edge '{}':".format(
                len(found), len(net.getEdges()), source_id))
        else:
            print("{} of {} edges are reachable from edge '{}':".format(
                len(found), len(net.getEdges()), source_id))

        ids = sorted([e.getID() for e in found])
        if options.selection_output:
            with open(options.selection_output, 'w') as f:
                for e in ids:
                    f.write("edge:{}\n".format(e))
        else:
            print(ids)

    except RuntimeError as e:
        sys.exit(e)


def checkRightOfWay(net, options):
    lanes = []
    for edge in net.getEdges(False):
        for lane in edge.getLanes():
            if lane.isAccelerationLane():
                for c in lane.getIncomingConnections():
                    if c.getFromLane().isNormal() and c.getState() != "M":
                        lanes.append(lane)

    if options.selection_output:
        with open(options.selection_output, 'w') as f:
            for lane in lanes:
                f.write("lane:%s\n" % lane.getID())
    else:
        if lanes:
            print("Found %s acceleration lanes with invalid right-of-way on the incoming connection" % len(lanes))
        print('\n'.join([lane.getID() for lane in lanes]))


def checkShortTLSEdges(net, options):
    SHORT_EDGE = 15
    short = []
    for edge in net.getEdges(False):
        if edge.getLength() < SHORT_EDGE and edge.getToNode().getType() == "traffic_light":
            short.append(edge)

    if options.selection_output:
        with open(options.selection_output, 'w') as f:
            for e in short:
                f.write("edge:%s\n" % e.getID())
    else:
        print('\n'.join([e.getID() for e in short]))
    if short:
        print("Found %s edges with length below %sm ahead of traffic lights" % (
            len(short), SHORT_EDGE))


def _edgeTangent(edge, atEnd):
    shape = edge.getShape()
    if atEnd:
        for i in range(len(shape) - 1, 0, -1):
            dx = shape[i][0] - shape[i - 1][0]
            dy = shape[i][1] - shape[i - 1][1]
            if dx != 0 or dy != 0:
                return math.degrees(math.atan2(dy, dx))
    else:
        for i in range(len(shape) - 1):
            dx = shape[i + 1][0] - shape[i][0]
            dy = shape[i + 1][1] - shape[i][1]
            if dx != 0 or dy != 0:
                return math.degrees(math.atan2(dy, dx))
    return 0.0


def _maxDisjointLanes(chain, vclass):
    # Unit vertex-capacity max-flow through allowed lanes along chain[0]..chain[-1]
    adj = {}
    source = ("SRC", 0)
    sink = ("SNK", 0)
    adj[source] = []
    adj[sink] = []

    def add_cap_edge(u, v):
        adj.setdefault(u, [])
        adj.setdefault(v, [])
        adj[u].append([v, 1, len(adj[v])])
        adj[v].append([u, 0, len(adj[u]) - 1])

    for step_idx, edge in enumerate(chain):
        for lane in edge.getLanes():
            if not vclass or lane.allows(vclass):
                u_in = ("IN", step_idx, lane.getIndex())
                u_out = ("OUT", step_idx, lane.getIndex())
                add_cap_edge(u_in, u_out)
                if step_idx == 0:
                    add_cap_edge(source, u_in)
                if step_idx == len(chain) - 1:
                    add_cap_edge(u_out, sink)

    for step_idx in range(len(chain) - 1):
        e1 = chain[step_idx]
        e2 = chain[step_idx + 1]
        seen_pairs = set()
        for conn in e1.getConnections(e2):
            fl = conn.getFromLane()
            tl = conn.getToLane()
            if vclass and (not fl.allows(vclass) or not tl.allows(vclass)):
                continue
            pair = (fl.getIndex(), tl.getIndex())
            if pair not in seen_pairs:
                seen_pairs.add(pair)
                add_cap_edge(("OUT", step_idx, pair[0]), ("IN", step_idx + 1, pair[1]))

    flow = 0
    while True:
        parent = {source: None}
        queue = [source]
        while queue and sink not in parent:
            u = queue.pop(0)
            for idx, (v, cap, _rev) in enumerate(adj.get(u, [])):
                if cap > 0 and v not in parent:
                    parent[v] = (u, idx)
                    queue.append(v)
        if sink not in parent:
            break
        flow += 1
        cur = sink
        while parent[cur] is not None:
            prev, e_idx = parent[cur]
            fwd = adj[prev][e_idx]
            fwd[1] -= 1
            adj[cur][fwd[2]][1] += 1
            cur = prev
    return flow


def checkCapacityReductions(net, options):
    vclass = options.vclass if options.vclass is not None else "passenger"

    def num_allowed(edge):
        return sum(1 for lane in edge.getLanes() if not vclass or lane.allows(vclass))

    def continuations(edge):
        ang1 = _edgeTangent(edge, True)
        res = []
        for out_edge in sorted(edge.getOutgoing().keys(), key=lambda e: e.getID()):
            if out_edge == edge.getBidi():
                continue
            diff = abs((ang1 - _edgeTangent(out_edge, False) + 180.0) % 360.0 - 180.0)
            if diff <= options.maxTurnAngle:
                res.append(out_edge)
        return res

    has_long_pred = set()
    for edge in net.getEdges(False):
        if num_allowed(edge) >= 2:
            for out_e in continuations(edge):
                if out_e.getLength() <= options.maxIntermediateLength:
                    has_long_pred.add(out_e)

    reductions = []
    flagged_edges = set()
    for e_start in sorted(net.getEdges(False), key=lambda e: e.getID()):
        n_in = num_allowed(e_start)
        if n_in < 2 or e_start in has_long_pred:
            continue
        stack = [([e_start], 0.0)]
        while stack:
            chain, mid_len = stack.pop(0)
            cur = chain[-1]
            for nxt in continuations(cur):
                if nxt in chain:
                    continue
                new_chain = chain + [nxt]
                if nxt.getLength() <= options.maxIntermediateLength and continuations(nxt):
                    if mid_len + nxt.getLength() <= options.maxIntermediateLength:
                        stack.append((new_chain, mid_len + nxt.getLength()))
                        continue
                n_out = num_allowed(nxt)
                if n_out >= 2:
                    passable = _maxDisjointLanes(new_chain, vclass)
                    expected = min(n_in, n_out)
                    if passable < expected:
                        reductions.append((e_start, nxt, new_chain, n_in, n_out, passable))
                        for ce in new_chain[:-1]:
                            flagged_edges.add(ce.getID())

    for e_start, e_end, chain, n_in, n_out, passable in reductions:
        via_str = ""
        if len(chain) > 2:
            via_str = " via %s" % ",".join(e.getID() for e in chain[1:-1])
        print("Capacity reduction: %s (%s lanes) -> %s (%s lanes)%s: only %s passable lane(s)" % (
            e_start.getID(), n_in, e_end.getID(), n_out, via_str, passable))

    if options.selection_output:
        with open(options.selection_output, 'w') as f:
            for eid in sorted(flagged_edges):
                f.write("edge:%s\n" % eid)
    if reductions:
        print("Found %s capacity reductions" % len(reductions))


if __name__ == "__main__":
    options = parse_args()
    net = sumolib.net.readNet(options.net,
                              withInternal=(options.vclass == "pedestrian"),
                              withPedestrianConnections=(options.vclass == "pedestrian"))
    if options.source:
        getReachable(net, options.source, options)
    elif options.destination:
        getReachable(net, options.destination, options, True)
    elif options.checkRightOfWay:
        checkRightOfWay(net, options)
    elif options.shortTlsEdges:
        checkShortTLSEdges(net, options)
    elif options.capacityReductions:
        checkCapacityReductions(net, options)
    else:
        components = getWeaklyConnected(
            net, options.vclass, options.ignore_connections)
        if len(components) != 1:
            print("Warning! Net is not connected.")

        total = 0
        max = 0
        max_idx = ""
        # Stores the distribution of components by edge counts - key: edge
        # counts - value: number found
        edge_count_dist = {}
        output_str_list = []
        dist_str_list = []

        # Iterate through components to output and summarize
        for idx, comp in enumerate(sorted(components, key=lambda c: next(iter(c)))):
            if options.selection_output:
                with open("{}comp{}.txt".format(options.selection_output, idx), 'w') as f:
                    for e in comp:
                        f.write("edge:{}\n".format(e))
            types = set()
            if options.print_types:
                for e in comp:
                    types.add(net.getEdge(e).getType())
                    if len(types) > 10:
                        break

            edge_count = len(comp)
            total += edge_count
            if edge_count > max:
                max = edge_count
                max_idx = idx

            if edge_count not in edge_count_dist:
                edge_count_dist[edge_count] = 0
            edge_count_dist[edge_count] += 1
            output_str = "Component: #{} Edge Count: {}\n {}\n".format(
                idx, edge_count, " ".join(comp))
            if types:
                output_str += "Type(s): {}\n".format(" ".join(sorted(types)))
            print(output_str)
            output_str_list.append(output_str)

        # Output the summary of all edges checked and largest component
        # To avoid divide by zero error if total is 0 for some reason.
        coverage = 0.0
        if total > 0:
            coverage = round(max * 100.0 / total, 2)
        summary_str = "Total Edges: {}\nLargest Component: #{} Edge Count: {} Coverage: {}%\n".format(
            total, max_idx, max, coverage)
        print(summary_str)
        dist_str = "Edges\tIncidence"
        print(dist_str)
        dist_str_list.append(dist_str)

        # Output the distribution of components by edge counts
        for key, value in sorted(edge_count_dist.items()):
            dist_str = "{}\t{}".format(key, value)
            print(dist_str)
            dist_str_list.append(dist_str)

        # Check for output of components to file
        if options.component_output is not None:
            print("Writing component output to: {}".format(
                options.component_output))
            with open(options.component_output, 'w') as f:
                f.write("\n".join(output_str_list))

        # Check for output of results summary to file
        if options.results_output is not None:
            print(
                "Writing results output to: {}".format(options.results_output))
            with open(options.results_output, 'w') as r:
                r.write(summary_str)
                r.write("\n".join(dist_str_list))
