#!/usr/bin/env python
"""
@file    routeStats.py
@author  Jakob Erdmann
@date    2014-12-18
@version $Id$

compute statistics for two sets of routes (for the same set of vehicles)

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from optparse import OptionParser

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.output import parse, parse_fast
    from sumolib.net import readNet
    from sumolib.miscutils import Statistics
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    USAGE = """Usage %prog [options] <net.xml> <rou1.xml> <rou2.xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--binwidth", type="float",
                         default=100, help="binning width of route length difference histogram")
    optParser.add_option("--hist-output", type="string",
                         default=None, help="output file for histogram (gnuplot compatible)")
    optParser.add_option("--full-output", type="string",
                         default=None, help="output file for full data dump")
    options, args = optParser.parse_args()
    try:
        options.network = args[0]
        options.routeFile1 = args[1]
        options.routeFile2 = args[2]
    except:
        sys.exit(USAGE)
    return options


def getRouteLength(net, vehicle):
    return sum([net.getEdge(e).getLength() for e in vehicle.route[0].edges.split()])


def main():
    options = get_options()
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])

    lengths1 = {}
    lengths2 = {}
    lengthDiffStats = Statistics(
        "route length difference", histogram=True, scale=options.binwidth)
    for vehicle in parse(options.routeFile1, 'vehicle'):
        lengths1[vehicle.id] = getRouteLength(net, vehicle)
    for vehicle in parse(options.routeFile2, 'vehicle'):
        lengths2[vehicle.id] = getRouteLength(net, vehicle)
        lengthDiffStats.add(
            lengths2[vehicle.id] - lengths1[vehicle.id], vehicle.id)

    print(lengthDiffStats)

    if options.hist_output is not None:
        with open(options.hist_output, 'w') as f:
            for bin, count in lengthDiffStats.histogram():
                f.write("%s %s\n" % (bin, count))

    if options.full_output is not None:
        with open(options.full_output, 'w') as f:
            differences = sorted(
                [(lengths2[id] - lengths1[id], id) for id in lengths1.keys()])
            for diff, id in differences:
                f.write("%s %s\n" % (diff, id))

if __name__ == "__main__":
    main()
