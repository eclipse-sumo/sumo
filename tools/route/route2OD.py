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

# @file    route2OD.py
# @author  Jakob Erdmann
# @date    2021-09-09

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import random
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Add fromTaz and toTaz to a route file")
    optParser.add_argument("-r", "--route-file", dest="routefile",
                           help="define the input route file (mandatory)")
    optParser.add_argument("-a", "-taz-files", dest="tazfiles",
                           help="define the files to load TAZ (districts) from (mandatory)")
    optParser.add_argument("-o", "--output-file", dest="outfile",
                           help="define the output filename (mandatory)")
    optParser.add_argument("-i", "--interval",
                           help="define the output aggregation interval")
    optParser.add_argument("--id", default="DEFAULT_VEHTYPE", dest="intervalID",
                           help="define the output aggregation interval")
    optParser.add_argument("-s", "--seed", type=int, default=42, help="random seed")

    options = optParser.parse_args(args=args)
    if not options.routefile or not options.tazfiles or not options.outfile:
        optParser.print_help()
        sys.exit(1)

    if options.interval is not None:
        options.interval = parseTime(options.interval)

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
              (ambiguousSink[:5], len(ambiguousSink)))

    class nl:  # nonlocal integral variables
        numFromNotFound = 0
        numToNotFound = 0
        numVehicles = 0
        end = 0

    # begin -> od -> count
    intervals = defaultdict(lambda: defaultdict(lambda: 0))

    def addVehicle(vehID, fromEdge, toEdge, time, count=1):
        nl.numVehicles += count
        fromTaz = None
        toTaz = None
        if fromEdge in edgeFromTaz:
            fromTaz = random.choice(edgeFromTaz[fromEdge])
        else:
            nl.numFromNotFound += 1
            if nl.numFromNotFound < 5:
                print("No fromTaz found for edge '%s' of vehicle '%s' " % (fromEdge, vehID))
        if toEdge in edgeToTaz:
            toTaz = random.choice(edgeToTaz[toEdge])
        else:
            nl.numToNotFound += 1
            if nl.numToNotFound < 5:
                print("No toTaz found for edge '%s' of vehicle '%s' " % (toEdge, vehID))
        if fromTaz and toTaz:
            if options.interval is None:
                intervalBegin = 0
            else:
                intervalBegin = int(time / options.interval) * options.interval
            intervals[intervalBegin][(fromTaz, toTaz)] += count
        nl.end = max(nl.end, time)

    for vehicle in sumolib.xml.parse(options.routefile, ['vehicle']):
        if vehicle.route and type(vehicle.route) == list:
            edges = vehicle.route[0].edges.split()
            addVehicle(vehicle.id, edges[0], edges[-1], parseTime(vehicle.depart))
        else:
            print("No edges found for vehicle '%s'" % vehicle.id)

    for trip in sumolib.xml.parse(options.routefile, ['trip']):
        addVehicle(trip.id, trip.attr_from, trip.to, parseTime(trip.depart))

    for flow in sumolib.xml.parse(options.routefile, ['flow']):
        count = None
        if flow.number:
            count = int(flow.number)
        else:
            time = parseTime(flow.end) - parseTime(flow.begin)
            if flow.probability:
                count = time * float(flow.probability)
            elif flow.vehsPerHour:
                count = time * float(flow.vehsPerHour) / 3600
            elif flow.period:
                count = time / float(flow.period)
        if count is None:
            print("Could not determine count for flow '%s'" % (flow.id))
            count = 1

        if flow.attr_from and flow.to:
            addVehicle(flow.id, flow.attr_from, flow.to, parseTime(flow.begin), count)
        elif flow.route and type(flow.route) == list:
            edges = flow.route[0].edges.split()
            addVehicle(flow.id, edges[0], edges[-1], parseTime(flow.begin), count)
        else:
            print("No edges found for flow '%s'" % flow.id)

    print("read %s vehicles" % nl.numVehicles)
    if nl.numFromNotFound > 0 or nl.numToNotFound > 0:
        print("No fromTaz found for %s edges and no toTaz found for %s edges" % (
            nl.numFromNotFound, nl.numToNotFound))

    if nl.numVehicles > 0:
        numOD = 0
        distinctOD = set()

        with open(options.outfile, 'w') as outf:
            sumolib.writeXMLHeader(outf, "$Id$", "data", "datamode_file.xsd", options=options)  # noqa
            for begin, tazRelations in intervals.items():
                if options.interval is not None:
                    end = begin + options.interval
                else:
                    end = nl.end + 1
                outf.write(4 * ' ' + '<interval id="%s" begin="%s" end="%s">\n' % (
                    options.intervalID, begin, end))
                for od in sorted(tazRelations.keys()):
                    numOD += 1
                    distinctOD.add(od)
                    outf.write(8 * ' ' + '<tazRelation from="%s" to="%s" count="%s"/>\n' % (
                        od[0], od[1], tazRelations[od]))
                outf.write(4 * ' ' + '</interval>\n')
            outf.write('</data>\n')

        print("Wrote %s OD-pairs (%s distinct) in %s intervals" % (
            numOD, len(distinctOD), len(intervals)))


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
