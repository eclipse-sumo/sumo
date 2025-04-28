#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    ptlines2flows.py
# @author  Gregor Laemmel
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2017-06-23

from __future__ import print_function
import sys
import codecs
import subprocess
import collections
import random
import math
from io import StringIO

import sumolib
from sumolib.xml import quoteattr
from sumolib.options import ArgumentParser
from sumolib import geomhelper


def get_options(args=None):
    ap = ArgumentParser()
    ap.add_option("-n", "--net-file", dest="netfile", category="input", required=True,
                  help="network file")
    ap.add_option("-l", "--ptlines-file", dest="ptlines", category="input", required=True,
                  help="public transit lines file")
    ap.add_option("-s", "--ptstops-file", dest="ptstops", category="input", required=True,
                  help="public transit stops file")
    ap.add_option("-o", "--output-file", dest="outfile", category="output", default="flows.rou.xml",
                  help="output flows file")
    ap.add_option("-i", "--stopinfos-file", dest="stopinfos", category="output", default="stopinfos.xml",
                  help="file from '--stop-output'")
    ap.add_option("-r", "--routes-file", dest="routes", category="output", default="vehroutes.xml",
                  help="file from '--vehroute-output'")
    ap.add_option("-t", "--trips-file", dest="trips", default="trips.trips.xml", help="output trips file")
    ap.add_option("-p", "--period", type=ap.time, default=600,
                  help="the default service period (in seconds) to use if none is specified in the ptlines file")
    ap.add_option("--period-aerialway", type=ap.time, default=60, dest="periodAerialway",
                  help=("the default service period (in seconds) to use for aerialways "
                        "if none is specified in the ptlines file"))
    ap.add_option("-b", "--begin", type=ap.time, default=0, help="start time")
    ap.add_option("-e", "--end", type=ap.time, default=3600, help="end time")
    ap.add_option("-j", "--jump-duration", type=ap.time, default=180, dest="jumpDuration",
                  help="The time to add for each missing stop (when joining opposite lines)")
    ap.add_option("-T", "--turnaround-duration", type=ap.time, default=300,
                  help="The extra stopping time at terminal stops")
    ap.add_option("--join", default=False, action="store_true",
                  help="Join opposite lines at the terminals")
    ap.add_option("--join-threshold", default=100, type=float, dest='joinThreshold',
                  help="maximum distance for terminal stops when joining lines")
    ap.add_option("--multistart", default=False, action="store_true",
                  help="Insert multiple vehicles per line at different offsets along the route to avoid warmup")
    ap.add_option("--min-stops", type=int, default=2, help="only import lines with at least this number of stops")
    ap.add_option("-f", "--flow-attributes", dest="flowattrs", default="", help="additional flow attributes")
    ap.add_option("--use-osm-routes", default=False, action="store_true", dest='osmRoutes', help="use osm routes")
    ap.add_option("--extend-to-fringe", default=False, action="store_true", dest='extendFringe',
                  help="let routes of incomplete lines start/end at the network border if the route edges are known")
    ap.add_option("--random-begin", default=False, action="store_true", dest='randomBegin',
                  help="randomize begin times within period")
    ap.add_option("--seed", type=int, help="random seed")
    ap.add_option("--ignore-errors", default=False, action="store_true", dest='ignoreErrors',
                  help="ignore problems with the input data")
    ap.add_option("--no-vtypes", default=False, action="store_true", dest='novtypes',
                  help="do not write vtypes for generated flows")
    ap.add_option("--types", help="only export the given list of types (using OSM nomenclature)")
    ap.add_option("--bus.parking", default=False, action="store_true", dest='busparking',
                  help="let busses clear the road while stopping")
    ap.add_option("--vtype-prefix", default="", dest='vtypeprefix', help="prefix for vtype ids")
    ap.add_option("-d", "--stop-duration", default=20, type=float, dest='stopduration',
                  help="Configure the minimum stopping duration")
    ap.add_option("--stop-duration-slack", default=10, type=float, dest='stopdurationSlack',
                  help="Stopping time reserve in the schedule")
    ap.add_option("--speedfactor.bus", default=0.95, type=float, dest='speedFactorBus',
                  help="Assumed bus relative travel speed")
    ap.add_option("--speedfactor.tram", default=1.0, type=float, dest='speedFactorTram',
                  help="Assumed tram relative travel speed")
    ap.add_option("-H", "--human-readable-time", dest="hrtime", default=False, action="store_true",
                  help="write times as h:m:s")
    ap.add_option("--night", action="store_true", default=False, help="Export night service lines")
    ap.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")
    options = ap.parse_args(args=args)

    if options.netfile is None or options.ptlines is None or options.ptstops is None:
        sys.stderr.write("Error: net-file, ptlines-file and ptstops-file must be set\n")
        ap.print_help()
        sys.exit(1)

    if options.begin >= options.end:
        sys.stderr.write("Error: end time must be larger than begin time\n")
        ap.print_help()
        sys.exit(1)

    if options.types is not None:
        options.types = options.types.split(',')

    return options


