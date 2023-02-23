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
import random
import subprocess
from heapq import heappush,heappop
from collections import defaultdict
from itertools import chain
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib
from sumolib.options import ArgumentParser  # noqa
from sumolib.miscutils import Colorgen  # noqa
import sumolib.geomhelper as gh


def get_options():
    ap = ArgumentParser()
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    ap.add_option("-n", "--net-file", dest="netfile",
                  help="the network to read lane and edge permissions")
    ap.add_option("-s", "--stop-file", dest="stopfile",
                  help="the additional file with stops")
    ap.add_option("-a", "--region-file", dest="regionfile",
                  help="Load network regions from additional file (as taz elements)")
    ap.add_option("-o", "--output-prefix", dest="prefix",
                  help="output prefix for patch files")
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

    options.output_nodes = options.prefix + ".nod.xml"
    options.output_edges = options.prefix + ".edg.xml"
    options.output_net = options.prefix + ".net.xml"

    return options


def loadRegions(options, net):
    regions = dict()
    if options.regionfile:
        for taz in sumolib.xml.parse(options.regionfile, 'taz'):
            edgeIDs = taz.edges.split()
            regions[taz.name] = [net.getEdge(e) for e in edges if net.hasEdge(e)]
    else:
        regions['ALL'] = list(net.getEdges())
    return regions


def findMainline(options, net, edges):
    """use platforms to determine mainline orientation"""
    knownEdges = set([e.getID() for e in edges])

    platforms = []
    for stop in sumolib.xml.parse(options.stopfile, ['busStop', 'trainStop']):
        name = stop.getAttributeSecure("attr_name", stop.id)
        edgeID = stop.lane.rsplit('_', 1)[0]
        if not edgeID in knownEdges:
            continue
        edge = net.getEdge(edgeID)
        begCoord = gh.positionAtShapeOffset(edge.getShape(), float(stop.startPos))
        endCoord = gh.positionAtShapeOffset(edge.getShape(), float(stop.endPos))
        platforms.append(begCoord)
        platforms.append(endCoord)

    coordsByX = sorted(platforms, key=lambda coord : coord[0])
    coordsByY = sorted(platforms, key=lambda coord : coord[1])
    xDelta = coordsByX[-1][0] - coordsByX[0][0]
    yDelta = coordsByY[-1][1] - coordsByY[0][1]

    mainLine = None
    if xDelta > yDelta:
        mainLine = [coordsByX[-1], coordsByX[0]]
    else:
        mainLine = [coordsByY[-1], coordsByY[0]]

    return mainLine


def rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, reverse):
    nodes = set()
    center = mainLine[0]
    angle = gh.angleTo2D(mainLine[0], mainLine[1])

    for edge in edges:
        nodes.add(edge.getFromNode())
        nodes.add(edge.getToNode())

    # init
    for node in nodes:
        if node.getID() not in nodeCoords:
            nodeCoords[node.getID()] = node.getCoord()

    if reverse:
        def transform(coord):
            coord = gh.rotateAround2D(coord, -angle, (0, 0))
            coord = gh.add(coord, center)
            return coord
    else:
        def transform(coord):
            coord = gh.sub(coord, center)
            coord = gh.rotateAround2D(coord, angle, (0, 0))
            return coord

    for node in nodes:
        coord = nodeCoords[node.getID()]
        nodeCoords[node.getID()] = transform(coord)

    for edge in edges:
        if edge.getID() in edgeShapes:
            shape = edgeShapes[edge.getID()]
            edgeShapes[edge.getID()] = [transform(coord) for coord in shape]


def patchShapes(options, edges, nodeCoords, edgeShapes):
    for edge in edges:
        edgeShapes[edge.getID()] = [nodeCoords[edge.getFromNode().getID()],
                                    nodeCoords[edge.getToNode().getID()]]


def main(options):

    if options.verbose:
        print("Reading net")
    net = sumolib.net.readNet(options.netfile)

    regions = loadRegions(options, net)
    nodeCoords = dict()
    edgeShapes = dict()

    for edges in regions.values():
        mainLine = findMainline(options, net, edges)
        rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, False)
        
        #computeTrackOrdering(edges, mainLine)
        #computeDistinctHorizontalPoints()
        #squeezeHorizontal(edges)

        patchShapes(options, edges, nodeCoords, edgeShapes)
        #rotateByMainLine(mainLine, edges, nodeCoords, edgeShapes, True)

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
                edgeID,
                ' '.join(["%.2f,%.2f" % coord for coord in shape]),
                edge.getLength()))
        outf_edg.write("</edges>\n")

    NETCONVERT = sumolib.checkBinary('netconvert')
    subprocess.call([NETCONVERT,
        '-s', options.netfile,
        '-n', options.output_nodes,
        '-e', options.output_edges,
        '-o', options.output_net,
        ])

if __name__ == "__main__":
    main(get_options())
