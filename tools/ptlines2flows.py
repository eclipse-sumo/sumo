#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    ptlines2flows.py
# @author  Gregor Laemmel
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2017-06-23
# @version $Id$

from __future__ import print_function
import os
import sys
import codecs
import subprocess
import collections
import random
from xml.sax.saxutils import escape

from optparse import OptionParser

import sumolib

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=None):
    optParser = OptionParser()
    optParser.add_option("-n", "--net-file", dest="netfile", help="network file")
    optParser.add_option("-l", "--ptlines-file", dest="ptlines", help="public transit lines file")
    optParser.add_option("-s", "--ptstops-file", dest="ptstops", help="public transit stops file")
    optParser.add_option("-o", "--output-file", dest="outfile", default="flows.rou.xml", help="output flows file")
    optParser.add_option("-i", "--stopinfos-file", dest="stopinfos",
                         default="stopinfos.xml", help="file from '--stop-output'")
    optParser.add_option(
        "-r", "--routes-file", dest="routes", default="vehroutes.xml", help="file from '--vehroute-output'")
    optParser.add_option("-t", "--trips-file", dest="trips", default="trips.trips.xml", help="output trips file")
    optParser.add_option("-p", "--period", type=float, default=600, help="period")
    optParser.add_option("-b", "--begin", type=float, default=0, help="start time")
    optParser.add_option("-e", "--end", type=float, default=3600, help="end time")
    optParser.add_option("--min-stops", type=int, default=2, help="only import lines with at least this number of stops")
    optParser.add_option("-f", "--flow-attributes", dest="flowattrs",
                         default="", help="additional flow attributes")
    optParser.add_option("--use-osm-routes", default=False, action="store_true", dest='osmRoutes', help="use osm routes")
    optParser.add_option("--random-begin", default=False, action="store_true", dest='randomBegin', help="randomize begin times within period")
    optParser.add_option("--seed", type="int", help="random seed")
    optParser.add_option("--ignore-errors", default=False, action="store_true", dest='ignoreErrors', help="ignore problems with the input data")
    optParser.add_option("--no-vtypes", default=False, action="store_true", dest='novtypes', help="do not write vtypes for generated flows")
    optParser.add_option("--types", help="only export the given list of types (using OSM nomenclature)")
    optParser.add_option("--bus.parking", default=False, action="store_true", dest='busparking', help="let busses clear the road while stopping")
    optParser.add_option("--vtype-prefix", default="", dest='vtypeprefix', help="prefix for vtype ids")
    optParser.add_option("-d", "--stop-duration", default=30, type="float", dest='stopduration', 
            help="Configure the minimum stopping duration")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)

    if options.netfile is None or options.ptlines is None or options.ptstops is None:
        sys.stderr.write("Error: net-file, ptlines-file and ptstops-file must be set\n")
        optParser.print_help()
        sys.exit(1)

    if options.types is not None:
        options.types = options.types.split(',')

    return options

def writeTypes(fout, prefix):
    print("""    <vType id="%sbus" vClass="bus"/>
    <vType id="%stram" vClass="tram"/>
    <vType id="%strain" vClass="rail"/>
    <vType id="%ssubway" vClass="rail_urban"/>
    <vType id="%slight_rail" vClass="rail_urban"/>
    <vType id="%smonorail" vClass="rail"/>
    <vType id="%strolleybus" vClass="bus"/>
    <vType id="%saerialway" vClass="bus"/>
    <vType id="%sferry" vClass="ship"/>""" % tuple([prefix]*9), file=fout)


def createTrips(options):
    print("generating trips...")
    stopsLanes = {}
    for stop in sumolib.output.parse_fast(options.ptstops, 'busStop', ['id', 'lane']):
        stopsLanes[stop.id] = stop.lane

    trpMap = {}
    with open(options.trips, 'w') as fouttrips:
        sumolib.writeXMLHeader(
            fouttrips, "$Id$", "routes")
        writeTypes(fouttrips, options.vtypeprefix)

        departTimes = [0 for line in sumolib.output.parse_fast(options.ptlines, 'ptLine', ['id'])]
        if options.randomBegin:
            departTimes = sorted([int(random.random() * options.period) for t in departTimes])

        for trp_nr, line in enumerate(sumolib.output.parse(options.ptlines, 'ptLine')):
            stop_ids = []
            for stop in line.busStop:
                if not stop.id in stopsLanes:
                    sys.stderr.write("Warning: skipping unknown stop '%s'\n" % stop.id)
                    continue
                laneId = stopsLanes[stop.id]
                try:
                    edge_id, lane_index = laneId.rsplit("_", 1)
                except ValueError:
                    if options.ignoreErrors:
                        sys.stderr.write("Warning: ignoring stop '%s' on invalid lane '%s'\n" % (stop.id, laneId))
                        continue
                    else:
                        sys.exit("Invalid lane '%s' for stop '%s'" % (laneId, stop.id))
                stop_ids.append(stop.id)

            if options.types is not None and not line.type in options.types:
                if options.verbose:
                    print("Skipping line '%s' because it has type '%s'\n" % (line.id, line.type))
                continue

            if len(stop_ids) < options.min_stops:
                sys.stderr.write("Warning: skipping line '%s' because it has too few stops\n" % line.id)
                continue

            begin = departTimes[trp_nr]
            if options.osmRoutes and line.route is not None:
                edges = line.route[0].edges.split()
                vias = ''
                if len(edges) > 2:
                    vias = ' via="%s"' % (' '.join(edges[1:-1]))
                fouttrips.write(
                    '    <trip id="%s" type="%s%s" depart="%s" departLane="%s" from="%s" to="%s"%s>\n' % (
                        line.id, options.vtypeprefix, line.type, begin, 'best', edges[0], edges[-1], vias))
            else:
                if len(stop_ids) == 0:
                    sys.stderr.write("Warning: skipping line '%s' because it has no stops\n" % line.id)
                    continue
                fr, _ = stopsLanes[stop_ids[0]].rsplit("_", 1)
                to, _ = stopsLanes[stop_ids[-1]].rsplit("_", 1)
                fouttrips.write(
                    '    <trip id="%s" type="%s%s" depart="%s" departLane="%s" from="%s" to="%s">\n' % (
                        line.id, options.vtypeprefix, line.type, begin, 'best', fr, to))

            trpMap[line.id] = (line.line.replace(" ", "_"), line.attr_name, line.completeness)
            for stop in stop_ids:
                fouttrips.write('        <stop busStop="%s" duration="%s"/>\n' % (stop, options.stopduration))
            fouttrips.write('    </trip>\n')
        fouttrips.write("</routes>\n")
    print("done.")
    return trpMap


