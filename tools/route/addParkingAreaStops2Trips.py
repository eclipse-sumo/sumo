#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
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
from random import Random

# (seed)

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("-r", "--route-file", category='input', dest="routefile", required=True,
                         help="define the input route file with trips")
    optParser.add_option("-o", "--output-file", category='output', dest="outfile",
                         help="output route file with trips with parking stops")
    optParser.add_option("-p", "--parking-areas", category='input', dest="parking", required=True,
                         help="define the parking areas separated by comma")
    optParser.add_option("-d", "--parking-duration", dest="duration",
                         help="define the parking duration (in seconds)", default=3600)
    optParser.add_option("-u", "--parking-until", dest="until",
                         help="define the parking until duration (in seconds)")
    optParser.add_option("-l", "--parking-untilend", dest="untilend",
                         help="define the parking until end variable duration (in seconds)")
    optParser.add_option("-b", "--parking-duration-begin", dest="durationBegin",
                         help="define the minimum parking duration (in seconds)")
    optParser.add_option("-e", "--parking-duration-end", dest="durationEnd",
                         help="define the maximum parking duration (in seconds)")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true", default=False,
                         help="tell me what you are doing")
    optParser.add_option("--random", action="store_true", default=False,
                         help="use a random seed to initialize the random number generator")
    optParser.add_option("-s", "--seed", type=int, default=42,
                         help="random seed")
    return optParser.parse_args(args=args)


def main(options):
    R1 = Random()
    R2 = Random()
    if not options.random:
        R1.seed(options.seed)
        R2.seed(options.seed)
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
        for trip in sumolib.xml.parse(infile, "trip", heterogeneous=True):
            # obtain random parking
            random_parking = R1.choice(parkings)
            # add child depending of durations
            if (options.durationBegin and options.durationEnd):
                # obtain random duration
                duration = R2.randint(int(options.durationBegin), int(options.durationEnd))
                trip.addChild("stop", {"parkingArea": random_parking.id, "duration": duration})
            elif options.until:
                if options.untilend:
                    # obtain random duration
                    until = R2.randint(int(options.until), int(options.untilend))
                    trip.addChild("stop", {"parkingArea": random_parking.id, "until": until})
                else:
                    trip.addChild("stop", {"parkingArea": random_parking.id, "until": options.until})
            else:
                trip.addChild("stop", {"parkingArea": random_parking.id, "duration": int(options.duration)})
            # write trip
            outf.write(trip.toXML(initialIndent="    "))
        # close route tag
        outf.write("</routes>\n")


if __name__ == "__main__":
    options = get_options()
    main(options)
