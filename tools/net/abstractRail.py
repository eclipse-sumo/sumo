#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    abstractRail.py
# @author  Jakob Erdmann
# @date    2023-02-22

"""
Convert a geodetical rail network in a abstract (schematic) rail network.
If the network is segmented (stationDistricts.py), the resulting network will be
a hybrid of multple schematic pieces being oriented in a roughly geodetical manner
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import subprocess
import numpy as np
import math
import time
import scipy.optimize as opt
from collections import defaultdict
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.miscutils import Colorgen  # noqa
import sumolib.geomhelper as gh  # noqa

try:
    sys.stdout.reconfigure(encoding='utf-8')
except:  # noqa
    pass

INTERSECT_RANGE = 1e5
COMPRESSION_WEIGHT = 0.01
STRAIGHT_WEIGHT = 2
OTHER_WEIGHT = 1

NETCONVERT = sumolib.checkBinary('netconvert')
STATION_DISTRICTS = os.path.join(SUMO_HOME, 'tools', 'district', 'stationDistricts.py')


def get_options():
    ap = ArgumentParser()
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    ap.add_option("-n", "--net-file", dest="netfile", required=True, type=ap.net_file,
                  help="the network to read lane and edge permissions")
    ap.add_option("-s", "--stop-file", dest="stopfile", required=True, type=ap.additional_file,
                  help="the additional file with stops")
    ap.add_option("-a", "--region-file", dest="regionfile", type=ap.additional_file,
                  help="Load network regions from additional file (as taz elements)")
    ap.add_option("-o", "--output-prefix", dest="prefix", required=True, type=ap.file,
                  help="output prefix for patch files")
    ap.add_option("--split", action="store_true", default=False,
                  help="automatically create a region file from the loaded stops,"
                  + " automatically split the network if needed")
    ap.add_option("--filter-regions", dest="filterRegions",
                  help="filter regions by name or id")
    ap.add_option("--keep-all", action="store_true", dest="keepAll", default=False,
                  help="keep original regions outside the filtered regions")
    ap.add_option("--horizontal", action="store_true", dest="horizontal", default=False,
                  help="output shapes roughly aligned along the horizontal")
    ap.add_option("--track-offset", type=float, default=20, dest="trackOffset",
                  help="default distance between parallel tracks")
    ap.add_option("--track-length", type=float, default=20, dest="trackLength",
                  help="maximum length of track pieces")
    ap.add_option("--time-limit", type=float, dest="timeLimit",
                  help="time limit per region")
    ap.add_option("--max-iter", type=int, dest="maxIter",
                  help="maximum number of solver iterations per region")
    ap.add_option("--skip-large", type=int, dest="skipLarge",
                  help="skip regions require more than the given number of constraints")
    ap.add_option("--skip-yopt", action="store_true", dest="skipYOpt", default=False,
                  help="do not optimize the track offsets")
    ap.add_option("--skip-building", action="store_true", dest="skipBuilding", default=False,
                  help="do not call netconvert with the patch files")
    ap.add_option("--extra-verbose", action="store_true", default=False, dest="verbose2",
                  help="tell me more about what you are doing")
    options = ap.parse_args()

    if not options.netfile:
        ap.print_help()
        ap.exit("Error! setting net-file is mandatory")
    if not options.stopfile:
        ap.print_help()
        ap.exit("Error! setting stop-file is mandatory")
    if not options.prefix:
        ap.print_help()
        ap.exit("Error! setting output is mandatory")
    if options.regionfile and options.split:
        ap.print_help()
        ap.exit("Error! Only one of the options --split or --region-file may be given")

    options.output_nodes = options.prefix + ".nod.xml"
    options.output_edges = options.prefix + ".edg.xml"
    options.output_net = options.prefix + ".net.xml"

    options.filterRegions = set(options.filterRegions.split(",")) if options.filterRegions else set()

    return options


def loadRegions(options, net):
    regions = dict()
    if options.regionfile:
        for taz in sumolib.xml.parse(options.regionfile, 'taz'):
            name = taz.attr_name
            if (options.filterRegions
                and name not in options.filterRegions
                    and taz.id not in options.filterRegions):
                continue
            edgeIDs = taz.edges.split()
            regions[name] = [net.getEdge(e) for e in edgeIDs if net.hasEdge(e)]
    else:
        regions['ALL'] = list(net.getEdges())
    return regions


def filterBidi(edges):
    return [e for e in edges if e.getBidi() is None or e.getID() < e.getBidi().getID()]


def initShapes(edges, nodeCoords, edgeShapes):
    nodes = getNodes(edges)

    for node in nodes:
        nodeCoords[node.getID()] = node.getCoord()

    for edge in edges:
        edgeShapes[edge.getID()] = edge.getShape(True)


def findMainline(options, net, edges):
    """use platforms to determine mainline orientation"""
    knownEdges = set([e.getID() for e in edges])

    angles = []
    for stop in sumolib.xml.parse(options.stopfile, ['busStop', 'trainStop']):
        # name = stop.getAttributeSecure("attr_name", stop.id)
        edgeID = stop.lane.rsplit('_', 1)[0]
        if edgeID not in knownEdges:
            continue
        edge = net.getEdge(edgeID)
        begCoord = gh.positionAtShapeOffset(edge.getShape(), float(stop.startPos))
        endCoord = gh.positionAtShapeOffset(edge.getShape(), float(stop.endPos))
        angles.append((gh.angleTo2D(begCoord, endCoord), (begCoord, endCoord)))

    angles.sort()
    mainLine = angles[int(len(angles) / 2)][1]

    if options.verbose2:
        print("mainLine=", shapeStr(mainLine))

    return mainLine


def getNodes(edges):
    nodes = set()
    for edge in edges:
        nodes.add(edge.getFromNode())
        nodes.add(edge.getToNode())
    return nodes


def rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, reverse,
                     horizontal=False, multiRegions=False):
    center = mainLine[0]
    angle = gh.angleTo2D(mainLine[0], mainLine[1])
    nodes = getNodes(edges)

    if reverse:
        def transform(coord):
            if not horizontal:
                coord = gh.rotateAround2D(coord, angle, (0, 0))
            if multiRegions or not horizontal:
                coord = gh.add(coord, center)
            return coord
    else:
        def transform(coord):
            coord = gh.sub(coord, center)
            coord = gh.rotateAround2D(coord, -angle, (0, 0))
            return coord

    for node in nodes:
        coord = nodeCoords[node.getID()]
        nodeCoords[node.getID()] = transform(coord)

    for edge in edges:
        shape = edgeShapes[edge.getID()]
        edgeShapes[edge.getID()] = [transform(coord) for coord in shape]


def getEdgeOrdering(edgeIDs, ordering, useOutgoing):
    result = []
    for vN in ordering:
        if type(vN) == sumolib.net.edge.Edge:
            result.append(vN.getID())
        else:
            edges = vN.getOutgoing() if useOutgoing else vN.getIncoming()
            # filter bidi
            edges = [e.getID() for e in edges if e.getID() in edgeIDs]
            if len(edges) == 1:
                result.append(edges[0])
            else:
                result.append(None)
    return result


def differentOrderings(edgeIDs, o1, o2):
    """
    check whether two orderings are different
    """
    if o2 is None or len(o1) != len(o2):
        return True

    # convert node to edge (two possibilities for each ordering)
    o1a = getEdgeOrdering(edgeIDs, o1, True)
    o1b = getEdgeOrdering(edgeIDs, o1, False)
    o2a = getEdgeOrdering(edgeIDs, o2, True)
    o2b = getEdgeOrdering(edgeIDs, o2, False)

    for o1x in [o1a, o1b]:
        for o2x in [o2a, o2b]:
            if o1x == o2x:
                return False
    return True


def computeTrackOrdering(options, mainLine, edges, nodeCoords, edgeShapes):
    """
    precondition: network is rotated so that the mainLine is on the horizontal
    for each x-value we imagine a vertical line and find all the edges that intersect
    this gives a list of track orderings
    Then we try to assign integers for each edge that are consistent with this ordering
    """

    # step 1: find ordering constraints
    orderings = []
    nodes = getNodes(edges)
    edgeIDs = set([e.getID() for e in edges])
    xyNodes = [(nodeCoords[n.getID()], n) for n in nodes]
    xyNodes.sort(key=lambda x: x[0][0])

    prevOrdering = None
    sameOrdering = []
    for (x, y), node in xyNodes:
        node._newX = x
        vertical = [(x, -INTERSECT_RANGE), (x, INTERSECT_RANGE)]
        ordering = []
        for edge in edges:
            if edge.getFromNode() == node or edge.getToNode() == node:
                continue
            shape = edgeShapes[edge.getID()]
            intersects = gh.intersectsAtLengths2D(vertical, shape)
            intersects = [i - INTERSECT_RANGE for i in intersects]
            # intersects now holds y-values
            if len(intersects) == 1:
                ordering.append((intersects[0], edge))
            elif len(intersects) > 1:
                sys.stderr.write(("Cannot compute track ordering for edge '%s'" +
                                  " because it runs orthogonal to the main line (intersects: %s)\n") % (
                    edge.getID(), intersects))
                # print("vertical=%s %s=%s" % (shapeStr(vertical), edge.getID(), shapeStr(shape)))

        if ordering:
            # also append the actual node before sorting
            ordering.append((y, node))
            ordering.sort(key=lambda x: x[0])
            ordering = [vn for y, vn in ordering]

            if differentOrderings(edgeIDs, ordering, prevOrdering):
                orderings.append((node.getID(), ordering))
                prevOrdering = ordering
                if options.verbose2:
                    print(x, list(map(lambda x: x.getID(), ordering)))
            else:
                sameOrdering.append((prevOrdering, ordering))
                if options.verbose2:
                    print("sameOrdering:", prevOrdering, ordering)

    # step 3:
    nodeYValues = optimizeTrackOrder(options, edges, nodes, orderings, nodeCoords)

    # step 4: apply yValues to virtual nodes that were skipped
    for prevOrdering, ordering in sameOrdering:
        for n1, n2 in zip(prevOrdering, ordering):
            nodeYValues[n2] = nodeYValues[n1]

    if options.verbose2:
        for k, v in nodeYValues.items():
            print(k.getID(), v)
    return nodeYValues


def optimizeTrackOrder(options, edges, nodes, orderings, nodeCoords):
    constrainedEdges = set()
    for nodeID, ordering in orderings:
        for vNode in ordering:
            if type(vNode) == sumolib.net.edge.Edge:
                constrainedEdges.add(vNode)

    # every node and every edge is assigned a single y-values
    generalizedNodes = list(nodes) + list(constrainedEdges)
    generalizedNodes.sort(key=lambda n: n.getID())
    nodeIndex = dict((n, i) for i, n in enumerate(generalizedNodes))

    # collect ordering constraints for nodes and virtual nodes
    yOrderConstraints = []
    for nodeID, ordering in orderings:
        for vNode, vNode2 in zip(ordering[:-1], ordering[1:]):
            yOrderConstraints.append((nodeIndex[vNode], nodeIndex[vNode2]))

    # collect constraints for keeping edges parallel
    yPrios = []
    ySimilarConstraints = []
    for edge in edges:
        angle = gh.angleTo2D(nodeCoords[edge.getFromNode().getID()], nodeCoords[edge.getToNode().getID()])
        straight = min(abs(angle), abs(angle - math.pi)) < np.deg2rad(10)
        if edge in constrainedEdges:
            numConstraints = 2
            ySimilarConstraints.append((nodeIndex[edge.getFromNode()], nodeIndex[edge]))
            ySimilarConstraints.append((nodeIndex[edge.getToNode()], nodeIndex[edge]))
        else:
            numConstraints = 1
            ySimilarConstraints.append((nodeIndex[edge.getFromNode()], nodeIndex[edge.getToNode()]))
        yPrios += [2 if straight else 1] * numConstraints

    k = len(generalizedNodes)
    m = len(ySimilarConstraints)
    m2 = m * 2
    # n: number of variables
    n = k + m
    # q: number of equations:
    #   2 per ySame constraint to minimize the absolute difference and one per elementary ordering constraint
    q = m2 + len(yOrderConstraints)

    # we use m slack variables for the differences between y-values (one per edge)
    # x2 =  [x, s]

    b_ub = [0] * m2 + [-options.trackOffset] * len(yOrderConstraints)
    A_ub = np.zeros((q, n))

    row = 0
    # encode inequalities for slack variables (minimize differences)
    for slackI, (fromI, toI) in enumerate(ySimilarConstraints):
        slackI += k
        A_ub[row][fromI] = 1
        A_ub[row][toI] = -1
        A_ub[row][slackI] = -1
        row += 1
        A_ub[row][fromI] = -1
        A_ub[row][toI] = 1
        A_ub[row][slackI] = -1
        row += 1

    # encode inequalities for ordering
    for index1, index2 in yOrderConstraints:
        A_ub[row][index1] = 1
        A_ub[row][index2] = -1
        row += 1

    # minimization objective (only minimize slack variables)
    c = [COMPRESSION_WEIGHT] * k + yPrios

    if options.verbose2:
        print("k=%s" % k)
        print("m=%s" % m)
        print("q=%s" % q)
        print("A_ub (%s) %s" % (A_ub.shape, A_ub))
        print("b_ub (%s) %s" % (len(b_ub), b_ub))
        print("c (%s) %s" % (len(c), c))

    if options.skipLarge and q > options.skipLarge:
        sys.stderr.write("Skipping optimization with %s inequalities\n" % q)
        return dict()

    linProgOpts = {}
    started = time.time()
    if options.verbose:
        print("Starting optimization with %s inequalities" % q)

    if options.verbose2:
        linProgOpts["disp"] = True

    if options.timeLimit:
        linProgOpts["time_limit"] = options.timeLimit

    if options.maxIter:
        linProgOpts["maxiter"] = options.maxIter

    res = opt.linprog(c, A_ub=A_ub, b_ub=b_ub, options=linProgOpts)

    if not res.success:
        sys.stderr.write("Optimization failed\n")
        return dict()

    if options.verbose:
        if options.verbose:
            score = np.dot(res.x, c)
            runtime = time.time() - started
            print("Optimization succeeded after %ss (score=%s)" % (runtime, score))
        if options.verbose2:
            print(res.x)
    yValues = res.x[:k]  # cut of slack variables
    # print(yValues)

    nodeYValues = dict([(vNode, yValues[i]) for vNode, i in nodeIndex.items()])
    return nodeYValues


def patchShapes(options, edges, nodeCoords, edgeShapes, nodeYValues):
    nodes = getNodes(edges)

    # patch node y-values
    edgeYValues = defaultdict(list)
    for node in nodes:
        coord = nodeCoords[node.getID()]
        nodeCoords[node.getID()] = (coord[0], nodeYValues[node])

    # compute inner edge shape points (0 or 2 per edge)
    for vNode, y in nodeYValues.items():
        if vNode not in nodes:
            fromID = vNode.getFromNode().getID()
            toID = vNode.getToNode().getID()
            x1, y1 = nodeCoords[fromID]
            x2, y2 = nodeCoords[toID]
            xDelta = x2 - x1
            length = abs(xDelta)
            yDelta1 = abs(y1 - y)
            yDelta2 = abs(y2 - y)
            xOffset1 = min(yDelta1, length / 2)
            xOffset2 = min(yDelta2, length / 2)
            sign = 1 if x2 > x1 else -1
            edgeYValues[vNode.getID()].append((x1 + xOffset1 * sign, y))
            edgeYValues[vNode.getID()].append((x2 - xOffset2 * sign, y))

    # set edge shapes
    for edge in edges:
        edgeID = edge.getID()
        fromCoord = nodeCoords[edge.getFromNode().getID()]
        toCoord = nodeCoords[edge.getToNode().getID()]
        shape = [fromCoord, toCoord]
        if edgeID in edgeYValues:
            for coord in edgeYValues[edgeID]:
                shape.append(coord)
            shape.sort()
            if fromCoord[0] > toCoord[0]:
                shape = list(reversed(shape))
        edgeShapes[edgeID] = shape


def patchXValues(options, edges, nodeCoords, edgeShapes):
    nodes = getNodes(edges)
    xValues = set()
    for node in nodes:
        xValues.add(nodeCoords[node.getID()][0])
    for edge in edges:
        xValues.update([c[0] for c in edgeShapes[edge.getID()]])

    xValues = sorted(list(xValues))
    xValues2 = list(xValues)
    baseRank = getIndexOfClosestToZero(xValues)
    for i in range(len(xValues2) - 1):
        dist = xValues2[i + 1] - xValues2[i]
        if dist > options.trackLength:
            for j in range(i + 1):
                xValues2[j] += dist - options.trackLength
    # shift the central point back to 0
    shift = xValues2[baseRank]
    xValues2 = [x - shift for x in xValues2]
    xMap = dict(zip(xValues, xValues2))

    for node in nodes:
        coord = nodeCoords[node.getID()]
        nodeCoords[node.getID()] = (xMap[coord[0]], coord[1])
    for edge in edges:
        shape = edgeShapes[edge.getID()]
        edgeShapes[edge.getID()] = [(xMap[c[0]], c[1]) for c in shape]


def getIndexOfClosestToZero(values):
    result = None
    bestValue = 1e100
    for i, v in enumerate(values):
        if abs(v) < bestValue:
            result = i
            bestValue = abs(v)
    return result


def cleanShapes(options, net, nodeCoords, edgeShapes):
    """Ensure consistent edge shape in case the same edge was part of multiple regions"""
    for edgeID, shape in edgeShapes.items():
        edge = net.getEdge(edgeID)
        fromID = edge.getFromNode().getID()
        toID = edge.getToNode().getID()
        shape[0] = nodeCoords[fromID]
        shape[-1] = nodeCoords[toID]


def shapeStr(shape):
    return ' '.join(["%.2f,%.2f" % coord for coord in shape])


def splitNet(options):
    # 1. create region file from stops
    options.regionfile = options.prefix + ".taz.xml"
    splitfile = options.prefix + ".split.edg.xml"
    oldNet = options.netfile
    oldStops = options.stopfile

    if options.verbose:
        print("Creating region file '%s'" % options.regionfile)
    subprocess.call([sys.executable, STATION_DISTRICTS,
                     '-n', options.netfile,
                     '-s', options.stopfile,
                     '-o', options.regionfile,
                     '--split-output', splitfile])

    # 2. optionally split the network if regions have shared edges
    numSplits = len(list(sumolib.xml.parse(splitfile, 'edge')))
    if numSplits > 0:
        if options.verbose:
            print("Splitting %s edges to ensure distinct regions" % numSplits)

        # rebuilt the network and stop file
        options.netfile = options.netfile[:-8] + ".split.net.xml"
        if options.stopfile[-8:] == ".add.xml":
            options.stopfile = options.stopfile[:-8] + ".split.add.xml"
        else:
            options.stopfile = options.stopfile[:-4] + ".split.xml"

        subprocess.call([NETCONVERT,
                         '-e', splitfile,
                         '-s', oldNet,
                         '-o', options.netfile,
                         '--ptstop-files', oldStops,
                         '--ptstop-output', options.stopfile])

        if options.verbose:
            print("Re-creating region file '%s' after splitting network" % options.regionfile)
        subprocess.call([sys.executable, STATION_DISTRICTS,
                         '-n', options.netfile,
                         '-s', options.stopfile,
                         '-o', options.regionfile])


def main(options):
    if options.split:
        splitNet(options)

    if options.verbose:
        print("Reading net")
    net = sumolib.net.readNet(options.netfile)

    regions = loadRegions(options, net)
    multiRegions = len(regions) > 1
    nodeCoords = dict()
    edgeShapes = dict()

    for name, edges in regions.items():
        edges = filterBidi(edges)
        if options.verbose:
            print("Processing region '%s' with %s edges" % (name, len(edges)))
        initShapes(edges, nodeCoords, edgeShapes)
        mainLine = findMainline(options, net, edges)
        rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, False)
        if not options.skipYOpt:
            nodeYValues = computeTrackOrdering(options, mainLine, edges, nodeCoords, edgeShapes)
            if nodeYValues:
                patchShapes(options, edges, nodeCoords, edgeShapes, nodeYValues)
                if options.trackLength:
                    patchXValues(options, edges, nodeCoords, edgeShapes)
        rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, True, options.horizontal, multiRegions)

    if len(regions) > 1:
        cleanShapes(options, net, nodeCoords, edgeShapes)

    with open(options.output_nodes, 'w') as outf_nod:
        sumolib.writeXMLHeader(outf_nod, "$Id$", "nodes", options=options)
        for nodeID in sorted(nodeCoords.keys()):
            coord = nodeCoords[nodeID]
            outf_nod.write('    <node id="%s" x="%.2f" y="%.2f"/>\n' % (
                nodeID, coord[0], coord[1]))
        outf_nod.write("</nodes>\n")

    with open(options.output_edges, 'w') as outf_edg:
        sumolib.writeXMLHeader(outf_edg, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for edgeID in sorted(edgeShapes.keys()):
            shape = edgeShapes[edgeID]
            edge = net.getEdge(edgeID)
            outf_edg.write('    <edge id="%s" shape="%s" length="%.2f"/>\n' % (
                edgeID, shapeStr(shape), edge.getLength()))
            if edge.getBidi():
                outf_edg.write('    <edge id="%s" shape="%s" length="%.2f"/>\n' % (
                    edge.getBidi().getID(), shapeStr(reversed(shape)), edge.getLength()))
        # remove the rest of the network
        if options.filterRegions and not options.keepAll:
            for edge in net.getEdges():
                if edge.getID() not in edgeShapes and edge.getBidi() not in edgeShapes:
                    outf_edg.write('    <delete id="%s"/>\n' % (edge.getID()))
        outf_edg.write("</edges>\n")

    if not options.skipBuilding:
        if options.verbose:
            print("Building new net")
        sys.stderr.flush()
        subprocess.call([NETCONVERT,
                         '-s', options.netfile,
                         '-n', options.output_nodes,
                         '-e', options.output_edges,
                         '-o', options.output_net,
                         ], stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    main(get_options())
