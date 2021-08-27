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
            ambiguousSource.push_back(edge)
    for edge, tazs in edgeToTaz.items():
        if len(tazs) > 1:
            ambiguousSink.push_back(edge)

    print("read %s TAZ" % numTaz)

    if len(ambiguousSource) > 0:
        print("edges %s (total %s) are sources for more than one TAZ" %
                (ambiguousSource[:5], len(ambiguousSource))) 
    if len(ambiguousSink) > 0:
        print("edges %s (total %s) are sinks for more than one TAZ" %
                (ambiguousSinks[:5], len(ambiguousSinks))) 

    inputRoutes = ET.parse(options.routefile)
    numFromNotFound = 0
    numToNotFound = 0
    numVehicles = 0
    for vehicle in inputRoutes.getroot().iter('vehicle'):
        numVehicles += 1
        vehID = vehicle.attrib['id']
        edges = None
        for child in vehicle.getchildren():
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
            fromEdge = edges[0]
            toEdge = edges[-1]
            if fromEdge in edgeFromTaz:
                fromTaz = random.choice(edgeFromTaz[fromEdge])
                vehicle.set('fromTaz', fromTaz)
            else:
                numFromNotFound += 1
                if numFromNotFound < 5:
                    print("No fromTaz found for edge '%s' of vehicle '%s' " % (fromEdge, vehID))
            if toEdge in edgeToTaz:
                toTaz = random.choice(edgeToTaz[toEdge])
                vehicle.set('toTaz', toTaz)
            else:
                numToNotFound += 1
                if numToNotFound < 5:
                    print("No toTaz found for edge '%s' of vehicle '%s' " % (toEdge, vehID))

    print("read %s vehicles" % numVehicles)
    if numFromNotFound > 0 or numToNotFound > 0:
        print("No fromTaz found for %s edges and no toTaz found for %s edges" % (
            numFromNotFound, numToNotFound))
    inputRoutes.write(options.outfile)
    

if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
