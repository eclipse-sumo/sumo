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

# @file    cutRoutes.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Leonhard Luecken
# @date    2017-04-11

"""
Cut down routes from a large scenario to a sub-scenario optionally using exitTimes
Output can be a route file or a tripfile.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import copy

from optparse import OptionParser
from collections import defaultdict
import sort_routes

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.xml import parse, parse_fast, writeHeader  # noqa
    from sumolib.net import readNet  # noqa
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


class Statistics:
    def __init__(self):
        self.num_vehicles = 0
        self.num_persons = 0
        self.num_flows = 0
        self.num_returned = 0
        self.missingEdgeOccurences = defaultdict(lambda: 0)
        # routes which enter the sub-scenario multiple times
        self.multiAffectedRoutes = 0
        self.teleportFactorSum = 0.0
        self.too_short = 0


def get_options(args=sys.argv[1:]):
    USAGE = """Usage %prog [options] <new_net.xml> <routes> [<routes2> ...]
If the given routes contain exit times these will be used to compute new
departure times. If the option --orig-net is given departure times will be
extrapolated based on edge-lengths and maximum speeds multiplied with --speed-factor"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--trips-output", help="output trip file")
    optParser.add_option("--pt-input", help="read public transport flows from file")
    optParser.add_option("--pt-output", help="write reduced public transport flows to file")
    optParser.add_option("--min-length", type='int', dest="min_length",
                         default=0, help="minimum route length in the subnetwork (in #edges)")
    optParser.add_option("--min-air-dist", type='int', dest="min_air_dist",
                         default=0, help="minimum route length in the subnetwork (in meters)")
    optParser.add_option("-o", "--routes-output", help="output route file")
    optParser.add_option("--stops-output", help="output filtered stop file")
    optParser.add_option(
        "-a", "--additional-input", help="additional file (for bus stop locations)")
    optParser.add_option("--speed-factor", type='float', default=1.0,
                         help="Factor for modifying maximum edge speeds when extrapolating new departure times " +
                              "(default 1.0)")
    optParser.add_option("--default.stop-duration", type='float', default=0.0, dest="defaultStopDuration",
                         help="default duration for stops in stand-alone routes")
    optParser.add_option("--orig-net", help="complete network for retrieving edge lengths")
    optParser.add_option("-b", "--big", action="store_true", default=False,
                         help="Perform out-of-memory sort using module sort_routes (slower but more memory efficient)")
    optParser.add_option("-d", "--disconnected-action", type='choice', default='discard',
                         choices=['discard', 'keep', "keep.walk"],  # XXX 'split', 'longest'
                         help="How to deal with routes that are disconnected in the subnetwork. If 'keep' is chosen " +
                              "a disconnected route generates several routes in the subnetwork corresponding to " +
                              "its parts.")
    optParser.add_option("-e", "--heterogeneous", action="store_true", default=False,
                         help="enable, if you use mixed style (external and internal routes) in the same file")
    # optParser.add_option("--orig-weights",
    # help="weight file for the original network for extrapolating new departure times")
    options, args = optParser.parse_args(args=args)
    try:
        options.network = args[0]
        options.routeFiles = args[1:]
    except Exception:
        sys.exit(USAGE.replace('%prog', os.path.basename(__file__)))
    if ((options.trips_output is None and options.routes_output is None) or
            (options.trips_output is not None and options.routes_output is not None)):
        sys.exit(
            "Exactly one of the options --trips-output or --routes-output must be given")
    else:
        if options.trips_output:
            options.output = options.trips_output
            options.trips = True
            options.routes = False
        else:
            options.output = options.routes_output
            options.routes = True
            options.trips = False
    return options


def hasMinLength(fromIndex, toIndex, edges, orig_net, options):
    if toIndex - fromIndex + 1 < options.min_length:
        return False
    if options.min_air_dist > 0:
        fromPos = orig_net.getEdge(edges[fromIndex]).getFromNode().getCoord()
        toPos = orig_net.getEdge(edges[toIndex]).getToNode().getCoord()
        if sumolib.miscutils.euclidean(fromPos, toPos) < options.min_air_dist:
            return False
    return True