class PTLine:
    def __init__(self, ref, name, completeness, missingBefore, missingAfter,
                 fromEdge, toEdge, period, color, refOrig,
                 typeID, depart, stop_ids, vias):
        self.ref = ref
        self.name = name
        self.completeness = completeness
        self.missingBefore = int(missingBefore) if missingBefore is not None else 0
        self.missingAfter = int(missingAfter) if missingAfter is not None else 0
        self.fromEdge = fromEdge
        self.toEdge = toEdge
        self.period = period
        self.color = color
        self.refOrig = refOrig
        self.typeID = typeID
        self.depart = depart
        self.stop_ids = stop_ids
        self.vias = vias
        # stop indices that need special handling
        self.jumps = {}  # stopIndex -> duration
        self.terminalIndices = []


def writeTypes(fout, prefix, options):
    # note: public transport vehicles have speedDev="0" by default
    prefixes_and_sf = [prefix, ""] * 11
    if options:
        # bus
        prefixes_and_sf[1] = ' speedFactor="%s"' % options.speedFactorBus
        # tram
        prefixes_and_sf[3] = ' speedFactor="%s"' % options.speedFactorTram
        # trolleybus
        prefixes_and_sf[13] = ' speedFactor="%s"' % options.speedFactorBus
        # minibus
        prefixes_and_sf[15] = ' speedFactor="%s"' % options.speedFactorBus
        # share_taxi
        prefixes_and_sf[17] = ' speedFactor="%s"' % options.speedFactorBus

    print("""    <vType id="%sbus" vClass="bus"%s/>
    <vType id="%stram" vClass="tram"%s/>
    <vType id="%strain" vClass="rail"%s/>
    <vType id="%ssubway" vClass="subway"%s/>
    <vType id="%slight_rail" vClass="rail_urban"%s/>
    <vType id="%smonorail" vClass="rail_urban"%s/>
    <vType id="%strolleybus" vClass="bus"%s/>
    <vType id="%sminibus" vClass="bus"%s/>
    <vType id="%sshare_taxi" vClass="taxi"%s/>
    <vType id="%saerialway" vClass="cable_car"%s length="2.5" width="2" personCapacity="4"/>
    <vType id="%sferry" vClass="ship"%s/>""" % tuple(prefixes_and_sf), file=fout)