def runSimulation(options):
    print("running SUMO to determine actual departure times...")
    subprocess.call([sumolib.checkBinary("sumo"), 
                     "-n", options.netfile,
                     "-r", options.trips,
                     "--begin", str(options.begin),
                     "--no-step-log",
                     "--ignore-route-errors",
                     "--error-log", options.trips + ".errorlog",
                     "-a", options.ptstops,
                     "--vehroute-output", options.routes,
                     "--stop-output", options.stopinfos, ])
    print("done.")


def createRoutes(options, trpMap):
    print("creating routes...")
    stopsUntil = {}
    for stop in sumolib.output.parse_fast(options.stopinfos, 'stopinfo', ['id', 'ended', 'busStop']):
        stopsUntil[(stop.id, stop.busStop)] = float(stop.ended)

    with codecs.open(options.outfile, 'w', encoding="UTF8") as foutflows:
        flows = []
        actualDepart = {} # departure may be delayed when the edge is not yet empty
        sumolib.writeXMLHeader(
            foutflows, "$Id$", "routes")
        if not options.novtypes:
            writeTypes(foutflows, options.vtypeprefix)
        for vehicle in sumolib.output.parse(options.routes, 'vehicle'):
            id = vehicle.id
            try:
                if vehicle.route is not None:
                    edges = vehicle.route[0].edges
                else:
                    edges = vehicle.routeDistribution[0].route[1].edges
            except StandardError:
                if options.ignoreErrors:
                    sys.stderr.write("Warning: Could not parse edges for vehicle '%s'\n" % id)
                    continue
                else:
                    sys.exit("Could not parse edges for vehicle '%s'\n" % id)
            flows.append((id, vehicle.type, float(vehicle.depart)))
            actualDepart[id] = float(vehicle.depart)
            parking = ' parking="true"' if vehicle.type == "bus" and options.busparking else ''
            stops = vehicle.stop
            foutflows.write('    <route id="%s" edges="%s" >\n' % (id, edges))
            if vehicle.stop is not None:
                for stop in stops:
                    if (id, stop.busStop) in stopsUntil:
                        untilZeroBased = stopsUntil[(id, stop.busStop)] - actualDepart[id]
                        foutflows.write(
                            '        <stop busStop="%s" duration="%s" until="%s"%s/>\n' % (
                                stop.busStop, stop.duration, untilZeroBased, parking))
                    else:
                        sys.stderr.write("Warning: Missing stop '%s' for flow '%s'\n" % (stop.busStop, id))
            else:
                sys.stderr.write("Warning: No stops for flow '%s'\n" % id)
            foutflows.write('    </route>\n')
        lineCount = collections.defaultdict(int)
        flow_duration = options.end - options.begin
        for flow, type, begin in flows:
            line, name, completeness = trpMap[flow]
            lineRef = "%s:%s" % (line, lineCount[line])
            lineCount[line] += 1
            foutflows.write('    <flow id="%s_%s" type="%s" route="%s" begin="%s" end="%s" period="%s" line="%s" %s>\n' % (
                type, lineRef, type, flow, begin, begin + flow_duration, options.period, lineRef, options.flowattrs))
            foutflows.write('        <param key="name" value="%s"/>\n        <param key="completeness" value="%s"/>\n    </flow>\n' %
                            (escape(name), completeness))
        foutflows.write('</routes>\n')

    print("done.")


def main(options):
    if options.seed:
        random.seed(options.seed)
    trpMap = createTrips(options)
    runSimulation(options)
    createRoutes(options, trpMap)


if __name__ == "__main__":
    main(get_options())
