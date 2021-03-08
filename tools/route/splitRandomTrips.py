#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    addParkingAreaStops2Routes.py
# @author  Evamarie Wiessner
# @date    2017-01-09

"""
add stops at parkingAreas to vehicle routes
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
    optParser.add_option("-r", "--route-file", dest="routefile", help="define the input route file with trips")
    optParser.add_option("-n", "--number", dest="number", help="number of trips to split")
    optParser.add_option("-a", "--output-file-a", dest="outputA", help="define the first output route file with trips")
    optParser.add_option("-b", "--output-file-b", dest="outputB", help="define the second output route file with trips")
    optParser.add_option("--random", action="store_true", default=False, help="use a random seed to initialize the random number generator")
    optParser.add_option("-s", "--seed", type="int", default=42, help="random seed")
    (options, args) = optParser.parse_args(args=args)
    if not options.routefile or not options.number:
        optParser.print_help()
        sys.exit()
    return options


def main(options):
    if not options.random:
        random.seed(options.seed)
    infile = options.routefile
    # check outputs 
    if not options.outputA:
        options.outputA = "tripsA.rou.xml"
    if not options.outputB:
        options.outputB = "tripsB.rou.xml"
    # copy all trips into an array
    tripsArray = []
    for trip in sumolib.xml.parse(infile, "trip"):
        tripsArray.append(trip)
    # declare range [0, numTrips]
    tripsRange = range(0, len(tripsArray))
    # randomSample
    if (int(options.number) < len(tripsArray)):
        randomSample = random.sample(tripsRange, int(options.number))
    else:
        randomSample = tripsRange
    # separate in two groups
    tripsArrayA =[]
    tripsArrayB =[]
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
    with open(options.outputA, 'w') as outf:
        # write header
        outf.write("<?xml version= \"1.0\" encoding=\"UTF-8\"?>\n\n")
        # open route rag
        outf.write("<routes>\n")
        # iterate over trips
        for trip in tripsArrayA:
            # write trip
            outf.write(trip.toXML(initialIndent="    "))
        # close route tag
        outf.write("</routes>\n")
    # write trips B
    with open(options.outputB, 'w') as outf:
        # write header
        outf.write("<?xml version= \"1.0\" encoding=\"UTF-8\"?>\n\n")
        # open route rag
        outf.write("<routes>\n")
        # iterate over trips
        for trip in tripsArrayB:
            # write trip
            outf.write(trip.toXML(initialIndent="    "))
        # close route tag
        outf.write("</routes>\n")


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
