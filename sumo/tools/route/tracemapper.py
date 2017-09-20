#!/usr/bin/env python
"""
@file    tracemapper.py
@author  Michael Behrisch
@date    2013-10-23
@version $Id$

This script tries to match a list of positions to a route in a sumo network.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from optparse import OptionParser
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

import sumolib  # noqa

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
    optParser.add_option("--geo", action="store_true",
                         default=False, help="read trace with geo-coordinates")
    (options, args) = optParser.parse_args()

    if not options.output or not options.net:
        optParser.exit("missing input or output")

    if options.verbose:
        print("Reading net ...")
    net = sumolib.net.readNet(options.net)

    if options.verbose:
        print("Reading traces ...")

    with open(options.output, "w") as outf:
        outf.write('<routes>\n')
        with open(options.trace) as traces:
            for line in traces:
                id, traceString = line.split(":")
                trace = [map(float, pos.split(",")) for pos in traceString.split()]
                if options.geo:
                    trace = [net.convertLonLat2XY(*pos) for pos in trace]
                edges = sumolib.route.mapTrace(trace, net, options.delta, options.verbose)
                outf.write('    <route id="%s" edges="%s">\n' % (id, " ".join(edges)))
        outf.write('</routes>\n')
