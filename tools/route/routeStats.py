#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    routeStats.py
# @author  Jakob Erdmann
# @date    2014-12-18

"""
compute statistics on route lengths for a single route or
for the length-difference between two sets of routes.
Routes must be children of <vehicle> elements and when comparing two sets of
routes, the same vehicle ids must appear.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib  # noqa
from sumolib.miscutils import Statistics, parseTime  # noqa


def get_options():
    USAGE = """Usage %prog [options] <rou.xml> [<rou2.xml>]"""
    optParser = sumolib.options.ArgumentParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("-f", "--fast", action="store_true",
                         default=False, help="Use fast xml parser (does not work with length and numEdges)")
    optParser.add_option("-n", "--network",
                         help="The network file to use with attribute 'length'")
    optParser.add_option("-a", "--attribute", default="length",
                         help="attribute to analyze [length,depart,numEdges,duration,routeLength,speed,speedKmh]")
    optParser.add_option("-b", "--binwidth", type=float,
                         default=500, help="binning width of result histogram")
    optParser.add_option("-l", "--minlength", type=float,
                         default=0, help="only use routes with a minimum route length")
    optParser.add_option("--hist-output", help="output file for histogram (gnuplot compatible)")
    optParser.add_option("--full-output", help="output file for full data dump")
    options, args = optParser.parse_known_args()

    if len(args) not in (1, 2):
        sys.exit(USAGE)

    options.routeFile2 = None
    if len(args) >= 1:
        options.routeFile = args[0]
    if len(args) == 2:
        options.routeFile2 = args[1]

    return options


def main():
    options = get_options()
    net = None
    attribute_retriever = None
    if options.attribute == "length":
        net = sumolib.net.readNet(options.network)

        def attribute_retriever(vehicle):
            return sum([net.getEdge(e).getLength() for e in vehicle.route[0].edges.split()])
    elif options.attribute == "depart":
        def attribute_retriever(vehicle):
            return parseTime(vehicle.depart)
    elif options.attribute == "numEdges":
        def attribute_retriever(vehicle):
            return len(vehicle.route[0].edges.split())
    elif options.attribute == "duration":
        def attribute_retriever(vehicle):
            return parseTime(vehicle.arrival) - parseTime(vehicle.depart)
    elif options.attribute == "routeLength":
        def attribute_retriever(vehicle):
            return float(vehicle.routeLength)
    elif options.attribute == "speed":
        def attribute_retriever(vehicle):
            return float(vehicle.routeLength) / (parseTime(vehicle.arrival) - parseTime(vehicle.depart))
    elif options.attribute == "speedKmh":
        def attribute_retriever(vehicle):
            return 3.6 * float(vehicle.routeLength) / (parseTime(vehicle.arrival) - parseTime(vehicle.depart))
    else:
        sys.exit("Invalid value '%s' for option --attribute" % options.attribute)

    lengths = {}
    lengths2 = {}

    if options.routeFile2 is None:
        # write statistics on a single route file
        stats = Statistics("route %ss" % options.attribute, histogram=True, scale=options.binwidth)

    if options.fast:
        def parse(routes):
            return sumolib.xml.parse_fast(routes, 'vehicle', ['id', 'depart', 'arrival', 'routeLength'])
    else:
        def parse(routes):
            return sumolib.xml.parse(routes, 'vehicle')

    for vehicle in parse(options.routeFile):
        if vehicle.routeLength is None or float(vehicle.routeLength) >= options.minlength:
            length = attribute_retriever(vehicle)
            if options.routeFile2 is None:
                stats.add(length, vehicle.id)
            lengths[vehicle.id] = length

    if options.routeFile2 is not None:
        # compare route lengths between two files
        stats = Statistics("route %s difference" % options.attribute, histogram=True, scale=options.binwidth)
        for vehicle in parse(options.routeFile2):
            if vehicle.routeLength is None or float(vehicle.routeLength) >= options.minlength:
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
