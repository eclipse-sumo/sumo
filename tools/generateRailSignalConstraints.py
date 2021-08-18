#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
(Zuglenkung)

Two types of constraints are generated in different cases:
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

3. <predecessor>
Whenever a vehicle A departs at a stop (assumed to coincide with the "arrival"
attribute of it's first stop), the latter train B that enters this stop is
identified (also based on "arrival"). Then a constraint is created that prevents
B from entering the section with the stop until A has passed the next signal that lies beyond the
stop.

== Inconsistencies ==

Inconsistent contraints may arise from inconsistent input and cause simulation
deadlock. To avoid this, the option --abort-unordered can be used to avoid
generating constraints that are likely to be inconsistent.
When the option is set the ordering of vehicles is cross-checked with regard to
arrival and until times:

Given two vehicles A and B which stop at the same location, if A arrives at the
stop later than B, but A also leaves earlier than B, then B is "overtaken" by A.
The stop of A and all subsequent stops of A are marked as invalid and will not
participate in constraint generation.

If two vehicles have a 'parking'-stop with the same 'until' time at the same
location, their stops will also be marked as invalid since the simulation cannot
enforce an order in this case (and local desired order is ambiguous).

== Post-Facto Stop Timings ==

When simulating the past (i.e. to predict the future), additional timing data
besides the scheduled arrival and until times may be available and included in
the 'started' and 'ended' attributes for each stop.
They can be used to detect changes in train order that occured during the actual
train operation and which must be taken into account during constraint
generation to avoid deadlock.
If train A has 'started' information for a
stop while train B has not, this implies that A has reached the stop ahead of B.
Likewise, both trains may have 'started' information but in the reverse order
compared to the schedule.
For all stops with complete started,ended information, those times can be used
as an updated schedule (replacing arrival and until). However, if an order
reversal was detected for a train, no constraints based on the old schedule
should be generated anymore (stops are ignored after started,ended information ends)


== Further Options ==
If constraints shall be modified during the simulation (traci.trafficlight.swapConstraints)
it may be useful to add additional constraints which would otherwise be
redundant. This can be accomplished by setting option --redundant with a time
range. When set, trains that follow a constrained train within the given time
range (and which would normally be constrained implicitly by their leading
train) will also receive a constraint. In this case option --limit must be used
to ensure that all constraint foe vehicles are recorded during the simulation.

"""

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
from collections import defaultdict
from operator import itemgetter
import copy

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
    parser.add_argument("--redundant", default=-1, help="Add redundant constraint within given time range " +
                                                        "(reduces impact of modifying constraints at runtime)")
    parser.add_argument("--comment.line", action="store_true", dest="commentLine", default=False,
                        help="add lines of involved trains in comment")
    parser.add_argument("--comment.id", action="store_true", dest="commentId", default=False,
                        help="add ids of involved trains in comment (when different from tripId)")
    parser.add_argument("--comment.switch", action="store_true", dest="commentSwitch", default=False,
                        help="add id of the merging switch that prompted the constraint")
    parser.add_argument("--comment.stop", action="store_true", dest="commentStop", default=False,
                        help="add busStop id that was used to determine the train ordering for the constraint")
    parser.add_argument("--comment.time", action="store_true", dest="commentTime", default=False,
                        help="add timing information for the constraint")
    parser.add_argument("--comment.all", action="store_true", dest="commentAll", default=False,
                        help="add all comments")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("--debug-switch", dest="debugSwitch",
                        help="print debug information for the given merge-switch edge")
    parser.add_argument("--debug-signal", dest="debugSignal",
                        help="print debug information for the given signal id")
    parser.add_argument("--debug-stop", dest="debugStop",
                        help="print debug information for the given busStop id")
    parser.add_argument("--debug-vehicle", dest="debugVehicle",
                        help="print debug information for the given vehicle id")

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

    if options.commentAll:
        options.commentLine = True
        options.commentId = True
        options.commentSwitch = True
        options.commentStop = True
        options.commentTime = True

    options.delay = parseTime(options.delay)
    options.redundant = parseTime(options.redundant)

    return options

def formatStopTimes(arrival, until, started, ended):
    times = [arrival, until]
    if started is not None:
        times.append(started)
    if ended is not None:
        assert(started is not None)
        times.append(ended)
    return "(%s)" % ', '.join(map(humanReadableTime, times))

class Conflict:
    def __init__(self, tripID, otherSignal, otherTripID, limit, line, otherLine,
                 vehID, otherVehID, conflictTime, switch, busStop, info):
        self.tripID = tripID
        self.otherSignal = otherSignal
        self.otherTripID = otherTripID
        self.limit = limit
        self.line = line
        self.otherLine = otherLine
        self.vehID = vehID
        self.otherVehID = otherVehID
        self.conflictTime = conflictTime
        self.switch = switch
        self.busStop = busStop
        self.info = info


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
        if vehicle.param is not None:
            for param in vehicle.param:
                if param.key == "tripId":
                    tripId = param.value
        line = vehicle.getAttributeSecure("line", "")
        for stop in vehicle.stop:
            numStops += 1
            if stop.busStop is None:
                if stop.edge is None:
                    stop.setAttribute("busStop", stop.lane)
                    stopEdges[stop.lane] = sumolib._laneID2edgeID(stop.lane)
                else:
                    stop.setAttribute("busStop", stop.edge)
                    stopEdges[stop.edge] = stop.edge
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
            reversalInfo = ""
            if edge in predReversal:
                numReversals += 1
                reversalInfo = " (has reversal)"
            if options.verbose:
                print("mergingEdge=%s pred=%s%s" % (
                    edge, ','.join(sorted(preds)), reversalInfo))
            mergeSwitches.add(edge)

    if numReversals == 0:
        reversalInfo = ""
    else:
        reversalInfo = " (including %s reversal-merges)" % numReversals
    print("processed %s routes across %s edges with %s merging switches%s" % (
        len(uniqueRoutes), len(predEdges), len(mergeSwitches), reversalInfo))
    return mergeSwitches


def findStopsAfterMerge(net, stopRoutes, mergeSwitches):
    """find stops at the same busStop that come directly after a the same merge switch (no prior stop before
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


def markOvertaken(options, vehicleStopRoutes, stopRoutes):
    """
    mark stops that should not participate in constraint generation
    once a vehice appears to be "overtaken" (based on inconsistent
    arrival/until timing), all subsequent stops of that vehicle should no
    longer be used for contraint generation
    """
    for vehicle, stopRoute in vehicleStopRoutes.items():
        overtaken = False
        ignored = False
        for i, (edgesBefore, stop) in enumerate(stopRoute):
            if not (stop.hasAttribute("arrival") and stop.hasAttribute("until")):
                continue
            parking = parseBool(stop.getAttributeSecure("parking", "false"))
            if not overtaken:
                arrival = parseTime(stop.arrival)
                until = parseTime(stop.until)
                started = parseTime(stop.started) if stop.hasAttribute("started") else None
                ended = parseTime(stop.ended) if stop.hasAttribute("ended") else None
                for edgesBefore2, stop2 in stopRoutes[stop.busStop]:
                    if stop2.vehID == stop.vehID:
                        continue
                    if not stop2.hasAttribute("arrival") or not stop2.hasAttribute("until"):
                        continue
                    parking2 = parseBool(stop2.getAttributeSecure("parking", "false"))
                    hasParking = parking or parking2
                    arrival2 = parseTime(stop2.arrival)
                    until2 = parseTime(stop2.until)
                    started2 = parseTime(stop2.started) if stop2.hasAttribute("started") else None
                    ended2 = parseTime(stop2.ended) if stop2.hasAttribute("ended") else None
                    # if parking stops have the same until-time their depart order
                    # is undefined so we could get deadlocks
                    if options.skipParking and hasParking and until != until2:
                        continue
                    if (arrival2 > arrival and (
                            # legacy: until replaced by ended
                            until2 < until or
                            (started2 is not None
                                # vehicle had not arrived but it's schedule follower had
                                and (started is None
                                    # vehicle arrived after schedule follower
                                    or started2 < started)))):
                        overtaken = True
                        ignored = started is None
                        ignoredInfo = " and ignored afterwards" if started is None else ""
                        print(("Vehicle %s %s overtaken by %s %s " +
                               "at stop %s (index %s)%s") %
                              (stop.vehID, formatStopTimes(arrival, until, started, ended),
                               stop2.vehID, formatStopTimes(arrival2, until2, started2, ended2),
                               stop.busStop, i, ignoredInfo),
                              file=sys.stderr)
                        break
                    elif hasParking and (until == until2 or (ended is not None and ended == ended2)):
                        overtaken = True
                        ignored = True
                        if stop.vehID < stop2.vehID:
                            # only warn once
                            print(("Ambiguous departure order at stop %s" +
                                   " (index %s) for %svehicle %s %s" +
                                   " and %svehicle %s %s." +
                                   " No constraints will be generated for them afterwards") % (
                                stop.busStop, i,
                                'parking ' if parking else ' ',
                                stop.vehID, formatStopTimes(arrival, until, started, ended),
                                'parking ' if parking2 else ' ',
                                stop2.vehID,formatStopTimes(arrival2, until2, started2, ended2),
                            ),
                                file=sys.stderr)
                        break

            if overtaken:
                # print("invalid veh=%s stop=%s arrival=%s until=%s" %
                #        (stop.vehID, stop.busStop,
                #            humanReadableTime(parseTime(stop.arrival)),
                #            humanReadableTime(parseTime(stop.until))))
                if not stop.hasAttribute("started"):
                    stop.setAttribute("invalid", True)
                    if not ignored:
                        print("Vehicle %s was overtaken and starts to ignore schedule at stop %s (index %s)" %
                            (stop.vehID, stop.busStop, i),
                            file=sys.stderr)
                        ignored = True

def updateStartedEnded(options, net, stopEdges, stopRoutes, vehicleStopRoutes):
    """
    replace arrival,until information with started,ended
    At stops with parking=true, additional constraints are needed so that
    vehicles without 'ended' delay their re-insertion until all vehicles with 'ended' have passed
    """
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    limit = 1 + options.limit
    conflicts = defaultdict(list)
    numConflicts = 0

    maxShift = 0
    for busStop, stops in stopRoutes.items():
        latestKnownTime = 0
        shift = 0
        stopEdge = stopEdges[busStop]
        node = net.getEdge(stopEdge).getToNode()
        parkingEnded = []

        for edgesBefore, stop in stops:
            if stop.hasAttribute("ended") and stop.hasAttribute("until"):
                ended = parseTime(stop.ended)
                if ended > latestKnownTime:
                    latestKnownTime = ended
                    shift = max(shift, ended - parseTime(stop.until))

                if parseBool(stop.getAttributeSecure("parking", "false")):
                    vehStops = vehicleStopRoutes[stop.vehID]
                    index = vehStops.index((edgesBefore, stop))
                    isPassing = index < len(vehStops) - 1
                    if isPassing:
                        # prevent overtaking by a vehicle without "ended" and earlier "until"
                        nextEdges = vehStops[index + 1][0]
                        parkingEnded.append((ended, stop, nextEdges))

            elif stop.hasAttribute("started") and stop.hasAttribute("arrival"):
                started = parseTime(stop.started)
                if started > latestKnownTime:
                    latestKnownTime = started
                    shift = max(shift, started - parseTime(stop.arrival))

        maxShift = max(maxShift, shift)
        parkingEnded.sort(key=itemgetter(0))

        for edgesBefore, stop in stops:
            if stop.hasAttribute("started"):
                stop.arrival = stop.started
            elif stop.hasAttribute("arrival"):
                stop.arrival = str(parseTime(stop.arrival) + shift)
            if stop.hasAttribute("ended"):
                stop.until = stop.ended
            elif stop.hasAttribute("until"):
                stop.until = str(parseTime(stop.until) + shift)

                if len(parkingEnded) > 0:
                    ended, pStop, pNextEdges = parkingEnded[-1]

                    vehStops = vehicleStopRoutes[stop.vehID]
                    index = vehStops.index((edgesBefore, stop))
                    isPassing = index < len(vehStops) - 1
                    if isPassing:
                        nNextEdges = vehStops[index + 1][0]

                        if parseBool(stop.getAttributeSecure("parking", "false")):
                            # we need an insertion constraint for insertion after parking

                            # find signal in nextEdges
                            pSignal = findSignal(net, (stopEdge,) + pNextEdges)
                            nSignal = findSignal(net, (stopEdge,) + nNextEdges)
                            if pSignal is None or nSignal is None:
                                print(("Ignoring parking insertion conflict between %s and %s at stop '%s' " +
                                    "because no rail signal was found after the stop") % (
                                        stop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                                continue

                            # vehicles have already stopped so the new tripId applies
                            nTripID = stop.getAttributeSecure("tripId", stop.vehID)
                            pTripID = pStop.getAttributeSecure("tripId", pStop.vehID)
                            conflicts[nSignal].append(Conflict(nTripID, pSignal, pTripID, limit,
                                                               # attributes for adding comments
                                                               stop.line,
                                                               pStop.line,
                                                               stop.vehID,
                                                               pStop.vehID,
                                                               "foeEnded=%s " % humanReadableTime(ended),
                                                               None,  # switch
                                                               stop.busStop,
                                                               "parking"))
                            numConflicts += 1

                        else:
                            # this is a foeInsertionConflicts and we need a normal constraint
                            print(("Warning: constraint for non-parking vehicle '%s'" % stop.vehID)
                                + (" after ended parking vehicle '%s' not implemented yet." % pStop.vehID)
                                + (" Possible deadlock at '%s' " % busStop), file=sys.stderr)

        if busStop == options.debugStop and shift > 0:
            print("Shifted stop times at %s by %s" % (busStop, shift))


    if options.verbose and maxShift > 0:
        print("Shifted stop times by up to %s" % maxShift)

    if options.verbose and numConflicts > 0:
        print("Found %s parking insertion conflicts" % numConflicts)

    return conflicts

def addCommonStop(options, switch, edgesBefore, stop, edgesBefore2, stop2, vehicleStopRoutes, stopRoutes2):
    """ add more items to stopRoutes2 for the common
    busStop but keep the edgeBefore of the original stops
    """
    assert(stop.busStop != stop2.busStop)
    if stop.vehID == stop2.vehID:
        return
    route = vehicleStopRoutes[stop.vehID]
    route2 = vehicleStopRoutes[stop2.vehID]
    stopIndex = route.index((edgesBefore, stop))
    stopIndex2 = route2.index((edgesBefore2, stop2))
    routeIndex = edgesBefore.index(switch)
    routeIndex2 = edgesBefore2.index(switch)
    # advance both routes until the routes diverge, one route ends or a common stop is found
    while stopIndex < len(route) and stopIndex2 < len(route2):
        eb, s = route[stopIndex]
        eb2, s2 = route2[stopIndex2]
        e = eb[routeIndex]
        e2 = eb2[routeIndex2]
        #print(stopIndex, routeIndex, stopIndex2, routeIndex2, len(eb), len(eb2))
        if e != e2:
            # routes diverge
            #print("e=%s e2=%s" % (e, e2))
            return
        if (routeIndex + 1 == len(eb) and
                routeIndex2 + 1 == len(eb2) and
                s.busStop == s2.busStop):
            # found common stop
            #print("switch=%s veh=%s veh2=%s commonStop=%s" % (switch,
            #    stop.vehID, stop2.vehID, s.busStop))
            if (edgesBefore2, s2) not in stopRoutes2[s.busStop]:
                s2copy = copy.copy(s2)
                s2copy.prevTripId = stop2.prevTripId
                s2copy.setAttribute("intermediateStop", stop2.busStop)
                s2copy.setAttribute("otherVeh", stop.vehID)
                stopRoutes2[s.busStop].append((edgesBefore2, s2copy))
            if s.busStop != stop.busStop and ((edgesBefore, s) not in stopRoutes2[s.busStop]):
                scopy = copy.copy(s)
                scopy.prevTripId = stop.prevTripId
                scopy.setAttribute("intermediateStop", stop.busStop)
                scopy.setAttribute("otherVeh", stop2.vehID)
                stopRoutes2[s.busStop].append((edgesBefore, scopy))
            return
        # advance along routes
        if routeIndex + 1 == len(eb):
            routeIndex = 0
            stopIndex += 1
            # skip duplicate stops since they do not add edges
            while stopIndex < len(route) and len(route[stopIndex][0]) == 0:
                stopIndex += 1
        else:
            routeIndex += 1

        if routeIndex2 + 1 == len(eb2):
            routeIndex2 = 0
            stopIndex2 += 1
            while stopIndex2 < len(route2) and len(route2[stopIndex2][0]) == 0:
                stopIndex2 += 1
        else:
            routeIndex2 += 1

    if stop.vehID == options.debugVehicle:
        print(("No common stop found after switch %s for vehicle %s with stop %s (%s, %s)"
                + " and intermediate stop of vehicle %s at %s (%s, %s)") % (
            switch, stop.vehID, stop.busStop,
            humanReadableTime(parseTime(stop.arrival)),
            humanReadableTime(parseTime(stop.until)),
            stop2.vehID, stop2.busStop,
            humanReadableTime(parseTime(stop2.arrival)),
            humanReadableTime(parseTime(stop2.until)),
            ))


def findConflicts(options, switchRoutes, mergeSignals, signalTimes, stopEdges, vehicleStopRoutes):
    """find stops that target the same busStop from different branches of the
    prior merge switch and establish their ordering"""

    numConflicts = 0
    numRedundant = 0
    numIgnoredConflicts = 0
    numIgnoredStops = 0
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    conflicts = defaultdict(list)

    for switch, stopRoutes2 in switchRoutes.items():
        numSwitchConflicts = 0
        numRedundantSwitchConflicts = 0
        numIgnoredSwitchConflicts = 0
        numIgnoredSwitchStops = 0
        if switch == options.debugSwitch:
            print("Switch %s lies ahead of busStops %s" % (switch, stopRoutes2.keys()))


        # detect approaches that skip stops (#8943) and add extra items
        stopRoutes3 = defaultdict(list)
        stopsAfterSwitch = defaultdict(set) # edge -> stops
        for busStop, stops in stopRoutes2.items():
            stopsAfterSwitch[stopEdges[busStop]].add(busStop)
        for busStop, stops in stopRoutes2.items():
            for edgesBefore, stop in stops:
                intermediateStops = set()
                for edge in edgesBefore:
                    for s in stopsAfterSwitch[edge]:
                        if s != busStop:
                            intermediateStops.add(s)
                if len(intermediateStops) != 0:
                    # try to establish an order between vehicles from different arms
                    # of the switch even if their immediate stops differs
                    # this is possible (and necessary) if they have a common stop
                    # and their routes are identical between the merging switch and
                    # the first common stop.
                    # note: the first common stop may differ from all stops in stopRoutes2
                    for busStop2, stops2 in stopRoutes2.items():
                        if busStop2 in intermediateStops:
                            for edgesBefore2, stop2 in stops2:
                                addCommonStop(options, switch, edgesBefore, stop,
                                    edgesBefore2, stop2, vehicleStopRoutes, stopRoutes3)
                    #print("Stop after switch %s at %s by %s (%s, %s) passes intermediate stops %s" % (
                    #    switch, busStop, stop.vehID,
                    #    humanReadableTime(parseTime(stop.arrival)),
                    #    humanReadableTime(parseTime(stop.until)),
                    #    ",".join(intermediateStops)
                    #    ))

        # delay dict merge until all extra stops have been found
        for busStop, stops in stopRoutes2.items():
            stopRoutes3[busStop] += stops

        for busStop, stops in stopRoutes3.items():
            arrivals = []
            for edges, stop in stops:
                if stop.hasAttribute("arrival"):
                    arrival = parseTime(stop.arrival)
                elif stop.hasAttribute("until"):
                    arrival = parseTime(stop.until) - parseTime(stop.getAttributeSecure("duration", "0"))
                else:
                    print("ignoring stop at %s without schedule information (arrival, until)" % busStop)
                    continue
                if stop.getAttributeSecure("invalid", False):
                    numIgnoredSwitchStops += 1
                    numIgnoredStops += 1
                    continue
                arrivals.append((arrival, edges, stop))
            arrivals.sort(key=itemgetter(0))
            arrivalsBySignal = defaultdict(list)
            for (pArrival, pEdges, pStop), (nArrival, nEdges, nStop) in zip(arrivals[:-1], arrivals[1:]):
                pSignal, pTimeSiSt = mergeSignals[(switch, pEdges)]
                nSignal, nTimeSiSt = mergeSignals[(switch, nEdges)]
                if switch == options.debugSwitch:
                    print(pSignal, nSignal, pStop, nStop)
                if (pSignal != nSignal and pSignal is not None and nSignal is not None
                        and pStop.vehID != nStop.vehID):
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
                    if nStop.intermediateStop and pStop.intermediateStop == nStop.intermediateStop:
                        # intermediate conflict was added via other foes and this particular conflict is a normal one
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
                    info = getIntermediateInfo(pStop, nStop)
                    times = "arrival=%s foeArrival=%s " % (humanReadableTime(nArrival), humanReadableTime(pArrival))
                    limit += countPassingTrainsToOtherStops(options, pSignal, busStop, pTimeAtSignal, end, signalTimes)
                    conflicts[nSignal].append(Conflict(nStop.prevTripId, pSignal, pStop.prevTripId, limit,
                                                       # attributes for adding comments
                                                       nStop.prevLine, pStop.prevLine,
                                                       nStop.vehID, pStop.vehID,
                                                       times, switch, nStop.busStop, info))
                    if options.redundant >= 0:
                        prevBegin = pTimeAtSignal
                        for p2Arrival, p2Stop in reversed(arrivalsBySignal[pSignal]):
                            if pArrival - p2Arrival > options.redundant:
                                break
                            if nStop.intermediateStop and p2Stop.intermediateStop == nStop.intermediateStop:
                                # intermediate conflict was added via other foes and this particular conflict is a normal one
                                continue
                            numRedundant += 1
                            numRedundantSwitchConflicts += 1
                            p2TimeAtSignal = p2Arrival - pTimeSiSt
                            limit += 1
                            limit += countPassingTrainsToOtherStops(options, pSignal,
                                                                    busStop, p2TimeAtSignal, prevBegin, signalTimes)
                            info = getIntermediateInfo(p2Stop, nStop)
                            times = "arrival=%s foeArrival=%s " % (humanReadableTime(nArrival), humanReadableTime(p2Arrival))
                            conflicts[nSignal].append(Conflict(nStop.prevTripId, pSignal, p2Stop.prevTripId, limit,
                                                               # attributes for adding comments
                                                               nStop.prevLine, p2Stop.prevLine,
                                                               nStop.vehID,
                                                               p2Stop.vehID,
                                                               times, switch,
                                                               nStop.busStop,
                                                               info))
                            prevBegin = p2TimeAtSignal

                if pSignal is not None and not (
                        options.skipParking and
                        parseBool(pStop.getAttributeSecure("parking", "false"))):
                    arrivalsBySignal[pSignal].append((pArrival, pStop))

        if options.verbose:
            print("Found %s conflicts at switch %s" % (numSwitchConflicts, switch))
            if numRedundantSwitchConflicts > 0:
                print("Found %s redundant conflicts at switch %s" % (numRedundantSwitchConflicts, switch))

            if numIgnoredSwitchConflicts > 0 or numIgnoredSwitchStops > 0:
                print("Ignored %s conflicts and % stops at switch %s" %
                      (numIgnoredSwitchConflicts, numIgnoredSwitchStops, switch))

    print("Found %s conflicts" % numConflicts)
    if numRedundant > 0:
        print("Found %s redundant conflicts" % numRedundant)

    if numIgnoredConflicts > 0 or numIgnoredStops > 0:
        print("Ignored %s conflicts and %s stops" % (numIgnoredConflicts, numIgnoredStops))
    return conflicts

def getIntermediateInfo(pStop, nStop):
    info = []
    if pStop.intermediateStop:
        info.append("intermediateStop=%s" % pStop.intermediateStop)
    if nStop.intermediateStop:
        info.append("foeIntermediateStop=%s" % nStop.intermediateStop)
    #if pStop.otherVeh:
    #    info.append("otherVeh=%s" % pStop.otherVeh)
    #if nStop.otherVeh:
    #    info.append("foeOtherVeh=%s" % nStop.otherVeh)
    return ' '.join(info)


def findSignal(net, nextEdges, reverse=False):
    for i, edge in enumerate(nextEdges):
        node = net.getEdge(edge).getFromNode()
        if node.getType() == "rail_signal":
            tls = net.getTLS(node.getID())
            prevEdge = None
            if not reverse and i > 0:
                prevEdge = nextEdges[i - 1]
            elif reverse and i + 1 < len(nextEdges):
                prevEdge = nextEdges[i + 1]

            for inLane, outLane, _ in tls.getConnections():
                if (outLane.getEdge().getID() == edge
                        and prevEdge == inLane.getEdge().getID()):
                    return tls.getID()
    return None


def findInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes):
    """find routes that start at a stop with a traffic light at end of the edge
    and routes that pass this stop. Ensure insertion happens in the correct order
    (finds constraints on insertion)
    """
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    conflicts = defaultdict(list)
    numConflicts = 0
    numIgnoredConflicts = 0
    for busStop, stops in stopRoutes.items():
        if busStop == options.debugStop:
            print("findInsertionConflicts at stop %s" % busStop)
        stopEdge = stopEdges[busStop]
        node = net.getEdge(stopEdge).getToNode()
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
        for i, (nUntil, nEdges, nStop) in enumerate(untils):
            nVehStops = vehicleStopRoutes[nStop.vehID]
            nIndex = nVehStops.index((nEdges, nStop))
            nIsPassing = nIndex < len(nVehStops) - 1
            nIsDepart = len(nEdges) == 1 and nIndex == 0
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
                        # find signal in nextEdges
                        pSignal = findSignal(net, (stopEdge,) + pNextEdges)
                        nSignal = findSignal(net, (stopEdge,) + nNextEdges)
                        if pSignal is None or nSignal is None:
                            print(("Ignoring insertion conflict between %s and %s at stop '%s' " +
                                   "because no rail signal was found after the stop") % (
                                nStop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                            continue
                        # check for inconsistent ordering
                        if pStop.getAttributeSecure("invalid", False):
                            numIgnoredConflicts += 1
                            continue
                        # predecessor tripId after stop is needed
                        pTripId = pStop.getAttributeSecure("tripId", pStop.vehID)
                        times = "until=%s foeUntil=%s " % (humanReadableTime(nUntil), humanReadableTime(pUntil))
                        conflicts[nSignal].append(Conflict(nStop.prevTripId, pSignal, pTripId, limit,
                                                           # attributes for adding comments
                                                           nStop.prevLine,
                                                           pStop.prevLine,
                                                           nStop.vehID,
                                                           pStop.vehID,
                                                           times,
                                                           switch=None,
                                                           busStop=nStop.busStop,
                                                           info=""))
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


def findFoeInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes):
    """find routes that start at a stop with a traffic light at end of the edge
    and routes that pass this stop. Ensure insertion happens in the correct order
    (finds constrains on entering the stop segment ahead of insertion)
    """
    # signal -> [(tripID, otherSignal, otherTripID, limit, line, otherLine, vehID, otherVehID), ...]
    conflicts = defaultdict(list)
    numConflicts = 0
    numIgnoredConflicts = 0
    for busStop, stops in stopRoutes.items():
        if busStop == options.debugStop:
            print("findFoeInsertionConflicts at stop %s" % busStop)
        stopEdge = stopEdges[busStop]
        node = net.getEdge(stopEdge).getToNode()
        arrivals = []
        for edgesBefore, stop in stops:
            if stop.hasAttribute("arrival") and not options.untilFromDuration:
                arrival = parseTime(stop.until)
            elif stop.hasAttribute("until"):
                arrival = parseTime(stop.until) - parseTime(stop.getAttributeSecure("duration", "0"))
            else:
                continue
            arrivals.append((arrival, edgesBefore, stop))
        # only use 'arrival' for sorting and keep the result stable otherwise
        arrivals.sort(key=itemgetter(0))
        prevDepart = None
        for i, (nArrival, nEdges, nStop) in enumerate(arrivals):
            nVehStops = vehicleStopRoutes[nStop.vehID]
            nIndex = nVehStops.index((nEdges, nStop))
            nIsPassing = nIndex < len(nVehStops) - 1
            nIsDepart = len(nEdges) == 1 and nIndex == 0
            if options.verbose and busStop == options.debugStop:
                print(i,
                      "n:", humanReadableTime(nArrival), nStop.tripId, nStop.vehID, nIndex, len(nVehStops),
                      "passing:", nIsPassing,
                      "depart:", nIsDepart)
            if prevDepart is not None and nIsPassing and not nIsDepart:
                pArrival, pEdges, pStop = prevDepart
                pVehStops = vehicleStopRoutes[pStop.vehID]
                pIndex = pVehStops.index((pEdges, pStop))
                # no need to constrain subsequent passing (simulation should maintain ordering)
                if len(nEdges) > 1 or nIndex > 0:
                    # find edges after stop
                    if busStop == options.debugStop:
                        print(i,
                              "p:", humanReadableTime(pArrival), pStop.tripId, pStop.vehID, pIndex, len(pVehStops),
                              "n:", humanReadableTime(nArrival), nStop.tripId, nStop.vehID, nIndex, len(nVehStops))
                    # both vehicles move past the stop
                    pNextEdges = pVehStops[pIndex + 1][0]
                    limit = 1  # recheck
                    # insertion vehicle must pass signal after the stop
                    # find signal in nextEdges
                    pSignal = findSignal(net, (stopEdge,) + pNextEdges)
                    if pSignal is None:
                        print(("Ignoring insertion foe conflict between %s and %s at stop '%s' " +
                               "because no rail signal was found after the stop") % (
                            nStop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                        continue
                    # passing vehicle must wait before the stop
                    nPrevEdges = nVehStops[nIndex][0]
                    if nIndex > 0:
                        # prepend on more edge from further back
                        nPrevEdges = nVehStops[nIndex - 1][0][-1:] + nPrevEdges
                    nSignal = findSignal(net, list(reversed(nPrevEdges)), True)
                    if nSignal is None:
                        print(("Ignoring foe insertion conflict between %s and %s at stop '%s' " +
                               "because no rail signal was found before the stop") % (
                            nStop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                        continue

                    # check for inconsistent ordering
                    if pStop.getAttributeSecure("invalid", False):
                        numIgnoredConflicts += 1
                        continue

                    if options.skipParking:
                        if parseBool(nStop.getAttributeSecure("parking", "false")):
                            print("ignoring stop at %s for parking vehicle %s (%s, %s)" % (
                                busStop, nStop.vehID, humanReadableTime(nArrival),
                                (humanReadableTime(parseTime(nStop.until)) if nStop.hasAttribute("until") else "-")))
                            numIgnoredConflicts += 1
                            continue
                        # if parseBool(pStop.getAttributeSecure("parking", "false")):
                        #    # additional check for until times
                        #    print("ignoring stop at %s for %s (%s, %s) after parking vehicle %s (%s, %s)" % (
                        #        busStop, nStop.vehID, humanReadableTime(nArrival),
                        #        (humanReadableTime(parseTime(nStop.until)) if nStop.hasAttribute("until") else "-"),
                        #        pStop.vehID, humanReadableTime(pArrival),
                        #        (humanReadableTime(parseTime(pStop.until)) if pStop.hasAttribute("until") else "-")))
                        #    numIgnoredConflicts += 1
                        #    continue

                    # hotfix for strange input
                    pNextStop = pVehStops[pIndex + 1][1]
                    if pNextStop.lane is not None:
                        print(("Ignoring foe insertion conflict between %s and %s at stop '%s' " +
                               "because the inserted train does not leave the stop edge (laneStop)") % (
                            nStop.prevTripId, pStop.prevTripId, busStop), file=sys.stderr)
                        continue

                    # predecessor tripId after stop is needed
                    pTripId = pStop.getAttributeSecure("tripId", pStop.vehID)
                    times = "arrival=%s foeArrival=%s " % (humanReadableTime(nArrival), humanReadableTime(pArrival))
                    conflicts[nSignal].append(Conflict(nStop.prevTripId, pSignal, pTripId, limit,
                                                       # attributes for adding comments
                                                       nStop.prevLine,
                                                       pStop.prevLine,
                                                       nStop.vehID, pStop.vehID,
                                                       times,
                                                       switch=None,
                                                       busStop=pStop.busStop,
                                                       info="foeInsertion"))
                    numConflicts += 1
                    if busStop == options.debugStop:
                        print("   found foe insertion conflict pSignal=%s nSignal=%s pVehId=%s pTripId=%s" % (
                            pSignal, nSignal, pStop.vehID, pTripId)),

            if nIsDepart and nIsPassing:
                prevDepart = (nArrival, nEdges, nStop)

    if numConflicts > 0:
        print("Found %s foe insertion conflicts" % numConflicts)
    if numIgnoredConflicts > 0:
        print("Ignored %s foe insertion conflicts" % (numIgnoredConflicts))
    return conflicts


def writeConstraint(options, outf, tag, c):
    comment = ""
    limit = c.limit + options.limit
    if options.commentLine:
        if c.line != "":
            comment += "line=%s " % c.line
        if c.otherLine != "":
            comment += "foeLine=%s " % c.otherLine
    if options.commentId:
        if c.vehID != c.tripID:
            comment += "vehID=%s " % c.vehID
        if c.otherVehID != c.otherTripID:
            comment += "foeID=%s " % c.otherVehID
    if options.commentSwitch and c.switch is not None:
            comment += "switch=%s " % c.switch
    if options.commentStop:
            comment += "busStop=%s " % c.busStop
    if options.commentTime:
            comment += c.conflictTime
    if c.info is not "":
        comment += "(%s) " % c.info
    if comment != "":
        comment = "   <!-- %s -->" % comment
    if limit == 1:
        limit = ""
    else:
        limit = ' limit="%s"' % limit
    outf.write('        <%s tripId="%s" tl="%s" foes="%s"%s/>%s\n' % (
        tag, c.tripID, c.otherSignal, c.otherTripID, limit, comment))


def main(options):
    net = sumolib.net.readNet(options.netFile)
    stopEdges = getStopEdges(options.addFile)
    uniqueRoutes, stopRoutes, vehicleStopRoutes = getStopRoutes(options, stopEdges)
    if options.abortUnordered:
        markOvertaken(options, vehicleStopRoutes, stopRoutes)
    parkingConflicts = updateStartedEnded(options, net, stopEdges, stopRoutes, vehicleStopRoutes)
    mergeSwitches = findMergingSwitches(options, uniqueRoutes, net)
    signalTimes = computeSignalTimes(options, net, stopRoutes)
    switchRoutes, mergeSignals = findStopsAfterMerge(net, stopRoutes, mergeSwitches)
    conflicts = findConflicts(options, switchRoutes, mergeSignals, signalTimes, stopEdges, vehicleStopRoutes)
    foeInsertionConflicts = findFoeInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes)
    insertionConflicts = findInsertionConflicts(options, net, stopEdges, stopRoutes, vehicleStopRoutes)

    signals = sorted(set(list(conflicts.keys())
                         + list(foeInsertionConflicts.keys())
                         + list(insertionConflicts.keys())
                         + list(parkingConflicts.keys())))

    with open(options.out, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)  # noqa
        for signal in signals:
            outf.write('    <railSignalConstraints id="%s">\n' % signal)
            for conflict in conflicts[signal] + foeInsertionConflicts[signal]:
                writeConstraint(options, outf, "predecessor", conflict)
            for conflict in insertionConflicts[signal] + parkingConflicts[signal]:
                writeConstraint(options, outf, "insertionPredecessor", conflict)
            outf.write('    </railSignalConstraints>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