def createTrips(options):
    print("generating trips...")
    tripList = []  # ids
    trpMap = {}  # ids->PTLine

    departTimes = [options.begin for line in sumolib.output.parse_fast(options.ptlines, 'ptLine', ['id'])]
    if options.randomBegin:
        departTimes = sorted([options.begin
                              + int(random.random() * options.period) for t in departTimes])

    lineCount = collections.defaultdict(int)
    typeCount = collections.defaultdict(int)
    numLines = 0
    numStops = 0
    numSkipped = 0
    for trp_nr, line in enumerate(sumolib.output.parse(options.ptlines, 'ptLine', heterogeneous=True)):
        stop_ids = []
        if not line.hasAttribute("period"):
            if line.type == "aerialway":
                line.setAttribute("period", options.periodAerialway)
            else:
                line.setAttribute("period", options.period)
        if line.busStop is not None:
            for stop in line.busStop:
                if stop.id not in options.stopEdges:
                    sys.stderr.write("Warning: skipping unknown stop '%s'\n" % stop.id)
                    continue
                stop_ids.append(stop.id)

        if options.types is not None and line.type not in options.types:
            if options.verbose:
                print("Skipping line '%s' because it has type '%s'" % (line.id, line.type))
            numSkipped += 1
            continue

        if line.hasAttribute("nightService"):
            if line.nightService == "only" and not options.night:
                if options.verbose:
                    print("Skipping line '%s' because it only drives at night" % (line.id))
                numSkipped += 1
                continue
            if line.nightService == "no" and options.night:
                if options.verbose:
                    print("Skipping line '%s' because it only drives during the day" % (line.id))
                numSkipped += 1
                continue

        lineRefOrig = line.line.replace(" ", "_")
        lineRefOrig = lineRefOrig.replace(";", "+")
        lineRefOrig = lineRefOrig.replace(">", "")
        lineRefOrig = lineRefOrig.replace("<", "")

        if len(stop_ids) < options.min_stops:
            sys.stderr.write("Warning: skipping line '%s' (%s_%s) because it has too few stops\n" % (
                line.id, line.type, lineRefOrig))
            numSkipped += 1
            continue

        lineRef = "%s:%s" % (lineRefOrig, lineCount[lineRefOrig])
        lineCount[lineRefOrig] += 1
        tripID = "%s_%s_%s" % (trp_nr, line.type, lineRef)

        edges = []
        fromEdge = None
        toEdge = None
        vias = []
        net = options.net
        if line.route is not None:
            missing = []
            for e in line.route[0].edges.split():
                if net.hasEdge(e):
                    edges.append(e)
                else:
                    missing.append(e)
            if missing and options.verbose:
                print("Removed %s missing edges from OSM route for line '%s'" % (len(missing), line.id))

        if options.osmRoutes and len(edges) == 0 and options.verbose:
            print("Cannot use OSM route for line '%s' (no edges given)" % line.id)
        elif options.osmRoutes and len(edges) > 0:
            fromEdge = edges[0]
            toEdge = edges[-1]
            if len(edges) > 2:
                vias = edges[1:-1]
        else:
            if options.extendFringe and len(edges) > len(stop_ids):
                fromEdge = edges[0]
                toEdge = edges[-1]
                # ensure that route actually covers the terminal stops
                # (otherwise rail network may be invalid beyond stops)
                if len(stop_ids) > 0:
                    firstStop = options.stopEdges[stop_ids[0]]
                    lastStop = options.stopEdges[stop_ids[-1]]
                    if firstStop not in edges:
                        fromEdge = firstStop
                        if options.verbose:
                            print(("Cannot extend route before first stop for line '%s' " +
                                   "(stop edge %s not in route)") % (line.id, firstStop))
                    if lastStop not in edges:
                        toEdge = lastStop
                        if options.verbose:
                            print(("Cannot extend route after last stop for line '%s' " +
                                   "(stop edge %s not in route)") % (line.id, lastStop))
                    else:
                        # if route is split, the route is incomplete and we
                        # should not extend beyond the split
                        prev = None
                        lastStopIndex = edges.index(lastStop)
                        for e in edges[lastStopIndex:]:
                            edge = net.getEdge(e)
                            if prev is not None and not prev.getConnections(edge):
                                toEdge = prev.getID()
                                if options.verbose:
                                    print("Extending up to route split between %s and %s" % (
                                        prev.getID(), edge.getID()))
                                break
                            prev = edge
            else:
                if options.extendFringe and options.verbose:
                    print("Cannot extend route to fringe for line '%s' (not enough edges given)" % line.id)
                if len(stop_ids) == 0:
                    sys.stderr.write("Warning: skipping line '%s' because it has no stops\n" % line.id)
                    numSkipped += 1
                    continue
                fromEdge = options.stopEdges[stop_ids[0]]
                toEdge = options.stopEdges[stop_ids[-1]]

        missingBefore = 0 if line.completeness == 1 else line.missingBefore
        missingAfter = 0 if line.completeness == 1 else line.missingAfter
        typeID = options.vtypeprefix + line.type
        trpMap[tripID] = PTLine(lineRef, line.attr_name, line.completeness,
                                missingBefore, missingAfter,
                                fromEdge, toEdge,
                                line.period,
                                line.getAttributeSecure("color"),
                                lineRefOrig,
                                typeID,
                                departTimes[trp_nr],
                                stop_ids, vias)
        tripList.append(tripID)
        typeCount[line.type] += 1
        numLines += 1
        numStops += len(stop_ids)

    if options.join:
        joinTrips(options, tripList, trpMap)
    writeTrips(options, tripList, trpMap)
    if options.verbose:
        print("Imported %s lines with %s stops and skipped %s lines" % (numLines, numStops, numSkipped))
        for lineType, count in sorted(typeCount.items()):
            print("   %s: %s" % (lineType, count))
    print("done.")
    return trpMap


