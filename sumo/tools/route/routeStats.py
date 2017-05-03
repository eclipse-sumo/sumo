#!/usr/bin/env python
"""
@file    routeStats.py
@author  Jakob Erdmann
@date    2014-12-18
@version $Id$

compute statistics on route lengths for a single route or
for the lenght-difference between two sets of routes.
Routes must be children of <vehicle> elements and when comparing two sets of
routes, the same vehicle ids must appear.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

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
    USAGE = """Usage %prog [options] <net.xml> <rou.xml> [<rou2.xml>]"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--attribute", type="string",
                         default="length", help="attribute to analyze [length,depart,numEdges]")
    optParser.add_option("--binwidth", type="float",
                         default=500, help="binning width of result histogram")
    optParser.add_option("--hist-output", type="string",
                         default=None, help="output file for histogram (gnuplot compatible)")
    optParser.add_option("--full-output", type="string",
                         default=None, help="output file for full data dump")
    options, args = optParser.parse_args()

    if len(args) not in (2, 3):
        sys.exit(USAGE)

    options.routeFile2 = None
    if len(args) >= 2:
        options.network = args[0]
        options.routeFile = args[1]
    if len(args) == 3:
        options.routeFile2 = args[2]

    return options


def main():
    options = get_options()
    net = None
    attribute_retriever = None
    if options.attribute == "length":
        net = readNet(options.network)
        attribute_retriever = lambda vehicle: sum([net.getEdge(e).getLength() for e in vehicle.route[0].edges.split()])
    elif options.attribute == "depart":
        attribute_retriever = lambda vehicle: float(vehicle.depart)
    elif options.attribute == "numEdges":
        attribute_retriever = lambda vehicle: len(vehicle.route[0].edges.split())
    else:
        sys.exit("Invalid value '%s' for option --attribute" % options.attribute)

    lengths = {}
    lengths2 = {}

    if options.routeFile2 is None:
        # write statistics on a single route file
        stats = Statistics(
            "route %ss" % options.attribute, histogram=True, scale=options.binwidth)

    for vehicle in parse(options.routeFile, 'vehicle'):
        length = attribute_retriever(vehicle)
        if options.routeFile2 is None:
            stats.add(length, vehicle.id)
        lengths[vehicle.id] = length

    if options.routeFile2 is not None:
        # compare route lengths between two files
        stats = Statistics(
            "route %s difference" % options.attribute, histogram=True, scale=options.binwidth)
        for vehicle in parse(options.routeFile2, 'vehicle'):
            lengths2[vehicle.id] = attribute_retriever(vehicle)
            stats.add(lengths2[vehicle.id] - lengths[vehicle.id], vehicle.id)
    print(stats)

    if options.hist_output is not None:
        with open(options.hist_output, 'w') as f:
            for bin, count in stats.histogram():
                f.write("%s %s\n" % (bin, count))

    if options.full_output is not None:
        with open(options.full_output, 'w') as f:
            if options.routeFile2 is None:
                data = [(v, k) for k, v in lengths.items()]
            else:
                data = [(lengths2[id] - lengths[id], id)
                        for id in lengths.keys()]
            for val, id in sorted(data):
                f.write("%s %s\n" % (val, id))

if __name__ == "__main__":
    main()