def _cutEdgeList(areaEdges, oldDepart, exitTimes, edges, orig_net, options, stats, disconnected_action):
    firstIndex = getFirstIndex(areaEdges, edges)
    if firstIndex is None:
        return []  # route does not touch the area
    lastIndex = len(edges) - 1 - getFirstIndex(areaEdges, reversed(edges))
    # check for connectivity
    route_parts = [(firstIndex + i, firstIndex + j)
                   for i, j in missingEdges(areaEdges, edges[firstIndex:(lastIndex + 1)],
                                            stats.missingEdgeOccurences)]
    if len(route_parts) > 1:
        stats.multiAffectedRoutes += 1
        if disconnected_action == 'discard':
            return []
    # loop over different route parts
    result = []
    for fromIndex, toIndex in route_parts:
        if not hasMinLength(fromIndex, toIndex, edges, orig_net, options):
            stats.too_short += 1
            continue
        # compute new departure
        if exitTimes is None:
            if orig_net is not None:
                # extrapolate new departure using default speed
                newDepart = (float(oldDepart) +
                             sum([(orig_net.getEdge(e).getLength() /
                                   (orig_net.getEdge(e).getSpeed() * options.speed_factor))
                                  for e in edges[:fromIndex]]))
            else:
                newDepart = float(oldDepart)
        else:
            departTimes = [oldDepart] + exitTimes.split()[:-1]
            teleportFactor = len(departTimes) / float(len(edges))
            stats.teleportFactorSum += teleportFactor
            # assume teleports were spread evenly across the vehicles route
            newDepart = float(departTimes[int(fromIndex * teleportFactor)])
        result.append((newDepart, edges[fromIndex:toIndex + 1]))
        stats.num_returned += 1
    return result


def cut_routes(aEdges, orig_net, options, busStopEdges=None, finalEdgeMap=None):
    areaEdges = set(aEdges)
    stats = Statistics()
    standaloneRoutes = {}  # routeID -> routeObject
    standaloneRoutesDepart = {}  # routeID -> time or 'discard' or None
    vehicleTypes = {}
    if options.additional_input:
        parse_standalone_routes(options.additional_input, standaloneRoutes, vehicleTypes, options.heterogeneous)
    for routeFile in options.routeFiles:
        parse_standalone_routes(routeFile, standaloneRoutes, vehicleTypes, options.heterogeneous)
    for _, t in sorted(vehicleTypes.items()):
        yield -1, t

    for routeFile in options.routeFiles:
        print("Parsing routes from %s" % routeFile)
        for moving in parse(routeFile, ('vehicle', 'person', 'flow'), heterogeneous=options.heterogeneous):
            old_route = None
            if moving.name == 'person':
                stats.num_persons += 1
                oldDepart = moving.depart
                newDepart = None
                remaining = set()
                newPlan = []
                for planItem in moving.getChildList():
                    if planItem.name == "walk":
                        disco = "keep" if options.disconnected_action == "keep.walk" else options.disconnected_action
                        routeParts = _cutEdgeList(areaEdges, oldDepart, None,
                                                  planItem.edges.split(), orig_net, options, stats, disco)
                        if busStopEdges.get(planItem.busStop) not in areaEdges:
                            planItem.busStop = None
                        walkEdges = []
                        for depart, edges in routeParts:
                            if newDepart is None:
                                newDepart = depart
                            walkEdges += edges
                        if walkEdges:
                            remaining.update(walkEdges)
                            planItem.edges = " ".join(walkEdges)
                            newPlan.append(planItem)
                    elif planItem.name == "ride":
                        keep = True
                        if busStopEdges.get(planItem.busStop) not in areaEdges:
                            if planItem.lines in finalEdgeMap:
                                planItem.busStop = None
                                planItem.setAttribute("to", finalEdgeMap[planItem.lines])
                            else:
                                keep = False
                        if newDepart is None:
                            newDepart = float(planItem.depart)
                            planItem.lines = planItem.intended
                        if keep:
                            newPlan.append(planItem)
                        if planItem.to:
                            break
                    else:
                        newPlan.append(planItem)
                moving.setChildList(newPlan)
                cut_stops(moving, busStopEdges, remaining)
                if not moving.getChildList():
                    continue
                moving.depart = "%.2f" % newDepart
                yield newDepart, moving
            else:
                if moving.name == 'vehicle':
                    stats.num_vehicles += 1
                    oldDepart = moving.depart
                else:
                    stats.num_flows += 1
                    oldDepart = moving.begin
                if isinstance(moving.route, list):
                    old_route = moving.route[0]
                    routeRef = None
                else:
                    newDepart = standaloneRoutesDepart.get(moving.route)
                    if newDepart == 'discard':
                        # route was already checked and discarded
                        continue
                    elif newDepart is not None:
                        # route was already treated
                        if moving.name == 'vehicle':
                            newDepart += float(moving.depart)
                            moving.depart = "%.2f" % newDepart
                        else:
                            moving.end = "%.2f" % (newDepart + float(moving.end))
                            newDepart += float(moving.begin)
                            moving.begin = "%.2f" % newDepart
                        yield newDepart, moving
                        continue
                    else:
                        old_route = routeRef = standaloneRoutes[moving.route]
                routeParts = _cutEdgeList(areaEdges, oldDepart, old_route.exitTimes,
                                          old_route.edges.split(), orig_net, options,
                                          stats, options.disconnected_action)
                if routeParts and old_route.exitTimes is None and orig_net is None:
                    print("Could not reconstruct new departure time for %s '%s'. Using old departure time." %
                          (moving.name, moving.id))
                old_route.exitTimes = None
                if routeRef and not routeParts:
                    standaloneRoutesDepart[moving.route] = 'discard'
                for ix_part, (newDepart, remaining) in enumerate(routeParts):
                    departShift = None
                    if routeRef:
                        departShift = newDepart - float(oldDepart)
                        standaloneRoutesDepart[moving.route] = departShift
                    cut_stops(moving, busStopEdges, remaining)
                    if routeRef:
                        cut_stops(routeRef, busStopEdges, remaining, departShift, options.defaultStopDuration)
                        routeRef.edges = " ".join(remaining)
                        yield -1, routeRef
                    else:
                        old_route.edges = " ".join(remaining)
                    if moving.name == 'vehicle':
                        moving.depart = "%.2f" % newDepart
                    else:
                        moving.begin = "%.2f" % newDepart
                        moving.end = "%.2f" % (newDepart - float(oldDepart) + float(moving.end))
                    if len(routeParts) > 1:
                        # return copies of the vehicle for each route part
                        yield_mov = copy.deepcopy(moving)
                        yield_mov.id = moving.id + "_part" + str(ix_part)
                        yield newDepart, yield_mov
                    else:
                        yield newDepart, moving

    if stats.teleportFactorSum > 0:
        teleports = " (avg teleportFactor %s)" % (
            1 - stats.teleportFactorSum / stats.num_returned)
    else:
        teleports = ""

    print("Parsed %s vehicles, %s persons, %s flows and kept %s routes%s" %
          (stats.num_vehicles, stats.num_persons, stats.num_flows, stats.num_returned, teleports))
    if stats.too_short > 0:
        msg = "Discarded %s routes because they have less than %s edges" % (stats.too_short, options.min_length)
        if options.min_air_dist > 0:
            msg += " or the air-line distance between start and end is less than %s" % options.min_air_dist
        print(msg)
    print("Number of disconnected routes: %s. Most frequent missing edges:" %
          stats.multiAffectedRoutes)
    printTop(stats.missingEdgeOccurences)


