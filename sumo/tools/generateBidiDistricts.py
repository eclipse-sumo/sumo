#!/usr/bin/env python
"""
@file    generateBidiDistricts.py
@author  Jakob Erdmann
@date    2015-07-31
@version $Id$

Generate a taz (district) file which groups edges in opposite directions
belonging to the same road. For each edge, a taz is created which contains this edge
and its opposite.
This allows routing without the need for an
initial/final turn-around by replacing the attribute names 'from' and 'to' with
'fromTaz' and 'toTaz'

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import sys
import os
import datetime
from optparse import OptionParser

import sumolib
from functools import reduce


def parse_args():
    USAGE = "Usage: " + sys.argv[0] + " <netfile> [options]"
    optParser = OptionParser()
    optParser.add_option("-o", "--outfile", help="name of output file")
    optParser.add_option(
        "-r", "--radius", type=float, default=10., help="radius around the edge")
    optParser.add_option("-t", "--travel-distance", action="store_true",
                         default=False, help="use travel distance in the graph")
    optParser.add_option("--symmetrical", action="store_true",
                         default=False, help="extend the bidi-relationship to be symmetrical")
    options, args = optParser.parse_args()
    try:
        options.net, = args
    except:
        sys.exit(USAGE)
    if options.outfile is None:
        options.outfile = options.net + ".taz.xml"
    return options


def getCandidates(edge, net, radius):
    candidates = []
    r = min(radius, sumolib.geomhelper.polyLength(edge.getShape()) / 2)
    for x, y in edge.getShape():
        nearby = set()
        for edge2, dist in net.getNeighboringEdges(x, y, r):
            nearby.add(edge2)
        candidates.append(nearby)
    return candidates

ASYM_BIDI_CACHE = {}  # edge : opposites


def computeBidiTazAsymByRadius(edge, net, radius):
    if not edge in ASYM_BIDI_CACHE:
        candidates = getCandidates(edge, net, radius)
        opposites = reduce(lambda a, b: a.intersection(b), candidates)
        opposites.update(set(edge.getToNode().getOutgoing()).intersection(
            set(edge.getFromNode().getIncoming())))
        ASYM_BIDI_CACHE[edge] = opposites
    return ASYM_BIDI_CACHE[edge]


def computeAllBidiTaz(net, radius, useTravelDist, symmetrical):
    for edge in net.getEdges():
        if useTravelDist:
            opposites = set()
            queue = [(edge, -1.)]
            while not len(queue) == 0:
                edge2, dist = queue.pop()
                if edge2 not in opposites and dist < radius:
                    opposites.add(edge2)
                    if dist == -1.:
                        dist = 0.
                    else:
                        dist += edge2.getLength()
                    toN = edge2.getToNode()
                    fromN = edge2.getFromNode()
                    for e in toN.getOutgoing() + toN.getIncoming() + fromN.getOutgoing() + fromN.getIncoming():
                        queue.append((e, dist))
        else:
            opposites = computeBidiTazAsymByRadius(edge, net, radius)
            if symmetrical:
                candidates = reduce(
                    lambda a, b: a.union(b), getCandidates(edge, net, radius))
                for cand in candidates:
                    if edge in computeBidiTazAsymByRadius(cand, net, radius):
                        opposites.add(cand)

        yield edge, opposites


def main(netFile, outFile, radius, useTravelDist, symmetrical):
    net = sumolib.net.readNet(netFile, withConnections=False, withFoes=False)
    with open(outFile, 'w') as outf:
        sumolib.writeXMLHeader(
            outf, "$Id$")
        outf.write('<tazs>\n')
        for taz, edges in computeAllBidiTaz(net, radius, useTravelDist, symmetrical):
            outf.write('    <taz id="%s" edges="%s"/>\n' % (
                taz.getID(), ' '.join(sorted([e.getID() for e in edges]))))
        outf.write('</tazs>\n')
    return net

if __name__ == "__main__":
    options = parse_args()
    main(options.net, options.outfile, options.radius,
         options.travel_distance, options.symmetrical)
