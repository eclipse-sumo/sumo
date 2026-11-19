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

# @file    odConnectionsCheck.py
# @author  Daniel Krajzewicz
# @author  Yun-Pang Floetteroed
# @author  Michael Behrisch
# @date    2007-03-20

from __future__ import absolute_import
from __future__ import print_function

import heapq
import itertools
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
else:
    sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import sumolib  # noqa


class Vertex:

    """Store node attributes and incoming/outgoing links."""

    def __init__(self, num):
        self.inEdges = []
        self.outEdges = []
        self.label = "%s" % num
        self.sourceEdges = []
        self.sinkEdges = []

    def __repr__(self):
        return self.label


class Edge:

    """Record link attributes for reachability and travel-time search."""

    def __init__(self, label, source, target, kind="junction"):
        self.label = label
        self.source = source
        self.target = target
        self.capacity = sys.maxsize
        self.kind = kind
        self.maxspeed = 1.0
        self.length = 0.0
        self.freeflowtime = 0.0
        self.numberlane = 0
        self.helpacttime = self.freeflowtime
        self.connection = 0

    def init(self, speed, length, laneNumber):
        self.maxspeed = float(speed)
        self.length = float(length)
        self.numberlane = float(laneNumber)
        if self.source.label == self.target.label or self.maxspeed <= 0:
            self.freeflowtime = 0.0
        else:
            self.freeflowtime = self.length / self.maxspeed
            self.helpacttime = self.freeflowtime

    def __repr__(self):
        return "%s_<%s|%s|%s>" % (self.label, self.kind, self.source, self.target)


class Net:

    def __init__(self):
        self._vertices = []
        self._edges = {}
        self._endVertices = []
        self._startVertices = []
        self.separateZones = []
        self.sinkEdges = []
        self.sourceEdges = []

    def newVertex(self):
        v = Vertex(len(self._vertices))
        self._vertices.append(v)
        return v

    def getEdge(self, edgeLabel):
        return self._edges[edgeLabel]

    def addEdge(self, edgeObj):
        edgeObj.source.outEdges.append(edgeObj)
        edgeObj.target.inEdges.append(edgeObj)
        if edgeObj.kind == "real":
            self._edges[edgeObj.label] = edgeObj

    def addIsolatedRealEdge(self, edgeLabel):
        self.addEdge(Edge(edgeLabel, self.newVertex(), self.newVertex(), "real"))

    def getTargets(self, separateZones):
        target = set()
        for end in self._endVertices:
            if end not in separateZones:
                target.add(end)
        return target

    def checkRoute(self, startVertex, endVertex, totalCounts, subCounts, P, odPairSet, matrixEntry, skipList, verbose):
        totalCounts += 1
        if startVertex.label not in skipList and endVertex.label not in skipList:
            if endVertex not in P:
                subCounts += 1
                odPairSet.append((startVertex.label, endVertex.label, matrixEntry))
                if verbose:
                    print("no connection to", endVertex.label)
        return totalCounts, subCounts, odPairSet


def dijkstra_plain(startVertex, targets):
    """Self-contained shortest-path search replacing tools/purgatory/dijkstra.py."""
    dist = {startVertex: 0.0}
    pred = {}
    remaining = set(targets)
    remaining.discard(startVertex)
    tie = itertools.count()
    heap = [(0.0, next(tie), startVertex)]
    visited = set()

    while heap:
        d, _, u = heapq.heappop(heap)
        if u in visited:
            continue
        visited.add(u)
        if u in remaining:
            remaining.remove(u)
            if not remaining:
                break
        for edge in u.outEdges:
            v = edge.target
            nd = d + edge.helpacttime
            if v not in dist or nd < dist[v]:
                dist[v] = nd
                pred[v] = edge
                heapq.heappush(heap, (nd, next(tie), v))
    return dist, pred


