#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    splitRandom.py
# @author  Pablo Alvarez Lopez
# @date    2021-03-09

"""
splits a route file randomly in two parts
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import optparse
import random

# (seed)

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-r", "--route-file", dest="routefile",
                         help="define the input route file with trips or vehicles")
    optParser.add_option("-n", "--number", dest="number",
                         help="number of trips/vehicles to split (not together with percent)")
    optParser.add_option("-p", "--percent", dest="percent",
                         help="percent of trips/vehicles to split (not together with number")
    optParser.add_option("-a", "--output-file-a", dest="outputA", default="tripsA.rou.xml",
                         help="define the first output route file")
    optParser.add_option("-b", "--output-file-b", dest="outputB", default="tripsB.rou.xml",
                         help="define the second output route file")
    optParser.add_option("--random", action="store_true", default=False,
                         help="use a random seed to initialize the random number generator")
    optParser.add_option("-s", "--seed", type="int", default=42,
                         help="random seed")
    (options, args) = optParser.parse_args(args=args)
    if not options.routefile or not (options.number or options.percent) or (options.number and options.percent):
        optParser.print_help()
        sys.exit()
    return options


def main(options):
    if not options.random:
        random.seed(options.seed)
    infile = options.routefile
    # copy all trips into an array
    tripsArray = list(sumolib.xml.parse(infile, ["trip", "vehicle", "flow"]))
    # declare range [0, numTrips]
    tripsRange = range(0, len(tripsArray))
    # declare num of vehicles
    numVehicles = 0
    # set num vehicles depending of options or number
    if (options.number):
        numVehicles = int(options.number)
    elif (options.percent):
        numVehicles = int(len(tripsArray) * int(options.percent) / 100.0)
    # randomSample
    if (numVehicles < len(tripsArray)):
        randomSample = random.sample(tripsRange, numVehicles)
    else:
        randomSample = tripsRange
    # separate in two groups
    tripsArrayA = []
    tripsArrayB = []
    # declare index
    index = 0
    # iterate over randomSample
    for trip in tripsArray:
        # if index is in randomSample, append in B, if not, append in A
        if index in randomSample:
            tripsArrayB.append(trip)
        else:
            tripsArrayA.append(trip)
        # update index
        index += 1
    # write trips A
    for fname, trips in [
        (options.outputA, tripsArrayA),
            (options.outputB, tripsArrayB)]:
        with open(fname, 'w') as outf:
            # write header
            sumolib.writeXMLHeader(outf, "$Id$", "routes")  # noqa
            # iterate over trips
            for trip in trips:
                # write trip
                outf.write(trip.toXML(initialIndent="    "))
            # close route tag
            outf.write("</routes>\n")


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