def cut_stops(vehicle, busStopEdges, remaining, departShift=0, defaultDuration=0):
    if vehicle.stop:
        skippedStopDuration = 0
        for stop in list(vehicle.stop):
            if stop.busStop:
                if not busStopEdges:
                    print("No bus stop locations parsed, skipping bus stop '%s'." % stop.busStop)
                elif stop.busStop not in busStopEdges:
                    print("Skipping bus stop '%s', which could not be located." % stop.busStop)
                elif busStopEdges[stop.busStop] in remaining:
                    if departShift > 0 and stop.until is not None:
                        stop.until = max(0, float(stop.until) - (departShift + skippedStopDuration))
                    continue
                elif stop.duration is not None:
                    skippedStopDuration += float(stop.duration)
                else:
                    skippedStopDuration += defaultDuration
            elif stop.lane[:-2] in remaining:
                continue
            vehicle.removeChild(stop)


def getFirstIndex(areaEdges, edges):
    for i, edge in enumerate(edges):
        if edge in areaEdges:
            return i
    return None


def missingEdges(areaEdges, edges, missingEdgeOccurences):
    '''
    Returns a list of intervals corresponding to the overlapping parts of the route with the area
    '''
    # store present edge-intervals
    route_intervals = []
    start = 0
    lastEdgePresent = False  # assert: first edge is always in areaEdges
    for j, edge in enumerate(edges):
        if edge not in areaEdges:
            if lastEdgePresent:
                # this is the end of a present interval
                route_intervals.append((start, j - 1))
#                 print("edge '%s' not in area."%edge)
                lastEdgePresent = False
            missingEdgeOccurences[edge] += 1
        else:
            if not lastEdgePresent:
                # this is a start of a present interval
                start = j
                lastEdgePresent = True
    if lastEdgePresent:
        #         print("edges = %s"%str(edges))
        route_intervals.append((start, len(edges) - 1))
    return route_intervals


def printTop(missingEdgeOccurences, num=1000):
    counts = sorted(
        [(v, k) for k, v in missingEdgeOccurences.items()], reverse=True)
    counts.sort(reverse=True)
    for count, edge in counts[:num]:
        print(count, edge)


