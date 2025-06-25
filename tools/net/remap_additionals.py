#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    remap_additionals.py
# @author  Jakob Erdmann
# @date    2025-02-19

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from math import fabs, degrees

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import sumolib  # noqa
from sumolib.geomhelper import distance  # noqa
from sumolib.xml import parse  # noqa
from sumolib.net import lane2edge  # noqa
import sumolib.geomhelper as gh  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Remap infrastructure from one network to another")
    ap.add_argument("--orig-net", dest="origNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network for loading infrastructure", metavar="FILE")
    ap.add_argument("--target-net", dest="targetNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network for writing infrastructure", metavar="FILE")
    ap.add_argument("-a", "--additional-file", dest="additional", required=True, category="input",
                    type=ap.additional_file,
                    help="File for reading infrastructure", metavar="FILE")
    ap.add_argument("-o", "--output-file", dest="output", required=True, category="output", type=ap.additional_file,
                    help="File for writing infrastructure", metavar="FILE")
    ap.add_argument("--radius", type=float, default=20,
                    help="radius for finding candidate edges")
    ap.add_argument("--shapecut", type=float, default=40,
                    help="Shorten polygon and edge shapes to FLOAT to increase robustness of angular comparison")
    ap.add_argument("--angle-tolerance", type=float, default=20, dest="atol",
                    help="Match polygons and edges if their angle differs by no more than DEGRESS")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                    default=False, help="tell me what you are doing")
    options = ap.parse_args()
    return options


def make_consecutive(net, laneIDs):
    lanes = [net.getLane(laneID) for laneID in laneIDs]
    lanes2 = []
    for i, lane in enumerate(lanes[:-1]):
        lanes2.append(lane)
        lane2 = lanes[i + 1]
        nTo = lane.getEdge().getToNode()
        nFrom = lane2.getEdge().getFromNode()
        if nTo != nFrom:
            path, cost = net.getShortestPath(lane.getEdge(), lane2.getEdge())
            if path:
                for edge in path[1:-1]:
                    index = min(edge.getLaneNumber() - 1, lane.getIndex())
                    lanes2.append(edge.getLanes()[index])
    lanes2.append(lanes[-1])

    return [lane.getID() for lane in lanes2]


def remap_lanes(options, obj, laneIDs, pos=None):
    laneIDs_positions2 = [remap_lane(options, obj, laneID, pos) for laneID in laneIDs.split()]
    laneIDs2 = [la for la, pos in laneIDs_positions2]
    if obj.name in ["e2Detector", "laneAreaDetector"]:
        laneIDs2 = make_consecutive(options.net2, laneIDs2)
    pos2 = laneIDs_positions2[0][1]

    return ' '.join(laneIDs2), pos2


def remap_edges(options, obj, edgeIDs, pos=None):
    edgeIDs_positions2 = [remap_edge(options, obj, edgeID, pos) for edgeID in edgeIDs.split()]
    edgeIDs2 = [e for e, pos in edgeIDs_positions2]
    pos2 = edgeIDs_positions2[0][1]
    return ' '.join(edgeIDs2), pos2


def remap_lane(options, obj, laneID, pos=None):
    lane = options.net.getLane(laneID)
    lane2 = None
    edge = lane.getEdge()
    edge2, pos2 = remap_edge(options, obj, edge.getID(), pos)
    if edge2:
        cands = [c for c in edge.getLanes() if c.getPermissions() == lane.getPermissions()]
        candIndex = cands.index(lane)

        cands2 = [c for c in edge2.getLanes() if c.getPermissions() == lane.getPermissions()]
        if not cands2 and lane.allows("passenger"):
            cands2 = [c for c in edge2.getLanes() if c.allows("passenger")]
        if not cands2 and lane.allows("bus"):
            cands2 = [c for c in edge2.getLanes() if c.allows("bus")]
        if not cands2 and lane.allows("taxi"):
            cands2 = [c for c in edge2.getLanes() if c.allows("taxi")]
        if not cands2 and lane.allows("bicycle"):
            cands2 = [c for c in edge2.getLanes() if c.allows("bicycle")]
        if not cands2:
            cands2 = edge2.getLanes()
        lane2 = cands2[min(candIndex, len(cands2) - 1)].getID()
    return lane2, pos2


