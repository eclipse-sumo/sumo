#!/usr/bin/env python
"""
@file    routeStats.py
@author  Jakob Erdmann
@date    2014-12-18
@version $Id$

compute statistics on route lengths

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys
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
    USAGE = """Usage %prog [options] <net.xml> <rou.xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("--binwidth", type="float",
            default=500, help="binning width of route length histogram")
    optParser.add_option("--hist-output", type="string",
            default=None, help="output file for histogram (gnuplot compatible)")
    options, args = optParser.parse_args()
    if len(args) != 2:
        sys.exit(USAGE)
    options.network = args[0]
    options.routeFile = args[1]
    return options


def main():
    options = get_options()
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])

    lengthStats = Statistics("route lengths", histogram=True, scale=options.binwidth) 
    for vehicle in parse(options.routeFile, 'vehicle'):
        length = sum([net.getEdge(e).getLength() for e in vehicle.route[0].edges.split()])
        lengthStats.add(length, vehicle.id)

    print lengthStats
    
    if options.hist_output is not None:
        with open(options.hist_output, 'w') as f:
            for bin, count in lengthStats.histogram():
                f.write("%s %s\n" % (bin, count))

if __name__ == "__main__":
    main()
