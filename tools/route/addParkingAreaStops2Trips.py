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

# @file    addParkingAreaStops2Trips.py
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
    optParser.add_option("-r", "--route-file", dest="routefile",
                         help="define the input route file with trips")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         help="output route file with trips with parking stops")
    optParser.add_option("-p", "--parking-areas", dest="parking",
                         help="define the parking areas seperated by comma")
    optParser.add_option("-d", "--parking-duration", dest="duration",
                         help="define the parking duration (in seconds)", default=3600)
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true", default=False,
                         help="tell me what you are doing")
    optParser.add_option("--random", action="store_true", default=False,
                         help="use a random seed to initialize the random number generator")
    optParser.add_option("-s", "--seed", type="int", default=42,
                         help="random seed")
    (options, args) = optParser.parse_args(args=args)
    if not options.routefile or not options.parking:
        optParser.print_help()
        sys.exit()
    return options


def main(options):
    if not options.random:
        random.seed(options.seed)
    infile = options.routefile
    # set default output file
    if not options.outfile:
        options.outfile = infile.replace(".xml", ".parking.xml")
    # declare a list with parkings
    parkings = []
    # save all parkings in a list
    for parking in sumolib.xml.parse(options.parking, "parkingArea"):
        parkings.append(parking)
    # open file
    with open(options.outfile, 'w') as outf:
        # write header
        outf.write("<?xml version= \"1.0\" encoding=\"UTF-8\"?>\n\n")
        # open route rag
        outf.write("<routes>\n")
        # iterate over trips
        for trip in sumolib.xml.parse(infile, "trip"):
            # obtain random parking
            random_parking = random.choice(parkings)
            trip.addChild("stop", {"parkingArea": random_parking.id, "duration": int(options.duration)})
            # write trip
            outf.write(trip.toXML(initialIndent="    "))
        # close route tag
        outf.write("</routes>\n")


if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
