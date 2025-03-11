#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2025 German Aerospace Center (DLR) and others.
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