def load_network(net, netfile):
    sumo_net = sumolib.net.readNet(netfile)
    for e in sumo_net.getEdges():
        eid = e.getID()
        net.addIsolatedRealEdge(eid)
        edgeObj = net.getEdge(eid)
        edgeObj.source.label = e.getFromNode().getID()
        edgeObj.target.label = e.getToNode().getID()
        edgeObj.init(e.getSpeed(), e.getLength(), e.getLaneNumber())
    for e in sumo_net.getEdges():
        edgeObj = net.getEdge(e.getID())
        for toEdge in e.getOutgoing().keys():
            if toEdge.getID() in net._edges:
                toObj = net.getEdge(toEdge.getID())
                net.addEdge(Edge(e.getID() + "_" + toEdge.getID(), edgeObj.target, toObj.source))


def load_districts(net, districts_file):
    index = 100
    for taz in sumolib.xml.parse(districts_file, "taz"):
        startIn = net.newVertex()
        startIn.label = taz.id
        endOut = net.newVertex()
        endOut.label = taz.id
        net._startVertices.append(startIn)
        net._endVertices.append(endOut)

        sources = list(taz.tazSource) if taz.tazSource else []
        sinks = list(taz.tazSink) if taz.tazSink else []
        if taz.hasAttribute("edges"):
            for eid in taz.edges.split():
                if eid in net._edges:
                    index += 1
                    srcEdge = Edge(taz.id + str(index), startIn, net.getEdge(eid).source, "real")
                    srcEdge.weight = "1.0"
                    srcEdge.connection = 2
                    net.addEdge(srcEdge)
                    index += 1
                    snkEdge = Edge(taz.id + str(index), net.getEdge(eid).target, endOut, "real")
                    snkEdge.weight = "1.0"
                    snkEdge.connection = 1
                    net.addEdge(snkEdge)
        for src in sources:
            if src.id in net._edges:
                sourcelink = net.getEdge(src.id)
                index += 1
                newEdge = Edge(taz.id + str(index), startIn, sourcelink.source, "real")
                newEdge.weight = src.getAttributeSecure("weight", "1.0")
                newEdge.connection = 2
                net.addEdge(newEdge)
        for snk in sinks:
            if snk.id in net._edges:
                sinklink = net.getEdge(snk.id)
                index += 1
                newEdge = Edge(taz.id + str(index), sinklink.target, endOut, "real")
                newEdge.weight = snk.getAttributeSecure("weight", "1.0")
                newEdge.connection = 1
                net.addEdge(newEdge)


def getMatrix(net, matrix):
    """Read VISUM $VM demand matrix from the given file."""
    matrixPshort = []
    startVertices = []
    endVertices = []
    Pshort_EffCells = 0
    periodList = []

    ODpairs = 0
    origins = 0
    skipCount = 0
    zones = 0
    smallDemandNum = 0
    with sumolib.openz(matrix) as mf:
        for raw_line in mf:
            line = raw_line.strip()
            if not line:
                continue
            if line[0] == '$':
                visumCode = line[1:3]
                if visumCode != 'VM':
                    skipCount += 1
            elif line[0] != '*':
                skipCount += 1
                if skipCount == 2:
                    for elem in line.split():
                        periodList.append(float(elem))
                elif skipCount > 3:
                    if zones == 0:
                        for elem in line.split():
                            zones = int(elem)
                    elif len(startVertices) < zones:
                        for elem in line.split():
                            if elem:
                                for startVertex in net._startVertices:
                                    if startVertex.label == elem:
                                        startVertices.append(startVertex)
                                for endVertex in net._endVertices:
                                    if endVertex.label == elem:
                                        endVertices.append(endVertex)
                        origins = len(startVertices)
                    elif len(startVertices) == zones and origins > 0:
                        if ODpairs % origins == 0:
                            matrixPshort.append([])
                        for item in line.split():
                            val = float(item)
                            matrixPshort[-1].append(val)
                            ODpairs += 1
                            if val > 0.:
                                Pshort_EffCells += 1
                            if 0. < val < 1.:
                                smallDemandNum += 1
    if Pshort_EffCells > 0:
        _ = float(smallDemandNum) / float(Pshort_EffCells)
    return matrixPshort, startVertices, endVertices


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Check OD connectivity between traffic zones")
    ap.add_option("-r", "--data-dir", dest="datadir", default=".",
                  help="give the data directory path")
    ap.add_option("-n", "--net-file", dest="netfile", required=True, category="input", type=ap.net_file,
                  help="define the net file")
    ap.add_option("-m", "--matrix-file", dest="mtxfile", category="input", type=ap.file,
                  help="define the matrix file")
    ap.add_option("-d", "--districts-file", dest="districtfile", required=True, category="input",
                  type=ap.additional_file, help="define the district file")
    ap.add_option("-o", "--output-file", dest="output", default="absentConnections.txt", category="output",
                  type=ap.file, help="output file for absent OD connections")
    ap.add_option("-v", "--verbose", action="store_true", dest="verbose",
                  default=False, help="tell me what you are doing")
    ap.add_option("-i", "--separate-zones", dest="separatezones", type=str,
                  help="define the zones which should be separated")
    ap.add_option("-s", "--skip-list", dest="skipList", type=str,
                  help="define the zones which will not be compared with each other")
    ap.add_option("-b", "--debug", action="store_true", dest="debug",
                  default=False, help="debug the program")
    return ap.parse_args(args=args)


