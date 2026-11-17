#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateBidiDistricts.py
# @author  Jakob Erdmann
# @date    2015-07-31

"""
Generate a taz (district) file which groups edges in opposite directions
belonging to the same road. For each edge, a taz is created which contains this edge
and its opposite.
This allows routing without the need for an
initial/final turn-around by replacing the attribute names 'from' and 'to' with
'fromTaz' and 'toTaz'
"""
from __future__ import absolute_import
from functools import reduce
import heapq
import math

import sumolib  # noqa


def parse_args():
    arg_parser = sumolib.options.ArgumentParser()
    arg_parser.add_argument("-o", "--outfile", category="output", help="name of output file")
    arg_parser.add_argument("-r", "--radius", category="processing", type=float, default=10.,
                            help="maximum air distance around the edge")
    arg_parser.add_argument("-t", "--travel-distance", category="processing", type=float,
                            help="maximum travel distance in the graph")
    arg_parser.add_argument("--symmetrical", action="store_true", default=False,
                            help="extend the bidi-relationship to be symmetrical")
    arg_parser.add_argument("net", category="input", help="SUMO network file")
    options = arg_parser.parse_args()
    if options.outfile is None:
        options.outfile = options.net + ".taz.xml"
    return options


def sampleShape(shape, maxStep):
    if maxStep is None or maxStep <= 0 or len(shape) < 2:
        return list(shape)
    pts = [shape[0]]
    for (x1, y1), (x2, y2) in zip(shape[:-1], shape[1:]):
        dist = math.hypot(x2 - x1, y2 - y1)
        steps = max(1, int(math.ceil(dist / maxStep)))
        for k in range(1, steps + 1):
            frac = float(k) / steps
            pts.append((x1 + (x2 - x1) * frac, y1 + (y2 - y1) * frac))
    return pts


def getCandidates(edge, net, radius):
    candidates = []
    for x, y in sampleShape(edge.getShape(), radius):
        nearby = set()
        for edge2, dist in net.getNeighboringEdges(x, y, radius):
            nearby.add(edge2)
        candidates.append(nearby)
    return candidates


ASYM_BIDI_CACHE = {}  # edge : opposites


def computeBidiTazAsymByRadius(edge, net, radius):
    if edge not in ASYM_BIDI_CACHE:
        candidates = getCandidates(edge, net, radius)
        opposites = reduce(lambda a, b: a.intersection(b), candidates)
        reverse_edges = set(edge.getToNode().getOutgoing()).intersection(
            set(edge.getFromNode().getIncoming()))
        max_rev_dist = max(radius, 20.0)
        sampled = sampleShape(edge.getShape(), max_rev_dist)
        for rev in reverse_edges:
            rev_shape = rev.getShape()
            if all(sumolib.geomhelper.distancePointToPolygon(pt, rev_shape) <= max_rev_dist for pt in sampled):
                opposites.add(rev)
        ASYM_BIDI_CACHE[edge] = opposites
    return ASYM_BIDI_CACHE[edge]


def computeAllBidiTaz(net, radius, travelDist, symmetrical):
    ASYM_BIDI_CACHE.clear()
    for edge in net.getEdges():
        travelOpposites = set()
        if travelDist is not None:
            bestDist = {edge: -1.}
            heap = [(-1., edge.getID(), edge)]
            while heap:
                dist, _, edge2 = heapq.heappop(heap)
                if dist > bestDist.get(edge2, float('inf')):
                    continue
                if dist < travelDist:
                    travelOpposites.add(edge2)
                    nextDist = 0. if dist == -1. else dist + edge2.getLength()
                    if nextDist < travelDist:
                        toN = edge2.getToNode()
                        fromN = edge2.getFromNode()
                        for e in toN.getOutgoing() + toN.getIncoming() + fromN.getOutgoing() + fromN.getIncoming():
                            if nextDist < bestDist.get(e, float('inf')):
                                bestDist[e] = nextDist
                                heapq.heappush(heap, (nextDist, e.getID(), e))
        if radius is not None and radius > 0.:
            opposites = computeBidiTazAsymByRadius(edge, net, radius)
            if symmetrical:
                candidates = reduce(
                    lambda a, b: a.union(b), getCandidates(edge, net, radius))
                for cand in candidates:
                    if edge in computeBidiTazAsymByRadius(cand, net, radius):
                        opposites.add(cand)
            travelOpposites.update(opposites)

        yield edge, travelOpposites


def main(netFile, outFile, radius, travelDist, symmetrical):
    net = sumolib.net.readNet(netFile, withConnections=False, withFoes=False)
    with sumolib.openz(outFile, mode='w') as outf:
        sumolib.writeXMLHeader(outf)
        outf.write(u'<tazs>\n')
        for taz, edges in computeAllBidiTaz(net, radius, travelDist, symmetrical):
            outf.write(u'    <taz id="%s" edges="%s"/>\n' % (
                taz.getID(), ' '.join(sorted([e.getID() for e in edges]))))
        outf.write(u'</tazs>\n')
    return net


if __name__ == "__main__":
    opts = parse_args()
    main(opts.net, opts.outfile, opts.radius, opts.travel_distance, opts.symmetrical)
