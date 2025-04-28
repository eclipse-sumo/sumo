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

# @file    remap_network.py
# @author  Jakob Erdmann
# @date    2025-03-26

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.geomhelper import distance  # noqa
from sumolib.xml import parse  # noqa
from sumolib.net import lane2edge  # noqa
from sumolib.statistics import Statistics  # noqa
import sumolib.geomhelper as gh  # noqa

SLACK = 1


def get_options(args=None):
    desc = "Identify which edges in the target network correspond to edges in the original network"
    ap = sumolib.options.ArgumentParser(description=desc)
    ap.add_argument("--orig-net", dest="origNet", required=True, category="input", type=ap.net_file,
                    help="SUMO network with edges of interest", metavar="FILE")
    ap.add_argument("--target-net", dest="targetNet", required=True, category="input", type=ap.net_file, metavar="FILE",
                    help="SUMO network with edge ids that should be used to represent the original edges")
    ap.add_argument("-o", "--output-file", dest="output", required=True, category="output", type=ap.file,
                    help="File for writing mapping information", metavar="FILE")
    ap.add_argument("-s", "--success-output", dest="successOutput", category="output", type=ap.file,
                    help="File for writing mapping success information", metavar="FILE")
    ap.add_argument("--radius", type=float, default=1.6,
                    help="radius for finding candidate edges")
    ap.add_argument("--min-common", type=float, dest="minCommon", default=0.1,
                    help="minimum common length in meters")
    ap.add_argument("--filter-ids", dest="filterIds",
                    help="only handle the given edges")
    ap.add_argument("-v", "--verbose", action="store_true", dest="verbose",
                    default=False, help="tell me what you are doing")
    options = ap.parse_args()
    if options.filterIds:
        options.filterIds = set(options.filterIds.split(','))
    if not options.successOutput:
        options.successOutput = options.output + ".success"
    return options


def compareEdge(edge, shape, edge2, radius):
    shape2 = edge2.getShape()
    off_dists = [(point,) + gh.polygonOffsetAndDistanceToPoint(point, shape2) for point in shape]
    lastOffset = None
    lastPoint = None
    commonLength = 0
    distSum = 0
    distCount = 0
    for point, offset, dist in off_dists:
        if (lastOffset is None or offset >= lastOffset) and dist <= radius:
            if lastOffset is not None:
                commonLength += gh.distance(lastPoint, point)
            lastOffset = offset
            lastPoint = point
            distSum += dist
            distCount += 1
        else:
            firstOffset = off_dists[0][1]
            offset2, dist2 = gh.polygonOffsetAndDistanceToPoint(shape2[-1], shape, True)
            if lastOffset is not None and firstOffset + offset2 >= lastOffset and dist2 <= radius:
                # edge2 ends before reaching the end of shape
                commonLength = offset2
                distSum += dist2
                distCount += 1
            break

    permissionMatch = 1
    for vClass in ["passenger", "bus", "taxi", "tram", "rail", "bicycle", "pedestrian"]:
        if edge.allows(vClass) and not edge2.allows(vClass):
            permissionMatch *= 0.9
    quality = (radius - distSum / distCount) / radius if distCount > 0 else 0
    laneMatch = 1 / (1 + abs(edge.getLaneNumber() - edge2.getLaneNumber()))

    score = commonLength * quality * permissionMatch * laneMatch
    return score, commonLength


def cutOff(shape, commonLength):
    shapelen = gh.polyLength(shape)
    if commonLength + SLACK > shapelen:
        return []
    else:
        index, seen = gh.indexAtShapeOffset(shape, commonLength)
        start = gh.positionAtShapeOffset(shape, commonLength)
        return [start] + shape[index + 1:]


def mapEdge(options, edge):
    success = 0
    results = []  # [(targetEdge, targetFraction, commonLength), ...]
    origShape = edge.getShape()
    shape = [options.remap_xy(xy) for xy in origShape]
    shapelen = gh.polyLength(shape)

    usedEdges = set()
    while shape:
        x2, y2 = shape[0]
        edges2 = options.net2.getNeighboringEdges(x2, y2, options.radius)
        best = None
        bestCommon = 0
        bestScore = 0
        for edge2, _ in edges2:
            if edge2 in usedEdges:
                continue
            score, commonLength = compareEdge(edge, shape, edge2, options.radius)
            if score > bestScore:
                best = edge2
                bestCommon = commonLength
                bestScore = score
        if bestCommon < options.minCommon:
            break
        bestLength = gh.polyLength(best.getShape())
        fraction = min(1.0, bestCommon / bestLength)
        results.append((best, fraction, bestCommon))
        cutFraction = bestCommon / shapelen
        shape = cutOff(shape, bestCommon)
        success += cutFraction
        usedEdges.add(best)
        # print(edge.getID(), best.getID(), commonLength, len(edges2))
    if options.verbose:
        print(edge.getID(), success)
    if success >= 1.01:
        print("implausibly high success %s for edge %s" % (success, edge.getID()), file=sys.stderr)
    success = round(success, 2)
    success = min(success, 1)
    return success, results


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

    successStats = Statistics("completeness")
    with open(options.output, 'w') as fout, open(options.successOutput, 'w') as sout:
        fout.write(';'.join(["origEdge", "targetEdge", "targetFrom", "targetTo", "targetFraction", "common"]) + '\n')
        sout.write(';'.join(["origEdge", "success"]) + '\n')
        for edge in options.net.getEdges():
            if options.filterIds and edge.getID() not in options.filterIds:
                continue
            success, targets = mapEdge(options, edge)
            sout.write(';'.join([edge.getID(), "%.2f" % success]) + '\n')
            successStats.add(success, edge.getID())
            for edge2, fraction, common in targets:
                fout.write(';'.join([
                    edge.getID(), edge2.getID(),
                    edge2.getFromNode().getID(), edge2.getToNode().getID(),
                    "%.2f" % fraction, "%.2f" % common]) + '\n')
    print(successStats)


if __name__ == "__main__":
    main(get_options())
