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

The schedule can either be based on 'arrival' or 'until' time of stops.
Vehicles that pass the same switch from different branches will be sorted based
on their arrival at the subsequent stop.
"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime  # noqa

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
    #parser.add_argument("--arrivals", action="store_true", default=False,
    #                    help="Use stop arrival time instead of 'until' time for sorting")
    #parser.add_argument("-p", "--ignore-parking", dest="ignoreParking", action="store_true", default=False,
    #                    help="Do not create constraints after a parking stop")
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
    stopRoutes = defaultdict(list) # busStop -> [(edges, stopObj), ....]
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
            lastIndex = routeIndex

    print("read %s routes (%s unique) and %s stops at %s busStops" % (
        numRoutes, len(uniqueRoutes), numStops, len(stopRoutes)))

    return uniqueRoutes, stopRoutes

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
    switchRoutes = defaultdict(lambda : defaultdict(list)) # mergeSwitch -> busStop -> [(edges, stopObj), ....]
    mergeSignals = {} # (switch, edges) -> signal
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
                    for inLane, outLane, linkNo in tls.getConnections():
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
    signalTimes = defaultdict(list) # signal -> [(timeAtSignal, stop), ...]
    for busStop, stops in stopRoutes.items():
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
                    for inLane, outLane, linkNo in tls.getConnections():
                        if outLane.getEdge().getID() == edge:
                            signal = tls.getID()
                            ttSignalStop = getTravelTime(net, edgesBefore[i:])
                            timeAtSignal = arrival - ttSignalStop
                            signalTimes[signal].append((timeAtSignal, stop))
                            if signal == options.debugSignal:
                                print("Route past signal %s to stop %s arrival=%s ttSignalStop=%s timeAtSignal=%s edges=%s" % (
                                    signal, stop.busStop, arrival, ttSignalStop, timeAtSignal, edgesBefore))
                            break
    for signal in signalTimes.keys():
        signalTimes[signal] = sorted(signalTimes[signal])

    if options.debugSignal in signalTimes:
        busStops = set([s.busStop for a, s in signalTimes[options.debugSignal]])
        arrivals = [a for a,s in signalTimes[options.debugSignal]]
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
    conflicts = defaultdict(list) # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    for switch, stopRoutes2 in switchRoutes.items():
        numSwitchConflicts = 0
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
            arrivals.sort()
            for (pArrival, pEdges, pStop), (nArrival, nEdges, nStop) in zip(arrivals[:-1], arrivals[1:]):
                pSignal, pTimeSiSt = mergeSignals[(switch, pEdges)]
                nSignal, nTimeSiSt = mergeSignals[(switch, nEdges)]
                if switch == options.debugSwitch:
                    print(pSignal, nSignal, pStop, nStop)
                if pSignal != nSignal and pSignal is not None and nSignal is not None:
                    numConflicts += 1
                    numSwitchConflicts += 1
                    # check for trains that pass the switch in between the
                    # current two trains (heading to another stop) and raise the limit
                    limit = 1
                    pTimeAtSignal = pArrival - pTimeSiSt
                    nTimeAtSignal = nArrival - nTimeSiSt
                    limit += countPassingTrainsToOtherStops(options, pSignal, busStop, pTimeAtSignal, nTimeAtSignal, signalTimes)
                    conflicts[nSignal].append((nStop.prevTripId, pSignal, pStop.prevTripId, limit,
                        # attributes for adding comments
                        nStop.prevLine, pStop.prevLine, nStop.vehID, pStop.vehID))
        if options.verbose:
            print("Found %s conflicts at switch %s" % (numSwitchConflicts, switch))

    print("Found %s conflicts" % numConflicts)
    return conflicts

def findInsertionConflicts(options, net, stopEdges, stopRoutes):
    """find routes that start at a stop with a traffic light at end of the edge
    and routes that pass this stop. Ensure
    insertion happens in the correct order"""
    conflicts = defaultdict(list) # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    numConflicts = 0
    for busStop, stops in stopRoutes.items():
        stopEdge = stopEdges[busStop]
        node = net.getEdge(stopEdge).getToNode()
        signal = node.getID()
        arrivals = []
        for edgesBefore, stop in stops:
            if stop.hasAttribute("arrival"):
                arrival = parseTime(stop.arrival)
            elif stop.hasAttribute("until"):
                arrival = parseTime(stop.until) - parseTime(stop.getAttributeSecure("duration", "0"))
            else:
                continue
            arrivals.append((arrival, edgesBefore, stop))
        arrivals.sort()
        for (pArrival, pEdges, pStop), (nArrival, nEdges, nStop) in zip(arrivals[:-1], arrivals[1:]):
            if len(nEdges) == 1 and len(pEdges) > 1:
                limit = 1 # recheck
                if node.getType() == "rail_signal":
                    conflicts[signal].append((nStop.prevTripId, signal, pStop.prevTripId, limit,
                        # attributes for adding comments
                        nStop.prevLine, pStop.prevLine, nStop.vehID, pStop.vehID))
                    numConflicts += 1
                elif options.verbose:
                    print("Ignoring insertion conflict between %s and %s at stop '%s' which has no rail signal after it's edge" % (
                                nStop.prevTripId, pStop.prevTripId, busStop))

    print("Found %s insertion conflicts" % numConflicts)
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
    uniqueRoutes, stopRoutes = getStopRoutes(options, stopEdges)
    mergeSwitches = findMergingSwitches(options, uniqueRoutes, net)
    signalTimes = computeSignalTimes(options, net, stopRoutes)
    switchRoutes, mergeSignals = findStopsAfterMerge(net, stopRoutes, mergeSwitches)
    conflicts = findConflicts(options, switchRoutes, mergeSignals, signalTimes)
    insertionConflicts = findInsertionConflicts(options, net, stopEdges, stopRoutes)

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
