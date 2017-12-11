#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    tracemapper.py
# @author  Michael Behrisch
# @date    2013-10-23
# @version $Id$


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
    optParser.add_option("-p", "--poi-output",
                         help="generate POI output for the trace", metavar="FILE")
    optParser.add_option("--geo", action="store_true",
                         default=False, help="read trace with geo-coordinates")
    (options, args) = optParser.parse_args()

    if not options.output or not options.net:
        optParser.exit("missing input or output")

    if options.verbose:
        print("Reading net ...")
    net = sumolib.net.readNet(options.net, withInternal=True)

    if options.verbose:
        print("Reading traces ...")

    with open(options.output, "w") as outf:
        outf.write('<routes>\n')
        poiOut = None
        if options.poi_output is not None:
            poiOut = open(options.poi_output, "w")
            poiOut.write('<pois>\n')
        with open(options.trace) as traces:
            for line in traces:
                id, traceString = line.split(":")
                trace = [map(float, pos.split(",")) for pos in traceString.split()]
                if options.geo:
                    trace = [net.convertLonLat2XY(*pos) for pos in trace]
                if options.poi_output is not None:
                    for idx, pos in enumerate(trace):
                        poiOut.write('<poi id="%s:%s" x="%s" y="%s"/>\n' % (id, idx, pos[0], pos[1]))
                edges = [e.getID() for e in sumolib.route.mapTrace(trace, net, options.delta, options.verbose) if e.getFunction() != "internal"]
                outf.write('    <route id="%s" edges="%s"/>\n' % (id, " ".join(edges)))
        outf.write('</routes>\n')
        if options.poi_output is not None:
            poiOut.write('</pois>\n')
            poiOut.close()
