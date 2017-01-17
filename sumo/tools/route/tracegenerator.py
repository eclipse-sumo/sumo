#!/usr/bin/env python
"""
@file    tracemapper.py
@author  Michael Behrisch
@date    2013-10-23
@version $Id$

This script tries to generate traces for routes in a sumo network.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

import sumolib


def generateTrace(route, step):
    trace = []
    for edge in route:
        numSteps = int(edge.getLength() / step)
        for p in range(numSteps):
            trace.append(
                sumolib.geomhelper.positionAtShapeOffset(edge.getShape(), p * step))
    return trace


if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-n", "--net",
                         help="SUMO network to use (mandatory)", metavar="FILE")
    optParser.add_option("-2", "--net2",
                         help="immediately match routes to a second network", metavar="FILE")
    optParser.add_option("-r", "--routes",
                         help="route file to use (mandatory)", metavar="FILE")
    optParser.add_option("-s", "--step", default="10",
                         type="float", help="distance between successive trace points")
    optParser.add_option("-d", "--delta", default="1",
                         type="float", help="maximum distance between edge and trace points when matching to the second net")
    optParser.add_option("-o", "--output",
                         help="trace or route output (mandatory)", metavar="FILE")
    (options, args) = optParser.parse_args()

    if not options.output or not options.net or not options.routes:
        optParser.exit("missing input or output")

    if options.verbose:
        print("Reading net ...")
    net = sumolib.net.readNet(options.net)
    net2 = None
    if options.net2:
        net.move(-net.getLocationOffset()[0], -net.getLocationOffset()[1])
        net2 = sumolib.net.readNet(options.net2)
        net2.move(-net2.getLocationOffset()[0], -net2.getLocationOffset()[1])

    if options.verbose:
        print("Reading routes ...")

    f = open(options.output, "w")
    for route in sumolib.output.parse_fast(options.routes, "vehicle", ["id", "edges"]):
        edges = [net.getEdge(e) for e in route.edges.split()]
        trace = generateTrace(edges, options.step)
        if net2:
            path = sumolib.route.mapTrace(trace, net2, options.delta)
            if not path or path == ["*"]:
                print("No match for", route.id)
            print(route.id, path, file=f)
        else:
            print("%s:%s" %
                  (route.id, " ".join(["%s,%s" % p for p in trace])), file=f)
    f.close()
