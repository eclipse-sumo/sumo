#!/usr/bin/env python
"""
@file    tracemapper.py
@author  Michael Behrisch
@date    2013-10-23
@version $Id$

This script tries to match a list of positions to a route in a sumo network.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, sys
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

import sumolib

def getMinPath(paths):
    minDist = 1e400
    minPath = None
    for path, dist in paths.iteritems():
        if dist < minDist:
            minPath = path
            minDist = dist
    return minPath

def mapTrace(trace, net, delta):
    result = []
    paths = {}
    for pos in trace:
        newPaths = {}
        for edge in net.getNeighboringEdges(pos[0], pos[1], delta):
            d = sumolib.geomhelper.distancePointToPolygon(pos, edge.getShape())
            if d < delta:
                if paths:
                    minDist = 1e400
                    minPath = None
                    for path, dist in paths.iteritems():
                        if dist < minDist:
                            if edge == path[-1]:
                                minPath = path
                                minDist = dist
                            elif edge in path[-1].getOutgoing():
                                minPath = path + (edge,)
                                minDist = dist
                    if minPath:
                        newPaths[minPath] = minDist + d
                else:
                    newPaths[(edge,)] = d
        if not newPaths:
            if paths:
                result += [e.getID() for e in getMinPath(paths)]
                result.append("*")
            if not result:
                result.append("*")
        paths = newPaths
    if paths:
        return result + [e.getID() for e in getMinPath(paths)]
    return result

if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-n", "--net",
                         help="SUMO network to use (mandatory)", metavar="FILE")
    optParser.add_option("-t", "--trace",
                         help="trace file to use (mandatory)", metavar="FILE")
    optParser.add_option("-d", "--delta", default="1",
                         type="float", help="maximum distance between edge and trace points")
    optParser.add_option("-o", "--output",
                         help="route output (mandatory)", metavar="FILE")
    (options, args) = optParser.parse_args()


    if not options.output or not options.net:
        optParser.exit("missing input or output")

    if options.verbose:
        print ("Reading net ...")
    net = sumolib.net.readNet(options.net)

    if options.verbose:
        print ("Reading traces ...")

    f = open(options.output, "w")
    with open(options.trace) as traces:
        for line in traces:
            id, traceString = line.split(":")
            trace = [map(float, pos.split(",")) for pos in traceString.split()]
            print(mapTrace(trace, net, options.delta), file=f)
    f.close()