def joinTrips(options, tripList, trpMap):
    net = options.net
    # join opposite pairs of trips
    linePairs = collections.defaultdict(list)
    for tripID, ptl in trpMap.items():
        linePairs[ptl.refOrig].append(tripID)

    for refOrig, tripIDs in linePairs.items():
        if len(tripIDs) > 2:
            sys.stderr.write("Warning: Cannot join line '%s' with %s trips\n" % (refOrig, len(tripIDs)))
        elif len(tripIDs) == 2:
            ptl1 = trpMap[tripIDs[0]]
            ptl2 = trpMap[tripIDs[1]]
            join1 = distCheck(options, refOrig, ptl1.toEdge, ptl2.fromEdge)
            join2 = distCheck(options, refOrig, ptl1.fromEdge, ptl2.toEdge)
            if not join1 and not join2:
                continue
            ptl1.completeness = str(0.5 * (float(ptl1.completeness) + float(ptl2.completeness)))
            ptl2.completeness = ptl1.completeness
            ptl1.ref = ptl1.refOrig
            ptl2.ref = ptl2.refOrig

            from1 = net.getEdge(ptl1.fromEdge)
            from2 = net.getEdge(ptl2.fromEdge)
            to1 = net.getEdge(ptl1.toEdge)
            to2 = net.getEdge(ptl2.toEdge)
            missingPart1 = ptl1.missingAfter + ptl2.missingBefore
            missingPart2 = ptl2.missingAfter + ptl1.missingBefore

            if join1:
                # append ptl1 after ptl2
                tripList.remove(tripIDs[1])
                if missingPart1 != 0 or to1.getBidi() != from2.getID():
                    ptl1.jumps[len(ptl1.stop_ids) - 1] = missingPart1 * options.jumpDuration
                ptl1.terminalIndices.append(len(ptl1.stop_ids) - 1)
                ptl1.stop_ids += ptl2.stop_ids
                if ptl1.vias:
                    ptl1.vias += [ptl1.toEdge, ptl2.fromEdge] + ptl2.vias
                ptl1.toEdge = ptl2.toEdge

                if join2:
                    # line forms a full circle so that vehicles can stay in the simulation continously.
                    # We have to compute the appropriate number of vehicles and then adapt the end time
                    if missingPart2 != 0 or to2.getBidi() != from1.getID():
                        ptl1.jumps[len(ptl1.stop_ids) - 1] = missingPart2 * options.jumpDuration
                    ptl1.terminalIndices.append(len(ptl1.stop_ids) - 1)

            elif join2:
                # only append ptl1 after ptl2
                tripList.remove(tripIDs[0])
                if missingPart2 != 0 or to2.getBidi() != from1.getID():
                    ptl2.jumps[len(ptl2.stop_ids) - 1] = missingPart2 * options.jumpDuration
                ptl2.terminalIndices.append(len(ptl2.stop_ids) - 1)
                ptl2.stop_ids += ptl1.stop_ids
                if ptl2.vias:
                    ptl2.vias += [ptl2.toEdge, ptl1.fromEdge] + ptl1.vias
                ptl2.toEdge = ptl1.toEdge


def distCheck(options, refOrig, eID1, eID2):
    net = options.net
    shape1 = net.getEdge(eID1).getShape(True)
    shape2 = net.getEdge(eID2).getShape(True)
    minDist = options.joinThreshold + 1
    for p in shape1:
        minDist = min(minDist, geomhelper.distancePointToPolygon(p, shape2))
    if minDist > options.joinThreshold:
        sys.stderr.write("Warning: Cannot join line '%s' at edges '%s' and '%s' with distance %s\n" % (
            refOrig, eID1, eID2, minDist))
        return False
    else:
        return True


def writeTrips(options, tripList, trpMap):
    with codecs.open(options.trips, 'w', encoding="UTF8") as fouttrips:
        sumolib.writeXMLHeader(
            fouttrips, "$Id: ptlines2flows.py v1_3_1+0313-ccb31df3eb jakob.erdmann@dlr.de 2019-09-02 13:26:32 +0200 $",
            "routes", options=options)
        writeTypes(fouttrips, options.vtypeprefix, options)

        for tripID in tripList:
            ptl = trpMap[tripID]
            via = ' via="%s"' % ' '.join(ptl.vias) if ptl.vias else ""
            fouttrips.write(
                ('    <trip id="%s" type="%s" depart="%s" departLane="best" from="%s" ' +
                 'to="%s"%s>\n') % (
                    tripID, ptl.typeID, ptl.depart, ptl.fromEdge, ptl.toEdge, via))
            for i, stop in enumerate(ptl.stop_ids):
                dur = options.stopduration + options.stopdurationSlack
                comment = "  <!-- %s -->" % options.stopNames.get(stop)
                if i in ptl.jumps:
                    jump = ' jump="%s"' % ptl.jumps[i]
                else:
                    jump = ""
                fouttrips.write('        <stop busStop="%s" duration="%s"%s/>%s\n' % (stop, dur, jump, comment))
            fouttrips.write('    </trip>\n')
        fouttrips.write("</routes>\n")