def resolve_path(dataDir, p):
    if os.path.isabs(p) or os.path.exists(p):
        return p
    return os.path.join(dataDir, p)


def main(args=None):
    options = get_options(args=args)
    dataDir = options.datadir
    districts = resolve_path(dataDir, options.districtfile)
    netfile = resolve_path(dataDir, options.netfile)
    sumDemand = 0.
    totalCounts = 0
    subCounts = 0
    separateZoneLabels = set(options.separatezones.split(',')) if options.separatezones else set()
    odPairSet = []
    skipList = options.skipList.split(',') if options.skipList else []

    net = Net()
    load_network(net, netfile)
    load_districts(net, districts)

    separateStart = {d for d in net._startVertices if d.label in separateZoneLabels}
    separateEnd = {d for d in net._endVertices if d.label in separateZoneLabels}

    if options.mtxfile:
        matrixPshort, startVertices, endVertices = getMatrix(net, resolve_path(dataDir, options.mtxfile))
    else:
        matrixPshort = None
        startVertices = net._startVertices
        endVertices = net._endVertices

    if options.verbose:
        print(len([e for e in net._edges.values() if e.connection == 0]), "edges read")
        print(len(startVertices), "start vertices read")
        print(len(endVertices), "target vertices read")

    for start, startVertex in enumerate(startVertices):
        if startVertex not in separateStart:
            targets = net.getTargets(separateEnd)
            if options.verbose:
                print("checking start vertex", startVertex.label)
            _, P = dijkstra_plain(startVertex, targets)
            for end, endVertex in enumerate(endVertices):
                if startVertex.label != endVertex.label and endVertex not in separateEnd:
                    entry = matrixPshort[start][end] if matrixPshort else 1
                    if entry > 0.:
                        totalCounts, subCounts, odPairSet = net.checkRoute(
                            startVertex, endVertex, totalCounts, subCounts, P, odPairSet,
                            entry, skipList, options.verbose)
        else:
            _, P = dijkstra_plain(startVertex, separateEnd)
            for end, endVertex in enumerate(endVertices):
                if endVertex in separateEnd and startVertex.label != endVertex.label:
                    entry = matrixPshort[start][end] if matrixPshort else 1
                    if entry > 0.:
                        totalCounts, subCounts, odPairSet = net.checkRoute(
                            startVertex, endVertex, totalCounts, subCounts, P, odPairSet,
                            entry, skipList, options.verbose)

    print('total OD connections:', totalCounts)
    if len(odPairSet) > 0:
        with sumolib.openz(options.output, 'w') as foutzones:
            for pair in odPairSet:
                sumDemand += float(pair[2])
                foutzones.write('from: %s   to: %s; demand:%s\n' % (pair[0], pair[1], pair[2]))
        print(subCounts, 'connections are absent!')
        print(sumDemand, 'vehicles are absent.')
    else:
        print('all connections exist! ')


if __name__ == "__main__":
    main()
