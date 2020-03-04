#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    computeCoordination.py
# @author  Daniel Wesemeyer
# @author  Jakob Erdmann
# @date    2018-08-18

"""
This script analyses fcd output to compute the coordination factor for a given cooridor
The coordination factor is defined as the fraction of vehicles that passed the
corridor without stopping to the total number of vehicles on the corridor
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from collections import defaultdict
import math  # noqa

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.xml import parse_fast  # noqa


def getOptions(args=None):

    usage = "generateTLSE2Detectors.py -n example.net.xml -l 250 -d .1 -f 60"
    argParser = sumolib.options.ArgumentParser(usage=usage)
    argParser.add_argument("-f", "--fcd-file", dest="fcdfile",
                           help="Input fcd file (mandatory)")
    argParser.add_argument("-m", "--min-speed", dest="minspeed", type=float, default=5,
                           help="Minimum speed to consider vehicle undelayed")
    argParser.add_argument("--filter-route", dest="filterRoute",
                           help="only consider vehicles that pass the given list of edges (regardless of gaps)")

    options = argParser.parse_args()

    if options.fcdfile is None:
        sys.exit("mandatory argument FCD_FILE missing")

    if options.filterRoute is not None:
        options.filterRoute = options.filterRoute.split(',')
    else:
        options.filterRoute = []
    return options


def main(options):

    routes = defaultdict(list)  # vehID -> recorded edges
    minSpeed = defaultdict(lambda: 1000)
    active = set()  # vehicles that have passed the first filterRoute edge
    for vehicle in parse_fast(options.fcdfile, 'vehicle', ['id', 'speed', 'lane']):
        vehID = vehicle.id
        edge = vehicle.lane[0:vehicle.lane.rfind('_')]
        if len(routes[vehID]) == 0 or routes[vehID][-1] != edge:
            routes[vehID].append(edge)
        if options.filterRoute and vehID not in active:
            if edge in options.filterRoute:
                active.add(vehID)
            else:
                continue
        minSpeed[vehID] = min(minSpeed[vehID], float(vehicle.speed))

    n = 0
    delayed = 0

    for vehID, route in routes.items():
        skip = False
        for required in options.filterRoute:
            if required not in route:
                skip = True
                break
        if not skip:
            n += 1
            if minSpeed[vehID] < options.minspeed:
                delayed += 1

    print("n=%s d=%s coordinationFactor=%s" % (n, delayed, (n - delayed) / float(n)))


if __name__ == "__main__":
    main(getOptions())
