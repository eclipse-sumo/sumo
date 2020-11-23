#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateRailSignalConstraints.py
# @author  Jakob Erdmann
# @date    2020-08-31

"""
Generate railSignalConstrains definitions that enforce a loaded rail schedule

Two types of constraints are generated:
1. <predecessor>
When two vehices stop subsequently at the same busStop (trainStop) and they reach that stop
via different routes, the switch where both routes merge is identified and a
constraint is created for the rail signals that guard this merging switch:
    The vehicle B that arrives at the stop later, must wait (at its signal Y)
    for the vehicle A that arrives first (to pass it's respective signal X)
    This uses the 'arrival' attribute of the vehicle stops

A complication arrises if the signal of the first vehicle is passed by other
trains which are en route to another stop. This makes it necessary to record a
larger number of passing vehicles within the simulation (controlled by the
limit attribute). The script attempts to determine the necessary limit value by
identifying all vehicles that pass the signal X en route to other stops between
the time A and B reach their respective signals (counting backwards from the
next stop based on "arrival". To account for delays the
options --delay and --limit can be used to override the limit values

2. <insertionPredecessor>
Whenever a vehicle B departs at a stop (assumed to coincide with the "until"
attribute of it's first stop), the prior train A that leaves this stop is
identified (also based on "until"). Then a constraint is created that prevents
insertion of B until train A has passed the next signal that lies beyond the
stop.
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
from collections import defaultdict
from operator import itemgetter

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime, parseBool, humanReadableTime  # noqa

DUAROUTER = sumolib.checkBinary('duarouter')


def get_options(args=None):
    parser = sumolib.options.ArgumentParser(description="Sample routes to match counts")
    parser.add_argument("-n", "--net-file", dest="netFile",
                        help="Input network file")
    parser.add_argument("-a", "--additional-file", dest="addFile",
                        help="Input additional file (busStops)")
    parser.add_argument("-t", "--trip-file", dest="tripFile",
                        help="Input trip file (will be processed into a route file)")
    parser.add_argument("-r", "--route-file", dest="routeFile",
                        help="Input route file (must contain routed vehicles rather than trips)")
    parser.add_argument("-o", "--output-file", dest="out", default="constraints.add.xml",
                        help="Output additional file")
    parser.add_argument("-b", "--begin", default="0",
                        help="ignore vehicles departing before the given begin time (seconds or H:M:S)")
    parser.add_argument("--until-from-duration", action="store_true", default=False, dest="untilFromDuration",
                        help="Use stop arrival+duration instead of 'until' to compute insertion constraints")
    parser.add_argument("-d", "--delay", default="0",
                        help="Assume given maximum delay when computing the number of intermediate vehicles " +
                        "that pass a given signal (for setting limit)")
    parser.add_argument("-l", "--limit", type=int, default=0,
                        help="Increases the limit value for tracking passed vehicles by the given amount")
    parser.add_argument("--abort-unordered", dest="abortUnordered", action="store_true", default=False,
                        help="Abort generation of constraints for a stop "
                        "once the ordering of vehicles by 'arrival' differs from the ordering by 'until'")
    parser.add_argument("-p", "--ignore-parking", dest="ignoreParking", action="store_true", default=False,
                        help="Ignore unordered timing if the vehicle which arrives first is parking")
    parser.add_argument("-P", "--skip-parking", dest="skipParking", action="store_true", default=False,
                        help="Do not generate constraints for a vehicle that parks at the next stop")
    parser.add_argument("--comment.line", action="store_true", dest="commentLine", default=False,
                        help="add lines of involved trains in comment")
    parser.add_argument("--comment.id", action="store_true", dest="commentId", default=False,
                        help="add ids of involved trains in comment (when different from tripId)")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("--debug-switch", dest="debugSwitch",
                        help="print debug information for the given merge-switch edge")
    parser.add_argument("--debug-signal", dest="debugSignal",
                        help="print debug information for the given signal id")
    parser.add_argument("--debug-stop", dest="debugStop",
                        help="print debug information for the given busStop id")

    options = parser.parse_args(args=args)
    if (options.routeFile is None and options.tripFile is None) or options.netFile is None:
        parser.print_help()
        sys.exit()

    if options.routeFile is None:
        options.routeFile = options.tripFile + ".rou.xml"
        args = [DUAROUTER, '-n', options.netFile,
                '-r', options.tripFile,
                '-a', options.addFile,
                '-o', options.routeFile,
                '--ignore-errors', '--no-step-log',
                ]
        print("calling", " ".join(args))
        sys.stdout.flush()
        subprocess.call(args)
        sys.stdout.flush()

    options.delay = parseTime(options.delay)

    return options


def getTravelTime(net, edges):
    result = 0
    for edgeID in edges:
        edge = net.getEdge(edgeID)
        result += edge.getLength() / edge.getSpeed()
    return result


def getStopEdges(addFile):
    """find edge for each stopping place"""
    stopEdges = {}
    for busstop in sumolib.xml.parse(addFile, 'busStop'):
        stopEdges[busstop.id] = sumolib._laneID2edgeID(busstop.lane)

    print("read %s busStops on %s edges" % (len(stopEdges), len(set(stopEdges.values()))))
    return stopEdges


def getStopRoutes(options, stopEdges):
    """parse routes and determine the list of edges between stops
        return: setOfUniqueRoutes, busstopDict
    """
    uniqueRoutes = set()
    stopRoutes = defaultdict(list)  # busStop -> [(edges, stopObj), ....]
    vehicleStopRoutes = defaultdict(list)  # vehID -> [(edges, stopObj), ....]
    numRoutes = 0
    numStops = 0
    begin = parseTime(options.begin)
    for vehicle in sumolib.xml.parse(options.routeFile, 'vehicle', heterogeneous=True):
        depart = parseTime(vehicle.depart)
        if depart < begin:
            continue
        numRoutes += 1
        edges = tuple(vehicle.route[0].edges.split())
        uniqueRoutes.add(edges)
        lastIndex = -1
        routeIndex = 0
        tripId = vehicle.id
        line = vehicle.getAttributeSecure("line", "")
        for stop in vehicle.stop:
            numStops += 1
            if stop.busStop is None:
                stop.setAttribute("busStop", stop.lane)
                stopEdges[stop.lane] = sumolib._laneID2edgeID(stop.lane)
            stopEdge = stopEdges[stop.busStop]
            while edges[routeIndex] != stopEdge:
                routeIndex += 1
                assert(routeIndex < len(edges))
            edgesBefore = edges[lastIndex + 1: routeIndex + 1]
            stop.setAttribute("prevTripId", tripId)
            stop.setAttribute("prevLine", line)
            stop.setAttribute("vehID", vehicle.id)
            tripId = stop.getAttributeSecure("tripId", tripId)
            line = stop.getAttributeSecure("line", line)
            stopRoutes[stop.busStop].append((edgesBefore, stop))
            vehicleStopRoutes[vehicle.id].append((edgesBefore, stop))
            lastIndex = routeIndex

    print("read %s routes (%s unique) and %s stops at %s busStops" % (
        numRoutes, len(uniqueRoutes), numStops, len(stopRoutes)))

    return uniqueRoutes, stopRoutes, vehicleStopRoutes


def findMergingSwitches(options, uniqueRoutes, net):
    """find switches where routes merge and thus conflicts must be solved"""
    predEdges = defaultdict(set)
    predReversal = set()
    for edges in uniqueRoutes:
        for i, edge in enumerate(edges):
            if i > 0:
                pred = edges[i - 1]
                if net.getEdge(edge).getBidi() == net.getEdge(pred):
                    predReversal.add(edge)
                predEdges[edge].add(pred)

    mergeSwitches = set()
    numReversals = 0
    for edge, preds in predEdges.items():
        if len(preds) > 1:
            if edge in predReversal:
                numReversals += 1
            if options.verbose:
                print("mergingEdge=%s pred=%s" % (edge, ','.join(preds)))
            mergeSwitches.add(edge)

    if numReversals == 0:
        reversalInfo = ""
    else:
        reversalInfo = " (including %s reversal-merges)" % numReversals
    print("processed %s routes across %s edges with %s merging switches%s" % (
        len(uniqueRoutes), len(predEdges), len(mergeSwitches), reversalInfo))
    return mergeSwitches


def findStopsAfterMerge(net, stopRoutes, mergeSwitches):
    """find stops at the same busStop that come directly after a the same merge switch (no prior step before
    the switch). Returns filtered stopRoutes"""
    switchRoutes = defaultdict(lambda: defaultdict(list))  # mergeSwitch -> busStop -> [(edges, stopObj), ....]
    mergeSignals = {}  # (switch, edges) -> signal
    numFound = 0
    for busStop, stops in stopRoutes.items():
        for edgesBefore, stop in stops:
            signal = None
            signalEdgeIndex = 0
            prevEdge = None
            for i, edge in enumerate(edgesBefore):
                node = net.getEdge(edge).getFromNode()
                if node.getType() == "rail_signal":
                    tls = net.getTLS(node.getID())
                    for inLane, outLane, _ in tls.getConnections():
                        if (outLane.getEdge().getID() == edge
                                and (prevEdge is None or prevEdge == inLane.getEdge().getID())):
                            signal = tls.getID()
                            signalEdgeIndex = i
                            break
                if edge in mergeSwitches:
                    numFound += 1
                    switchRoutes[edge][busStop].append((edgesBefore, stop))
                    if signal is None:
                        print("No signal found when approaching stop %s via switch %s along route %s" % (
                            busStop, edge, ','.join(edgesBefore)), file=sys.stderr)
                    # travel time from signal to stop
                    ttSignalStop = getTravelTime(net, edgesBefore[signalEdgeIndex:])
                    mergeSignals[(edge, edgesBefore)] = (signal, ttSignalStop)
                prevEdge = edge

    print("Found %s stops after merging switches and %s signals that guard switches" % (
        numFound, len(set(mergeSignals.values()))))
    return switchRoutes, mergeSignals


def computeSignalTimes(options, net, stopRoutes):
    signalTimes = defaultdict(list)  # signal -> [(timeAtSignal, stop), ...]
    debugInfo = []
    for _, stops in stopRoutes.items():
        for edgesBefore, stop in stops:
            if stop.hasAttribute("arrival"):
                arrival = parseTime(stop.arrival)
            elif stop.hasAttribute("until"):
                arrival = parseTime(stop.until) - parseTime(stop.getAttributeSecure("duration", "0"))
            else:
                continue
            for i, edge in enumerate(edgesBefore):
                node = net.getEdge(edge).getFromNode()
                if node.getType() == "rail_signal":
                    tls = net.getTLS(node.getID())
                    for _, outLane, __ in tls.getConnections():
                        if outLane.getEdge().getID() == edge:
                            signal = tls.getID()
                            ttSignalStop = getTravelTime(net, edgesBefore[i:])
                            timeAtSignal = arrival - ttSignalStop
                            signalTimes[signal].append((timeAtSignal, stop))
                            if signal == options.debugSignal:
                                debugInfo.append((timeAtSignal,
                                                  ("%s vehID=%s prevTripId=%s passes signal %s to stop %s " +
                                                   "arrival=%s ttSignalStop=%s edges=%s") % (
                                                      humanReadableTime(timeAtSignal),
                                                      stop.vehID, stop.prevTripId,
                                                      signal, stop.busStop,
                                                      humanReadableTime(arrival), ttSignalStop,
                                                      edgesBefore)))
                            break
    for signal in signalTimes.keys():
        signalTimes[signal] = sorted(signalTimes[signal])

    if options.debugSignal in signalTimes:
        for _, info in sorted(debugInfo):
            print(info)
        busStops = set([s.busStop for a, s in signalTimes[options.debugSignal]])
        arrivals = [a for a, s in signalTimes[options.debugSignal]]
        print("Signal %s is passed %s times between %s and %s on approach to stops %s" % (
            options.debugSignal, len(arrivals), arrivals[0], arrivals[-1], ' '.join(busStops)))

    return signalTimes


def countPassingTrainsToOtherStops(options, signal, currentStop, begin, end, signalTimes):
    count = 0
    for timeAtSwitch, stop in signalTimes[signal]:
        if timeAtSwitch < begin:
            continue
        if timeAtSwitch > end:
            break
        if stop.busStop == currentStop:
            continue
        count += 1
        if options.debugSignal == signal:
            print("vehicle %s (%s) passes signal %s at time %s (in between %s and %s)" % (
                stop.prevTripId, stop.vehID, signal, timeAtSwitch, begin, end))
    return count


def findConflicts(options, switchRoutes, mergeSignals, signalTimes):
    """find stops that target the same busStop from different branches of the
    prior merge switch and establish their ordering"""

    numConflicts = 0
    numIgnoredConflicts = 0
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    conflicts = defaultdict(list)
    ignoredVehicles = set()
    for switch, stopRoutes2 in switchRoutes.items():
        numSwitchConflicts = 0
        numIgnoredSwitchConflicts = 0
        if switch == options.debugSwitch:
            print("Switch %s lies ahead of busStops %s" % (switch, stopRoutes2.keys()))
        for busStop, stops in stopRoutes2.items():
            arrivals = []
            for edges, stop in stops:
                if stop.hasAttribute("arrival"):
                    arrival = parseTime(stop.arrival)
                elif stop.hasAttribute("until"):
                    arrival = parseTime(stop.until) - parseTime(stop.getAttributeSecure("duration", "0"))
                else:
                    print("ignoring stop at %s without schedule information (arrival, until)" % busStop)
                    continue
                arrivals.append((arrival, edges, stop))
            arrivals.sort(key=itemgetter(0))
            ignore = False
            for (pArrival, pEdges, pStop), (nArrival, nEdges, nStop) in zip(arrivals[:-1], arrivals[1:]):
                pSignal, pTimeSiSt = mergeSignals[(switch, pEdges)]
                nSignal, nTimeSiSt = mergeSignals[(switch, nEdges)]
                if switch == options.debugSwitch:
                    print(pSignal, nSignal, pStop, nStop)
                if pSignal != nSignal and pSignal is not None and nSignal is not None:
                    if (ignore or (options.abortUnordered
                                   and pStop.hasAttribute("until")
                                   and nStop.hasAttribute("until")
                                   and (not options.ignoreParking or not
                                        parseBool(pStop.getAttributeSecure("parking", "false")))
                                   and parseTime(pStop.until) > parseTime(nStop.until))):
                        numIgnoredConflicts += 1
                        numIgnoredSwitchConflicts += 1
                        # ignore conflict and any that follow
                        if not ignore:
                            print("Found inconsistent times at stop %s "
                                  "for vehicle %s (%s, %s) and vehicle %s (%s, %s)" % (
                                      busStop,
                                      pStop.vehID,
                                      humanReadableTime(pArrival),
                                      humanReadableTime(parseTime(pStop.until)),
                                      nStop.vehID,
                                      humanReadableTime(nArrival),
                                      humanReadableTime(parseTime(nStop.until))),
                                  file=sys.stderr)
                            # ignoredVehicles.insert(pStop.vehID)
                            ignoredVehicles.add(nStop.vehID)
                        ignore = True
                        continue
                    if (options.abortUnordered and nStop.vehID in ignoredVehicles):
                        # no constraints for inconsistent vehicle to avoid deadlock
                        numIgnoredConflicts += 1
                        numIgnoredSwitchConflicts += 1
                        continue
                    if options.skipParking and parseBool(nStop.getAttributeSecure("parking", "false")):
                        print("ignoring stop at %s for parking vehicle %s (%s, %s)" % (
                            busStop, nStop.vehID, humanReadableTime(nArrival),
                            (humanReadableTime(parseTime(nStop.until)) if nStop.hasAttribute("until") else "-")))
                        numIgnoredConflicts += 1
                        numIgnoredSwitchConflicts += 1
                        continue
                    if options.skipParking and parseBool(pStop.getAttributeSecure("parking", "false")):
                        print("ignoring stop at %s for %s (%s, %s) after parking vehicle %s (%s, %s)" % (
                            busStop, nStop.vehID, humanReadableTime(nArrival),
                            (humanReadableTime(parseTime(nStop.until)) if nStop.hasAttribute("until") else "-"),
                            pStop.vehID, humanReadableTime(pArrival),
                            (humanReadableTime(parseTime(pStop.until)) if pStop.hasAttribute("until") else "-")))
                        numIgnoredConflicts += 1
                        numIgnoredSwitchConflicts += 1
                        continue

                    numConflicts += 1
                    numSwitchConflicts += 1
                    # check for trains that pass the switch in between the
                    # current two trains (heading to another stop) and raise the limit
                    limit = 1
                    pTimeAtSignal = pArrival - pTimeSiSt
                    nTimeAtSignal = nArrival - nTimeSiSt
                    end = nTimeAtSignal + options.delay
                    if options.verbose and options.debugSignal == pSignal:
                        print("check vehicles between %s and %s (including delay %s) at signal %s pStop=%s nStop=%s" % (
                            humanReadableTime(pTimeAtSignal),
                            humanReadableTime(end), options.delay, pSignal,
                            pStop, nStop))
                    limit += countPassingTrainsToOtherStops(options, pSignal, busStop, pTimeAtSignal, end, signalTimes)
                    conflicts[nSignal].append((nStop.prevTripId, pSignal, pStop.prevTripId, limit,
                                               # attributes for adding comments
                                               nStop.prevLine, pStop.prevLine, nStop.vehID, pStop.vehID))
        if options.verbose:
            print("Found %s conflicts at switch %s" % (numSwitchConflicts, switch))
            if numIgnoredSwitchConflicts > 0:
                print("Ignored %s conflicts at switch %s" % (numIgnoredSwitchConflicts, switch))

    print("Found %s conflicts" % numConflicts)
    if numIgnoredConflicts > 0:
        print("Ignored %s conflicts" % numIgnoredConflicts)
    return conflicts


def findSignal(net, nextEdges):
    prevEdge = None
    for edge in nextEdges:
        node = net.getEdge(edge).getFromNode()
        if node.getType() == "rail_signal":
            tls = net.getTLS(node.getID())
            for inLane, outLane, _ in tls.getConnections():
                if (outLane.getEdge().getID() == edge
                        and (prevEdge is None or prevEdge == inLane.getEdge().getID())):
                    return tls.getID()
        prevEdge = edge
    return None


def findInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes):
    """find routes that start at a stop with a traffic light at end of the edge
    and routes that pass this stop. Ensure
    insertion happens in the correct order"""
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    conflicts = defaultdict(list)
    numConflicts = 0
    numIgnoredConflicts = 0
    for busStop, stops in stopRoutes.items():
        stopEdge = stopEdges[busStop]
        node = net.getEdge(stopEdge).getToNode()
        signal = node.getID()
        untils = []
        for edgesBefore, stop in stops:
            if stop.hasAttribute("until") and not options.untilFromDuration:
                until = parseTime(stop.until)
            elif stop.hasAttribute("arrival"):
                until = parseTime(stop.arrival) + parseTime(stop.getAttributeSecure("duration", "0"))
            else:
                continue
            untils.append((until, edgesBefore, stop))
        # only use 'until' for sorting and keep the result stable otherwise
        untils.sort(key=itemgetter(0))
        prevPassing = None
        ignore = False
        for i, (nUntil, nEdges, nStop) in enumerate(untils):
            nVehStops = vehicleStopRoutes[nStop.vehID]
            nIndex = nVehStops.index((nEdges, nStop))
            nIsPassing = nIndex < len(nVehStops) - 1
            nIsDepart = len(nEdges) == 1
            if options.verbose and busStop == options.debugStop:
                print(i,
                      "n:", humanReadableTime(nUntil), nStop.tripId, nStop.vehID, nIndex, len(nVehStops),
                      "passing:", nIsPassing,
                      "depart:", nIsDepart)
            if prevPassing is not None and nIsDepart:
                pUntil, pEdges, pStop = prevPassing
                pVehStops = vehicleStopRoutes[pStop.vehID]
                pIndex = pVehStops.index((pEdges, pStop))
                # no need to constrain subsequent departures (simulation should maintain ordering)
                if len(pEdges) > 1 or pIndex > 0:
                    # find edges after stop
                    if busStop == options.debugStop:
                        print(i,
                              "p:", humanReadableTime(pUntil), pStop.tripId, pStop.vehID, pIndex, len(pVehStops),
                              "n:", humanReadableTime(nUntil), nStop.tripId, nStop.vehID, nIndex, len(nVehStops))
                    if nIsPassing:
                        # both vehicles move past the stop
                        pNextEdges = pVehStops[pIndex + 1][0]
                        nNextEdges = nVehStops[nIndex + 1][0]
                        limit = 1  # recheck
                        pSignal = signal
                        nSignal = signal
                        if node.getType() != "rail_signal":
                            # find signal in nextEdges
                            pSignal = findSignal(net, pNextEdges)
                            nSignal = findSignal(net, nNextEdges)
                            if pSignal is None or nSignal is None:
                                print(("Ignoring insertion conflict between %s and %s at stop '%s' " +
                                       "because no rail signal was found after the stop") % (
                                    nStop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                                continue
                        # check for inconsistent ordering
                        if (ignore or (options.abortUnordered
                                       and pStop.hasAttribute("arrival")
                                       and nStop.hasAttribute("arrival")
                                       and (not options.ignoreParking or not
                                            parseBool(nStop.getAttributeSecure("parking", "false")))
                                       and parseTime(pStop.arrival) > parseTime(nStop.arrival))):
                            numIgnoredConflicts += 1
                            # ignore conflict and any that follow
                            if not ignore:
                                # sort output by arrival again
                                print("Found inconsistent times at stop %s "
                                      "for vehicle %s (%s, %s) and vehicle %s (%s, %s)" % (
                                          busStop,
                                          nStop.vehID,
                                          humanReadableTime(parseTime(nStop.arrival)),
                                          humanReadableTime(nUntil),
                                          pStop.vehID,
                                          humanReadableTime(parseTime(pStop.arrival)),
                                          humanReadableTime(pUntil)),
                                      file=sys.stderr)
                            ignore = True
                            continue
                        # predecessor tripId after stop is needed
                        pTripId = pStop.getAttributeSecure("tripId", pStop.vehID)
                        conflicts[nSignal].append((nStop.prevTripId, pSignal, pTripId, limit,
                                                   # attributes for adding comments
                                                   nStop.prevLine, pStop.prevLine, nStop.vehID, pStop.vehID))
                        numConflicts += 1
                        if busStop == options.debugStop:
                            print("   found insertionConflict pSignal=%s nSignal=%s pTripId=%s" % (
                                pSignal, nSignal, pTripId)),

            if nIsPassing:
                prevPassing = (nUntil, nEdges, nStop)

    print("Found %s insertion conflicts" % numConflicts)
    if numIgnoredConflicts > 0:
        print("Ignored %s insertion conflicts" % (numIgnoredConflicts))
    return conflicts


def writeConstraint(options, outf, tag, values):
    tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID = values
    comment = ""
    if options.commentLine:
        if line != "":
            comment += "line=%s " % line
        if otherLine != "":
            comment += "foeLine=%s " % otherLine
    if options.commentId:
        if vehID != tripID:
            comment += "vehID=%s " % vehID
        if otherVehID != otherTripID:
            comment += "foeID=%s " % otherVehID
    limit += options.limit
    if comment != "":
        comment = "   <!-- %s -->" % comment
    if limit == 1:
        limit = ""
    else:
        limit = ' limit="%s"' % limit
    outf.write('        <%s tripId="%s" tl="%s" foes="%s"%s/>%s\n' % (
        tag, tripID, otherSignal, otherTripID, limit, comment))


def main(options):
    net = sumolib.net.readNet(options.netFile)
    stopEdges = getStopEdges(options.addFile)
    uniqueRoutes, stopRoutes, vehicleStopRoutes = getStopRoutes(options, stopEdges)
    mergeSwitches = findMergingSwitches(options, uniqueRoutes, net)
    signalTimes = computeSignalTimes(options, net, stopRoutes)
    switchRoutes, mergeSignals = findStopsAfterMerge(net, stopRoutes, mergeSwitches)
    conflicts = findConflicts(options, switchRoutes, mergeSignals, signalTimes)
    insertionConflicts = findInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes)

    signals = sorted(set(list(conflicts.keys()) + list(insertionConflicts.keys())))
    with open(options.out, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional")  # noqa
        for signal in signals:
            outf.write('    <railSignalConstraints id="%s">\n' % signal)
            for values in conflicts[signal]:
                writeConstraint(options, outf, "predecessor", values)
            for values in insertionConflicts[signal]:
                writeConstraint(options, outf, "insertionPredecessor", values)
            outf.write('    </railSignalConstraints>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
