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
    #parser.add_argument("--arrivals", action="store_true", default=False,
    #                    help="Use stop arrival time instead of 'until' time for sorting")
    parser.add_argument("-v", "--verbose", action="store_true", default=False,
                        help="tell me what you are doing")
    parser.add_argument("--debug-switch", dest="debugSwitch",
            help="debug operation for the given merge-switch edge")

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
    ignored = set()
    stopRoutes = defaultdict(list) # busStop -> [(edges, stopObj), ....]
    numRoutes = 0
    numStops = 0
    for vehicle in sumolib.xml.parse(options.routeFile, 'vehicle', heterogeneous=True):
        numRoutes += 1
        edges = tuple(vehicle.route[0].edges.split())
        uniqueRoutes.add(edges)
        lastIndex = -1
        routeIndex = 0
        tripId = vehicle.id
        for stop in vehicle.stop:
            numStops += 1
            if stop.busStop is None:
                if not stop.lane in ignored:
                    print("ignoring stop on lane '%s'" % stop.lane)
                    ignored.add(stop.lane)
                continue
            stopEdge = stopEdges[stop.busStop]
            while edges[routeIndex] != stopEdge:
                routeIndex += 1
                assert(routeIndex < len(edges))
            edgesBefore = edges[lastIndex + 1: routeIndex + 1]
            tripId = stop.getAttributeSecure("tripId", tripId)
            stop.setAttribute("prevTripId", tripId)
            stopRoutes[stop.busStop].append((edgesBefore, stop))
            lastIndex = routeIndex

    print("read %s routes (%s unique) and %s stops at %s busStops" % (
        numRoutes, len(uniqueRoutes), numStops, len(stopRoutes)))

    return uniqueRoutes, stopRoutes

def findMergingSwitches(options, uniqueRoutes, net):
    """find switches where routes merge and thus conflicts must be solved"""
    predEdges = defaultdict(set)
    for edges in uniqueRoutes:
        for i, edge in enumerate(edges):
            if i > 0:
                pred = edges[i - 1]
                if net.getEdge(edge).getBidi() != net.getEdge(pred):
                    predEdges[edge].add(pred)

    mergeSwitches = set()
    for edge, preds in predEdges.items():
        if len(preds) > 1:
            if options.verbose:
                print("mergingEdge=%s pred=%s" % (edge, ','.join(preds)))
            mergeSwitches.add(edge)

    print("processed %s routes across %s edges with %s merging switches" % (
        len(uniqueRoutes), len(predEdges), len(mergeSwitches)))
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
            for i, edge in enumerate(edgesBefore):
                node = net.getEdge(edge).getFromNode()
                if node.getType() == "rail_signal":
                    tls = net.getTLS(node.getID())
                    for inLane, outLane, linkNo in tls.getConnections():
                        if outLane.getEdge().getID() == edge:
                            signal = tls.getID()
                            signalEdgeIndex = i
                            break
                if edge in mergeSwitches:
                    numFound += 1
                    switchRoutes[edge][busStop].append((edgesBefore, stop))
                    if signal is None:
                        print("No signal found when approaching stop %s via switch %s along route %s" % (
                            busStop, edge, ','.join(edgesBefore)), file=sys.stderr)
                    time = getTravelTime(net, edgesBefore[signalEdgeIndex:])
                    mergeSignals[(edge, edgesBefore)] = (signal, time)

    print("Found %s stops after merging switches and %s signals that guard switches" % (
        numFound, len(set(mergeSignals.values()))))
    return switchRoutes, mergeSignals


def findConflicts(options, switchRoutes, mergeSignals):
    """find stops that target the same busStop from different branches of the
    prior merge switch and establish their ordering"""

    numConflicts = 0
    conflicts = defaultdict(list) # signal -> [(tripID, otherSignal, otherTripID), ...]
    for switch, stopRoutes2 in switchRoutes.items():
        numSwitchConflicts = 0
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
                pSignal, pTime = mergeSignals[(switch, pEdges)]
                nSignal, nTime = mergeSignals[(switch, nEdges)]
                if switch == options.debugSwitch:
                    print(pSignal, nSignal, pStop, nStop)
                if pSignal != nSignal:
                    numConflicts += 1
                    numSwitchConflicts += 1
                    conflicts[nSignal].append((nStop.prevTripId, pSignal, pStop.prevTripId))
        if options.verbose:
            print("Found %s conflicts at switch %s" % (numSwitchConflicts, switch))

    print("Found %s conflicts" % numConflicts)

    with open(options.out, "w") as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional")  # noqa
        for signal in sorted(conflicts.keys()):
            outf.write('    <railSignalConstraints id="%s">\n' % signal)
            for tripID, otherSignal, otherTripID in conflicts[signal]:
                outf.write('        <predecessor tripId="%s" tl="%s" foes="%s"/>\n' % (
                    tripID, otherSignal, otherTripID))
            outf.write('    </railSignalConstraints>\n')
        outf.write('</additional>\n')

def main(options):
    net = sumolib.net.readNet(options.netFile)
    stopEdges = getStopEdges(options.addFile)
    uniqueRoutes, stopRoutes = getStopRoutes(options, stopEdges)
    mergeSwitches = findMergingSwitches(options, uniqueRoutes, net)
    switchRoutes, mergeSignals = findStopsAfterMerge(net, stopRoutes, mergeSwitches)
    findConflicts(options, switchRoutes, mergeSignals)



if __name__ == "__main__":
    main(get_options())
