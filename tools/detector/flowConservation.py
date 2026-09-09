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

# @file    flowConversation.py
# @author  Jakob Erdmann
# @date    2026-09-07

"""
Checks for conservation of flow around junctions by comparing total incoming and
total outgoing flow.
"""

from __future__ import absolute_import
from __future__ import print_function
import math
import sys
import os

from collections import defaultdict

import detector
from detector import relError

SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.xml import parse  # noqa
from sumolib.miscutils import parseTime  # noqa
DEBUG = False


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                    help="define the net file (mandatory)")
    ap.add_argument("-e", "--edgedata-file", dest="edgeDataFile", category="input", required=True, type=ap.edgedata_file,
                    help="read edgeData from FILE (mandatory)", metavar="FILE")
    ap.add_argument("-a", "--edgedata-attribute", category="input", dest="edgeDataAttr", default="count",
                    help="Read edgeData counts from the given attribute")
    ap.add_argument("-i", "--interval", type=ap.time, default="3600",
                    help="aggregation interval in seconds or H:M:S")
    ap.add_argument("-b", "--begin", type=ap.time, default=0,
                    help="begin time in seconds or H:M:S")
    ap.add_argument("--end", type=ap.time, default="1:0:0:0",
                    help="end time in seconds or H:M:S")
    ap.add_argument("--vclass", default="passenger",
                    help="only from and to edges which permit the given vehicle class")
    ap.add_argument("--symmetry-threshold", type=float, default=-1, dest="symmetryThresh",
                    help="if an edge has no data, use the reverse edge data if that value is below FLOAT")
    ap.add_argument("-o", "--output-file", dest="output", category="output", type=ap.file,
                    help="write output to file instead of printing it to console", metavar="FILE")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args()

    if options.output is None:
        options.outfile = sys.stdout
    else:
        options.outfile = open(options.output, 'w')

    return options


def readEdgeData(net, edgeDataFile, begin, end, attr):
    edgeFlow = defaultdict(lambda: 0)
    for interval in parse(edgeDataFile, "interval", attr_conversions={"begin": parseTime, "end": parseTime}):
        if interval.begin < end and interval.end > begin:
            if interval.edge is None:
                continue
            # if read interval is partly outside comparison interval we must scale demand
            validInterval = interval.end - interval.begin
            if interval.begin < begin:
                validInterval -= begin - interval.begin
            if interval.end > end:
                validInterval -= interval.end - end
            scale = validInterval / (interval.end - interval.begin)
            for edge in interval.edge:
                flow = float(edge.getAttribute(attr))
                edgeFlow[net.getEdge(edge.id)] += flow
                # print(interval.begin, interval.end, edge.id, edge.departed, edge.entered, scale, edgeFlow[edge.id])
            if DEBUG:
                print("    validInterval=%s scale=%s" % (validInterval, scale))
    return dict(edgeFlow)


def getFlow(graph, d, edgeFlow, vclass, n):
    """d is the graph direction"""
    nextFn = sumolib.net.edge.Edge.getFromNode if d == 0 else sumolib.net.edge.Edge.getToNode
    nextEdgesFn = sumolib.net.edge.Edge.getAllowedIncoming if d == 0 else sumolib.net.edge.Edge.getAllowedOutgoing
    d2 = 1 - d  # other direction
    seen = set()
    check = graph[n][d][:]
    result = 0
    while check:
        e = check.pop()
        if e in seen:
            continue
        seen.add(e)
        f = edgeFlow.get(e)
        if f is None:
            n2 = nextFn(e)
            if len(graph[n2][d2]) > 1:
                # abort search because we passed a non-simple junction
                return None
            nextEdges = nextEdgesFn(e, vclass).keys()
            if not nextEdges:
                # abort search because we reached a dead-end without finding flow
                return None
            check += nextEdges
        else:
            result += f
    return result


def checkFlow(options, begin, graph, edgeFlow):
    mismatch = dict()  # node -> (inflow, outflow)
    for n, in_out in graph.items():
        if all(in_out):
            mismatch[n] = (getFlow(graph, 0, edgeFlow, options.vclass, n),
                           getFlow(graph, 1, edgeFlow, options.vclass, n))
    
    maxMismatch = []
    for n, (inflow, outflow) in mismatch.items():
        if inflow is not None and outflow is not None:
            maxMismatch.append([inflow - outflow, n.getID(), inflow, outflow])
    maxMismatch.sort()
    for item in maxMismatch:
        options.outfile.write(';'.join(map(str, [begin] + item)) + '\n')


def addSymmetry(options, edgeFlow):
    add = {}
    for e, v in edgeFlow.items():
        for e2 in e.getToNode().getOutgoing():
            if   (e2.getToNode() == e.getFromNode()
                  and e2.allows(options.vclass)
                  and e2 not in edgeFlow
                  and v < options.symmetryThresh):
                add[e2] = edgeFlow[e]
    edgeFlow |= add


def main(options):
    net = sumolib.net.readNet(options.netfile)
    graph = dict()  # node -> (allowed_incoming, allowed_outgoing)
    for n in net.getNodes():
        graph[n] = ([e for e in n.getIncoming() if e.getAllowedOutgoing(options.vclass)],
                [e for e in n.getOutgoing() if e.getAllowedIncoming(options.vclass)])

    begin = options.begin
    options.outfile.write("begin;mismatch;junction;inflow;outflow\n")
    while begin < options.end:
        end = min(begin + options.interval, options.end)
        edgeFlow = readEdgeData(net, options.edgeDataFile, begin, end, options.edgeDataAttr)
        if edgeFlow:
            addSymmetry(options, edgeFlow)
            checkFlow(options, begin, graph, edgeFlow)
        begin += options.interval
    options.outfile.close()


if __name__ == "__main__":
    main(get_options())
