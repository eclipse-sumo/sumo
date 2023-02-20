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

# @file    route2OD.py
# @author  Jakob Erdmann
# @date    2021-09-09

# Given a route file
#   - a taz-based OD xml-file and/or
#   - a edge-based OD xml-file (optional)
# will be generated.
#

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
    optParser.add_argument("-a", "--taz-files", dest="tazfiles",
                           help="define the files to load TAZ (districts); "
                           "it is mandatory when generating taz-based OD file")
    optParser.add_argument("-o", "--output-file", dest="outfile",
                           help="define the output filename (mandatory)")
    optParser.add_argument("-i", "--interval",
                           help="define the output aggregation interval")
    optParser.add_argument("--id", default="DEFAULT_VEHTYPE", dest="intervalID",
                           help="define the output aggregation interval")
    optParser.add_argument("-s", "--seed", type=int, default=42, help="random seed")
    optParser.add_argument("--edge-relations", action="store_true", dest="edgeod", default=False,
                           help="generate edgeRelations instead of tazRelations")

    options = optParser.parse_args(args=args)
    if not options.routefile or not options.outfile or (not options.edgeod and not options.tazfiles):
        optParser.print_help()
        sys.exit(1)

    if options.interval is not None:
        options.interval = parseTime(options.interval)

    if not options.edgeod:
        options.tazfiles = options.tazfiles.split()
    return options


def main(options):
    random.seed(options.seed)
    if not options.edgeod:
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
                    for sink in taz.tazSink:
                        edgeToTaz[sink.id].append(taz.id)

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

    class nl:  # nonlocal integral variables
        numFromNotFound = 0
        numToNotFound = 0
        numVehicles = 0
        end = 0

    # begin -> od -> count
    intervals = defaultdict(lambda: defaultdict(lambda: 0))
    intervals_edge = defaultdict(lambda: defaultdict(lambda: 0))  # for options.edgeod

    def addVehicle(vehID, fromEdge, toEdge, time, count=1, isTaz=False):
        nl.numVehicles += count
        if options.interval is None:
            intervalBegin = 0
        else:
            intervalBegin = int(time / options.interval) * options.interval

        if options.edgeod and not isTaz:
            intervals_edge[intervalBegin][(fromEdge, toEdge)] += count
        else:
            fromTaz = None
            toTaz = None
            if isTaz:
                fromTaz = fromEdge
                toTaz = toEdge
            else:
                if fromEdge in edgeFromTaz:
                    fromTaz = random.choice(edgeFromTaz[fromEdge])
                if toEdge in edgeToTaz:
                    toTaz = random.choice(edgeToTaz[toEdge])

            if fromTaz and toTaz:
                intervals[intervalBegin][(fromTaz, toTaz)] += count
            else:
                if fromTaz is None:
                    nl.numFromNotFound += 1
                    if nl.numFromNotFound < 5:
                        if isTaz:
                            print("No fromTaz found for vehicle '%s' " % (vehID))
                        else:
                            print("No fromTaz found for edge '%s' of vehicle '%s' " % (fromEdge, vehID))
                if toTaz is None:
                    nl.numToNotFound += 1
                    if nl.numToNotFound < 5:
                        if isTaz:
                            print("No toTaz found for vehicle '%s' " % (vehID))
                        else:
                            print("No toTaz found for edge '%s' of vehicle '%s' " % (toEdge, vehID))

        nl.end = max(nl.end, time)

    for vehicle in sumolib.xml.parse(options.routefile, ['vehicle']):
        if vehicle.route and type(vehicle.route) == list:
            edges = vehicle.route[0].edges.split()
            addVehicle(vehicle.id, edges[0], edges[-1], parseTime(vehicle.depart))
        else:
            print("No edges found for vehicle '%s'" % vehicle.id)

    for trip in sumolib.xml.parse(options.routefile, ['trip']):
        if trip.attr_from and trip.to:
            addVehicle(trip.id, trip.attr_from, trip.to, parseTime(trip.depart))
        elif trip.fromTaz and trip.toTaz:
            if options.edgeod:
                print("No OD-edge information! Only TAZ-based OD counts with a given district file can be calculated.")
                sys.exit(1)
            else:
                addVehicle(trip.id, trip.fromTaz, trip.toTaz, parseTime(trip.depart), 1, True)

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
        elif flow.fromTaz and flow.toTaz:
            if options.edgeod:
                print("No OD-edge information! A district file is needed for TAZ-based OD counts.")
                sys.exit(1)
            else:
                addVehicle(flow.id, flow.fromTaz, flow.toTaz, parseTime(flow.begin), count, True)

        else:
            print("No edges found for flow '%s'" % flow.id)

    print("read %s vehicles" % nl.numVehicles)
    if not options.edgeod:
        if nl.numFromNotFound > 0 or nl.numToNotFound > 0:
            print("No fromTaz found for %s edges and no toTaz found for %s edges" % (
                nl.numFromNotFound, nl.numToNotFound))

    if nl.numVehicles > 0:
        numOD = 0
        numVehicles = 0
        distinctOD = set()
        if options.edgeod:
            edgeOD = set()
            with open(options.outfile, 'w') as outf:
                sumolib.writeXMLHeader(outf, "$Id$", "data", "datamode_file.xsd", options=options)  # noqa
                for begin, edgeRelations in intervals_edge.items():
                    if options.interval is not None:
                        end = begin + options.interval
                    else:
                        end = nl.end + 1
                    outf.write(4 * ' ' + '<interval id="%s" begin="%s" end="%s">\n' % (
                        options.intervalID, begin, end))
                    for od in sorted(edgeRelations.keys()):
                        numOD += 1
                        numVehicles += edgeRelations[od]
                        edgeOD.add(od)
                        outf.write(8 * ' ' + '<edgeRelation from="%s" to="%s" count="%s"/>\n' % (
                            od[0], od[1], edgeRelations[od]))
                    outf.write(4 * ' ' + '</interval>\n')
                outf.write('</data>\n')

            print("Wrote %s OD-pairs (%s edgeOD) in %s intervals (%s vehicles total)" % (
                numOD, len(edgeOD), len(intervals_edge), numVehicles))
        else:
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
                        numVehicles += tazRelations[od]
                        distinctOD.add(od)
                        outf.write(8 * ' ' + '<tazRelation from="%s" to="%s" count="%s"/>\n' % (
                            od[0], od[1], tazRelations[od]))
                    outf.write(4 * ' ' + '</interval>\n')
                outf.write('</data>\n')

            print("Wrote %s OD-pairs (%s distinct) in %s intervals (%s vehicles total)" % (
                numOD, len(distinctOD), len(intervals), numVehicles))


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