def runSimulation(options):
    print("running SUMO to determine actual departure times...")
    subprocess.call([sumolib.checkBinary("sumo"),
                     "-n", options.netfile,
                     "-r", options.trips,
                     "--begin", str(options.begin),
                     "--no-step-log",
                     "--ignore-route-errors",
                     "--time-to-teleport.disconnected", "0",
                     "--error-log", options.trips + ".errorlog",
                     "-a", options.ptstops,
                     "--device.rerouting.adaptation-interval", "0",  # ignore tls and traffic effects
                     "--vehroute-output", options.routes,
                     "--stop-output", options.stopinfos,
                     "--aggregate-warnings", "5"])
    print("done.")
    sys.stdout.flush()


def formatTime(seconds):
    return "%02i:%02i:%02i" % (seconds / 3600, (seconds % 3600) / 60, (seconds % 60))


def createRoutes(options, trpMap):
    net = options.net
    print("creating routes...")
    stopsUntil = collections.defaultdict(list)
    for stop in sumolib.output.parse_fast(options.stopinfos, 'stopinfo', ['id', 'ended', 'busStop']):
        stopsUntil[(stop.id, stop.busStop)].append(float(stop.ended))

    ft = formatTime if options.hrtime else lambda x: x
    routes = []
    flows = []
    actualDepart = {}  # departure may be delayed when the edge is not yet empty
    collections.defaultdict(int)
    routeDurations = {}
    routeSize = {}
    flow_duration = options.end - options.begin
    for vehicle in sumolib.output.parse(options.routes, 'vehicle'):
        tmpio = StringIO()
        id = vehicle.id
        ptline = trpMap[id]
        flowID = "%s_%s" % (vehicle.type, ptline.ref)
        try:
            if vehicle.route is not None:
                edges = vehicle.route[0].edges
            else:
                edges = vehicle.routeDistribution[0].route[1].edges
            routeSize[flowID] = len(edges.split())
        except BaseException:
            if options.ignoreErrors:
                sys.stderr.write("Warning: Could not parse edges for vehicle '%s'\n" % id)
                continue
            else:
                sys.exit("Could not parse edges for vehicle '%s'\n" % id)

        flows.append((id, flowID, ptline.ref, vehicle.type, float(vehicle.depart)))
        actualDepart[id] = float(vehicle.depart)
        parking = ' parking="true"' if vehicle.type == "bus" and options.busparking else ''
        color = ' color="%s"' % ptline.color if ptline.color is not None else ""
        repeat = ""
        stops = vehicle.stop
        if len(ptline.terminalIndices) == 2 and stops:
            lastBusStop = stops[-1].busStop
            lastUntil = stopsUntil.get((id, lastBusStop))
            if lastUntil is not None:
                cycleTime = lastUntil[-1] - actualDepart[id]
                numRepeats = math.ceil(flow_duration / cycleTime)
                if numRepeats > 1:
                    repeat = ' repeat="%s" cycleTime="%s"' % (numRepeats, ft(cycleTime))

        # jump over disconnected parts
        jumps = {}  # edge -> duration
        usedJumps = set()
        prev = None
        for edgeID in edges.split():
            edge = net.getEdge(edgeID)
            if prev is not None and not prev.getConnections(edge):
                jumpDist = geomhelper.distance(prev.getToNode().getCoord(), edge.getFromNode().getCoord())
                jumpSpeed = (prev.getSpeed() + edge.getSpeed()) * 0.5
                jumpDuration = jumpDist / jumpSpeed
                jumps[prev.getID()] = jumpDuration
            prev = edge
        # record all stops on an edge (only the last stop needs to jump when there is a disconnect
        edgeStops = collections.defaultdict(list)
        if stops is not None:
            for stop in stops:
                edgeStops[options.stopEdges[stop.busStop]].append(stop.busStop)

        tmpio.write('    <route id="%s"%s edges="%s"%s >\n' % (flowID, color, edges, repeat))
        if stops is not None:
            untilOffset = 0
            for stop in stops:
                if (id, stop.busStop) in stopsUntil:
                    stopEdge = options.stopEdges[stop.busStop]
                    until = stopsUntil[(id, stop.busStop)]
                    stopname = ''
                    if stop.busStop in options.stopNames:
                        stopname = ' <!-- %s -->' % options.stopNames[stop.busStop]
                    untilZeroBased = until[0] - actualDepart[id] + untilOffset
                    if stop.jump is not None:
                        jump = ' jump="%s"' % stop.jump
                    elif stopEdge in jumps and stop.busStop == edgeStops[stopEdge][-1]:
                        #  only the last stop on an edge needs the jump
                        usedJumps.add(stopEdge)
                        untilOffset += jumps[stopEdge]
                        jump = ' jump="%s"' % jumps[stopEdge]
                    else:
                        jump = ""
                    if len(until) > 1:
                        stopsUntil[(id, stop.busStop)] = until[1:]
                    tmpio.write(
                        '        <stop busStop="%s" duration="%s" until="%s"%s%s/>%s\n' % (
                            stop.busStop, options.stopduration, ft(untilZeroBased), parking, jump, stopname))
                    routeDurations[flowID] = untilZeroBased
                else:
                    sys.stderr.write("Warning: Missing stop '%s' for flow '%s'\n" % (stop.busStop, id))
        else:
            sys.stderr.write("Warning: No stops for flow '%s'\n" % id)
        for edgeID, jumpDuration in jumps.items():
            if edgeID not in usedJumps:
                tmpio.write(
                    '        <stop edge="%s" speed="999" jump="%s" index="fit"/>\n' % (edgeID, jumpDuration))

        tmpio.write('    </route>\n')
        routes.append((flowID, tmpio.getvalue()))

    with codecs.open(options.outfile, 'w', encoding="UTF8") as foutflows:
        sumolib.writeXMLHeader(foutflows, root="routes", options=options)
        if not options.novtypes:
            writeTypes(foutflows, options.vtypeprefix, None)

        for routeID, routeStr in sorted(routes):
            foutflows.write(routeStr)

        for vehID, flowID, lineRef, type, begin in sorted(flows):
            ptline = trpMap[vehID]
            number = None
            if flowID in routeDurations:
                number = max(1, int(routeDurations[flowID] / float(ptline.period)))
            if options.multistart and len(ptline.terminalIndices) == 2:
                # vehicles stay in a continuous loop. We create a fixed number of vehicles with repeating routes
                for i in range(number):
                    # this is a hack since edges could have very different lengths
                    departEdge = int(i * routeSize[flowID] / float(number))
                    foutflows.write('    <vehicle id="%s.%s" type="%s" route="%s" depart="%s" departEdge="%s" line="%s" %s>\n' % (  # noqa
                        flowID, i, type, flowID, ft(begin), departEdge, ptline.ref, options.flowattrs))
                    writeParams(foutflows, ptline)
                    foutflows.write('    </vehicle>\n')
                foutflows.write('\n')

            else:
                end = ' end="%s"' % ft(begin + flow_duration)
                if len(ptline.terminalIndices) == 2 and number is not None:
                    end = ' number="%s"' % number
                foutflows.write('    <flow id="%s" type="%s" route="%s" begin="%s"%s period="%s" line="%s" %s>\n' % (
                    flowID, type, flowID, ft(begin), end,
                    int(float(ptline.period)), ptline.ref, options.flowattrs))
                writeParams(foutflows, ptline)
                foutflows.write('    </flow>\n')
        foutflows.write('</routes>\n')

    print("done.")


def writeParams(foutflows, ptline):
    if ptline.name is not None:
        foutflows.write('        <param key="name" value=%s/>\n' % quoteattr(ptline.name))
    if ptline.completeness is not None:
        foutflows.write('        <param key="completeness" value=%s/>\n' % quoteattr(ptline.completeness))


def main(options):
    if options.seed:
        random.seed(options.seed)
    sys.stderr.flush()
    options.net = sumolib.net.readNet(options.netfile)
    options.stopEdges = {}
    options.stopNames = {}
    for stop in sumolib.output.parse(options.ptstops, ['busStop', 'trainStop']):
        options.stopEdges[stop.id] = sumolib._laneID2edgeID(stop.lane)
        if stop.name:
            options.stopNames[stop.id] = stop.attr_name

    trpMap = createTrips(options)
    sys.stderr.flush()
    runSimulation(options)
    createRoutes(options, trpMap)


if __name__ == "__main__":
    main(get_options())