def remap_edge(options, obj, edgeID, pos=None):
    edge = options.net.getEdge(edgeID)
    permissions = set(edge.getPermissions())
    shape = edge.getShape()
    shapelen = gh.polyLength(shape)
    relpos = pos / edge.getLength() if pos else 0.5
    if relpos < 0:
        relpos += 1
    x, y = gh.positionAtShapeOffset(shape, shapelen * relpos)
    x2, y2 = options.remap_xy((x, y))

    edges = options.net2.getNeighboringEdges(x2, y2, options.radius)
    if not edges:
        print("No edges near %.2f,%.2f (origEdge %s)" % (x, y, edgeID), file=sys.stderr)
        return None, None

    scut = options.shapecut
    if shapelen < scut:
        origAngle = gh.angleTo2D(shape[0], shape[-1])
    else:
        origAngle = gh.angleTo2D(gh.positionAtShapeOffset(shape, shapelen * relpos - scut / 2),
                                 gh.positionAtShapeOffset(shape, shapelen * relpos + scut / 2))
    cands = []
    for e, d in edges:
        if e.getLength() < scut:
            angle = gh.angleTo2D(e.getFromNode().getCoord(), e.getToNode().getCoord())
        else:
            eShape = e.getShape()
            offset = gh.polygonOffsetWithMinimumDistanceToPoint((x2, y2), eShape)
            offset1 = max(0, offset - scut / 2)
            offset2 = min(gh.polyLength(eShape), offset + scut / 2)
            angle = gh.angleTo2D(gh.positionAtShapeOffset(eShape, offset1),
                                 gh.positionAtShapeOffset(eShape, offset2))
        if degrees(fabs(origAngle - angle)) < options.atol:
            cands.append(e)
    edges = cands
    cands2 = [e for e in edges if permissions.issubset(e.getPermissions())]
    if not cands2:
        # relax permission requirements a minimum
        for svc in ["passenger", "bus", "bicycle" "tram", "rail", "pedestrian"]:
            if svc in permissions:
                permissions = set([svc])
                break
        cands2 = [e for e in edges if permissions.issubset(e.getPermissions())]
        if not cands2:
            print("No edges that allow '%s' found near %.2f,%.2f (origEdge %s)" % (
                " ".join(permissions), x2, y2, edgeID), file=sys.stderr)
            return None, None
    edges = cands2

    if not edges:
        print("No edges with angle %.2f found near %.2f,%.2f (origEdge %s)" % (
            degrees(origAngle), x2, y2, edgeID), file=sys.stderr)
        return None, None

    shape2 = list(map(options.remap_xy, shape))
    edge2 = None
    bestDist = 1e10
    for e in edges:
        if shapelen > e.getLength():
            maxDist = max([gh.distancePointToPolygon(xy, shape2) for xy in e.getShape()])
        else:
            maxDist = max([gh.distancePointToPolygon(xy, e.getShape()) for xy in shape2])
        if maxDist < bestDist:
            bestDist = maxDist
            edge2 = e

    if pos is not None:
        edge2shapelen = gh.polyLength(edge2.getShape())
        pos2 = gh.polygonOffsetWithMinimumDistanceToPoint((x2, y2), edge2.getShape())
        pos2 = pos2 / edge2shapelen * edge2.getLength()

    return edge2, pos2


POSITIONS = ['startPos', 'endPos', 'pos']


def getPosAttrs(obj):
    result = []
    for attr in POSITIONS:
        if obj.hasAttribute(attr):
            result.append((attr, float(getattr(obj, attr))))
    return result


IDS = {
    'edge': remap_edge,
    'lane': remap_lane,
    'edges': remap_edges,
    'lanes': remap_lanes,
}


def remap(options, obj, level=1):
    success = True
    for attr, mapper in IDS.items():
        if obj.hasAttribute(attr):
            posAttrs = getPosAttrs(obj)
            if len(posAttrs) == 0:
                pos = None
            else:
                pos = posAttrs[0][1]
                obj.setAttribute("friendlyPos", True)
            id2, pos2 = mapper(options, obj, getattr(obj, attr), pos)
            if id2:
                obj.setAttribute(attr, id2)
                for posAttr, posOrig in posAttrs:
                    obj.setAttribute(posAttr, pos2 + posOrig - pos)
            else:
                print("Could not map %s on %s '%s'" % (
                    obj.name, attr, getattr(obj, attr)),
                    file=sys.stderr)
                if level == 1:
                    success = False
                else:
                    obj.setCommented()
    for child in obj.getChildList():
        success &= remap(options, child, level + 1)
    patchSpecialCases(options, obj, level)
    return success


def patchSpecialCases(options, obj, level):
    if level == 1:
        accessEdges = set()
        for child in obj.getChildList():
            if child.name == "access" and not child.isCommented():
                edge = lane2edge(child.lane)
                if edge in accessEdges:
                    print("Disabling duplicate access on edge %s" % edge, file=sys.stderr)
                    child.setCommented()
                else:
                    accessEdges.add(edge)
                    lane = options.net2.getLane(child.lane)
                    if not lane.allows("pedestrian"):
                        found = False
                        for cand in lane.getEdge().getLanes():
                            if cand.allows("pedestrian"):
                                child.lane = cand.getID()
                                found = True
                                break
                        if not found:
                            print("Disabling access on non-pedestrian lane %s" % child.lane, file=sys.stderr)
                            child.setCommented()


def main(options):
    if options.verbose:
        print("Reading orig-net '%s'" % options.origNet)
    options.net = sumolib.net.readNet(options.origNet)
    if options.verbose:
        print("Reading target-net '%s'" % options.targetNet)
    options.net2 = sumolib.net.readNet(options.targetNet)

    if options.net.hasGeoProj() and options.net2.hasGeoProj():
        def remap_xy(xy):
            lon, lat = options.net.convertXY2LonLat(*xy)
            return options.net2.convertLonLat2XY(lon, lat)
        options.remap_xy = remap_xy
    else:
        options.remap_xy = lambda x: x

    with open(options.output, 'w') as fout:
        sumolib.writeXMLHeader(fout, "$Id$", "additional", options=options)
        for obj in parse(options.additional):
            if not remap(options, obj):
                obj.setCommented()
            fout.write(obj.toXML(initialIndent=" " * 4))
        fout.write("</additional>\n")


if __name__ == "__main__":
    main(get_options())
