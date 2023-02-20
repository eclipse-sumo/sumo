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

# @file    addTAZ.py
# @author  Jakob Erdmann
# @date    2021-08-27

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
from collections import defaultdict
import xml.etree.ElementTree as ET

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Add fromTaz and toTaz to a route file")
    optParser.add_argument("-r", "--route-file", dest="routefile",
                           help="define the input route file (mandatory)")
    optParser.add_argument("-a", "-taz-files", dest="tazfiles",
                           help="define the files to load TAZ (districts) from (mandatory)")
    optParser.add_argument("-o", "--output-file", dest="outfile",
                           help="define the output filename (mandatory)")
    optParser.add_argument("-s", "--seed", type=int, default=42, help="random seed")

    options = optParser.parse_args(args=args)
    if not options.routefile or not options.tazfiles or not options.outfile:
        optParser.print_help()
        sys.exit(1)

    options.tazfiles = options.tazfiles.split()
    return options


def main(options):
    random.seed(options.seed)
    edgeFromTaz = defaultdict(list)
    edgeToTaz = defaultdict(list)
    numTaz = 0
    for tazfile in options.tazfiles:
        for taz in sumolib.xml.parse(tazfile, 'taz'):
            numTaz += 1
            if taz.edges:
                for edge in taz.edges.split():
                    edgeFromTaz[edge].append(taz.id)
                    edgeToTaz[edge].append(taz.id)
            if taz.tazSource:
                for source in taz.tazSource:
                    edgeFromTaz[source.id].append(taz.id)
            if taz.tazSink:
                for sink in taz.tazSource:
                    edgeToTaz[source.id].append(taz.id)

    ambiguousSource = []
    ambiguousSink = []
    for edge, tazs in edgeFromTaz.items():
        if len(tazs) > 1:
            ambiguousSource.append(edge)
    for edge, tazs in edgeToTaz.items():
        if len(tazs) > 1:
            ambiguousSink.append(edge)

    print("read %s TAZ" % numTaz)

    if len(ambiguousSource) > 0:
        print("edges %s (total %s) are sources for more than one TAZ" %
              (ambiguousSource[:5], len(ambiguousSource)))
    if len(ambiguousSink) > 0:
        print("edges %s (total %s) are sinks for more than one TAZ" %
              (ambiguousSink[:5], len(ambiguousSink)))

    inputRoutes = ET.parse(options.routefile)

    class nl:  # nonlocal integral variables
        numFromNotFound = 0
        numToNotFound = 0
        numVehicles = 0

    def addAttrs(vehicle, fromEdge, toEdge):
        vehID = vehicle.attrib['id']
        if fromEdge in edgeFromTaz:
            fromTaz = random.choice(edgeFromTaz[fromEdge])
            vehicle.set('fromTaz', fromTaz)
        else:
            nl.numFromNotFound += 1
            if nl.numFromNotFound < 5:
                print("No fromTaz found for edge '%s' of vehicle '%s' " % (fromEdge, vehID))
        if toEdge in edgeToTaz:
            toTaz = random.choice(edgeToTaz[toEdge])
            vehicle.set('toTaz', toTaz)
        else:
            nl.numToNotFound += 1
            if nl.numToNotFound < 5:
                print("No toTaz found for edge '%s' of vehicle '%s' " % (toEdge, vehID))

    for vehicle in inputRoutes.getroot().iter('vehicle'):
        nl.numVehicles += 1
        vehID = vehicle.attrib['id']
        edges = None
        for child in vehicle:
            if child.tag == "route":
                edges = child.attrib['edges']
                break
            elif child.tag == "routeDistribution":
                for child2 in child.getchildren():
                    if child2.tag == "route":
                        edges = child2.attrib['edges']
                        break
                break

        if edges is None:
            print("No edges found for vehicle '%s'" % vehID)
        else:
            edges = edges.split()
            addAttrs(vehicle, edges[0], edges[-1])

    for trip in inputRoutes.getroot().iter('trip'):
        addAttrs(trip, trip.attrib['from'], trip.attrib['to'])

    print("read %s vehicles" % nl.numVehicles)
    if nl.numFromNotFound > 0 or nl.numToNotFound > 0:
        print("No fromTaz found for %s edges and no toTaz found for %s edges" % (
            nl.numFromNotFound, nl.numToNotFound))
    inputRoutes.write(options.outfile)


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
