#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateLandmarks.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2021-11-25

"""
Create a landmark table input file for generating a full table duarouter options
--astar.landmark-distances --astar.save-landmark-distances
for a given netfile and with a given number of landmarks
The landmarks are somewhat evenly spaced around the rim of the network
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
from math import pi, degrees

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Generate random boundary landmarks for a given network")
    optParser.add_argument("-n", "--net-file", category="input", dest="netfile", required=True,
                           help="define the net file (mandatory)")
    optParser.add_argument("-o", "--output-file", category="output", dest="outfile",
                           default="landmarks.txt", help="define the output filename")
    optParser.add_argument("--selection-output", category="output", dest="selout",
                           help="Write a selection file for visualization")
    optParser.add_argument("-N", "--number", category="processing", type=int, default=10,
                           help="Define number of landmarks to generate")
    optParser.add_argument("--geo", category="processing", action="store_true", default=False,
                           help="Store landmarks as geo-coordinates instead of edge IDs")
    optParser.add_argument("-d", "--no-deadends", category="processing", action="store_true", default=False,
                           dest="noDeadends",
                           help="Only use edges that have at least one follower and one successor")
    optParser.add_argument("-p", "--min-priority", category="processing", type=int, default=-10, dest="minPrio",
                           help="Only use edges with priority of at least INT")
    optParser.add_argument("-s", "--seed", category="processing", type=int, default=42, help="random seed")
    optParser.add_argument("--random", category="processing", action="store_true", default=False,
                           help="use a random seed to initialize the random number generator")
    optParser.add_argument("--vclass", category="processing", default="passenger",
                           help="only use edges which permit the given vehicle class")
    optParser.add_argument("-v", "--verbose", category="processing", action="store_true",
                           default=False, help="tell me what you are doing")
    return optParser.parse_args(args=args)


def filterEdges(options, edges):
    return [e for e in edges if (
        e.getPriority() >= options.minPrio
        and e.allows(options.vclass)
        and (not options.noDeadends or (e.getAllowedOutgoing(options.vclass) and e.getIncoming())))]


def getAngles(net, edges):
    """compute angle towards network center and sort edges by this angle"""
    xmin, ymin, xmax, ymax = net.getBoundary()
    center = ((xmin + xmax) / 2, (ymin + ymax) / 2)
    result = []
    for e in edges:
        pos = e.getToNode().getCoord()
        angle = sumolib.geomhelper.angleTo2D(center, pos)
        dist = sumolib.geomhelper.distance(center, pos)
        result.append((angle, dist, e))
    result.sort(key=lambda x: x[0:2])
    return result


def main(options):
    if not options.random:
        random.seed(options.seed)
    net = sumolib.net.readNet(options.netfile)
    if options.geo and not net.hasGeoProj():
        print("Network does not provide geo-projection. Option --geo cannot be used", file=sys.stderr)
        sys.exit(1)

    edges = filterEdges(options, net.getEdges())
    edgeAngles = getAngles(net, edges)

    angleSteps = [-pi + (i + 1) * 2 * pi / options.number for i in range(options.number)]
    i = 0
    landmarks = []
    aPrev = -pi
    for a in angleSteps:
        edgeDistances = []
        while i < len(edgeAngles) and edgeAngles[i][0] < a:
            edgeDistances.append(edgeAngles[i][1:])
            i += 1
        if edgeDistances:
            edgeDistances.sort(key=lambda x: x[0])
            landmarks.append(edgeDistances[-1][1])
        else:
            print("Found no edges between angles %.0f and %0.f" % (degrees(aPrev), degrees(a)), file=sys.stderr)
        aPrev = a

    with open(options.outfile, 'w') as outf:
        for e in landmarks:
            if options.geo:
                x, y = sumolib.geomhelper.positionAtShapeOffset(e.getShape(), e.getLength() / 2)
                outf.write("%f %f\n" % net.convertXY2LonLat(x, y))
            else:
                outf.write("%s\n" % e.getID())

    if options.selout:
        with open(options.selout, 'w') as outf:
            for e in landmarks:
                outf.write("edge:%s\n" % e.getID())


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