def write_trip(file, vehicle):
    edges = vehicle.route[0].edges.split()
    file.write('    <trip depart="%s" id="%s" from="%s" to="%s" type="%s"' % (
               vehicle.depart, vehicle.id, edges[0], edges[-1], vehicle.type))
    if vehicle.stop:
        file.write('>\n')
        for stop in vehicle.stop:
            file.write(stop.toXML('        '))
        file.write('</trip>\n')
    else:
        file.write('/>\n')


def write_route(file, vehicle):
    file.write(vehicle.toXML('    '))


def parse_standalone_routes(file, into, typesMap, heterogeneous):
    for element in parse(file, ('vType', 'route'), heterogeneous=heterogeneous):
        if element.id is not None:
            if element.name == 'vType':
                typesMap[element.id] = element
            else:
                into[element.id] = element


def main(options):
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])
    if options.orig_net is not None:
        orig_net = readNet(options.orig_net)
    else:
        orig_net = None
    print("Valid area contains %s edges" % len(edges))

    if options.trips:
        writer = write_trip
    else:
        writer = write_route

    busStopEdges = {}
    if options.stops_output:
        busStops = open(options.stops_output, 'w')
        writeHeader(busStops, os.path.basename(__file__), 'additional')
    if options.additional_input:
        num_busstops = 0
        kept_busstops = 0
        num_taz = 0
        kept_taz = 0
        for busStop in parse(options.additional_input, ('busStop', 'trainStop')):
            num_busstops += 1
            edge = busStop.lane[:-2]
            busStopEdges[busStop.id] = edge
            if options.stops_output and edge in edges:
                kept_busstops += 1
                if busStop.access:
                    busStop.access = [acc for acc in busStop.access if acc.lane[:-2] in edges]
                busStops.write(busStop.toXML('    '))
        for taz in parse(options.additional_input, 'taz'):
            num_taz += 1
            taz_edges = [e for e in taz.edges.split() if e in edges]
            if taz_edges:
                taz.edges = " ".join(taz_edges)
                if options.stops_output:
                    kept_taz += 1
                    busStops.write(taz.toXML('    '))
        if num_busstops > 0 and num_taz > 0:
            print("Kept %s of %s busStops and %s of %s tazs" % (
                kept_busstops, num_busstops, kept_taz, num_taz))
        elif num_busstops > 0:
            print("Kept %s of %s busStops" % (
                kept_busstops, num_busstops))
        elif num_taz > 0:
            print("Kept %s of %s tazs" % (
                kept_taz, num_taz))

    if options.stops_output:
        busStops.write('</additional>\n')
        busStops.close()

    def write_to_file(vehicles, f):
        writeHeader(f, os.path.basename(__file__), 'routes')
        numRefs = defaultdict(int)
        for _, v in vehicles:
            if options.trips and v.name == "vehicle":
                numRefs["trip"] += 1
            else:
                numRefs[v.name] += 1
            if v.name == "vType":
                f.write(v.toXML('    '))
            else:
                writer(f, v)
        f.write('</routes>\n')
        if numRefs:
            print("Wrote", ", ".join(["%s %ss" % (k[1], k[0]) for k in sorted(numRefs.items())]))
        else:
            print("Wrote nothing")

    finalEdgeMap = None
    if options.pt_input:
        allRouteFiles = options.routeFiles
        options.routeFiles = [options.pt_input]
        finalRouteEdge = {}
        finalEdgeMap = {}
        with open(options.pt_output if options.pt_output else options.pt_input + ".cut", 'w') as f:
            writeHeader(f, os.path.basename(__file__), 'routes')
            for _, v in cut_routes(edges, orig_net, options, busStopEdges):
                f.write(v.toXML('    '))
                if v.name == "route":
                    finalRouteEdge[v.id] = v.edges.split()[-1]
                elif isinstance(v.route, list):
                    finalEdgeMap[v.line] = v.route[0].edges.split()[-1]
                elif v.route is not None:
                    finalEdgeMap[v.line] = finalRouteEdge[v.route]
            f.write('</routes>\n')
        options.routeFiles = allRouteFiles

    if options.big:
        # write output unsorted
        tmpname = options.output + ".unsorted"
        with open(tmpname, 'w') as f:
            write_to_file(cut_routes(edges, orig_net, options, busStopEdges, finalEdgeMap), f)
        # sort out of memory
        sort_routes.main([tmpname, '--big', '--outfile', options.output])
    else:
        routes = list(cut_routes(edges, orig_net, options, busStopEdges, finalEdgeMap))
        routes.sort(key=lambda v: v[0])
        with open(options.output, 'w') as f:
            write_to_file(routes, f)


if __name__ == "__main__":
    main(get_options())
