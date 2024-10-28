#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
import itertools
import io

from collections import defaultdict
import sort_routes

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.xml import parse, writeHeader  # noqa
    from sumolib.net import readNet  # noqa
    from sumolib.miscutils import parseTime  # noqa
    import sumolib  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


class Statistics:
    def __init__(self):
        self.num_vehicles = 0
        self.num_persons = 0
        self.num_flows = 0
        self.num_returned = 0
        self.missingEdgeOccurrences = defaultdict(lambda: 0)
        # routes which enter the sub-scenario multiple times
        self.multiAffectedRoutes = 0
        self.teleportFactorSum = 0.0
        self.too_short = 0
        self.broken = 0

    def total(self):
        return self.num_vehicles + self.num_persons + self.num_flows


def get_options(args=None):
    USAGE = """Usage %(prog)s [options] <new_net.xml> <routes> [<routes2> ...]
If the given routes contain exit times these will be used to compute new
departure times. If the option --orig-net is given departure times will be
extrapolated based on edge-lengths and maximum speeds multiplied with --speed-factor"""
    optParser = sumolib.options.ArgumentParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--trips-output", category='output', help="output trip file")
    optParser.add_option("--pt-input", category='input', help="read public transport flows from file")
    optParser.add_option("--pt-output", category='output', help="write reduced public transport flows to file")
    optParser.add_option("--min-length", type=int,
                         default=0, help="minimum route length in the subnetwork (in #edges)")
    optParser.add_option("--min-air-dist", type=float,
                         default=0., help="minimum route length in the subnetwork (in meters)")
    optParser.add_option("-o", "--routes-output", category='output', help="output route file")
    optParser.add_option("--stops-output", category='output', help="output filtered stop file")
    optParser.add_option("-a", "--additional-input", category='input',
                         help="additional file (for bus stop locations)")
    optParser.add_option("--speed-factor", type=float, default=1.0,
                         help="Factor for modifying maximum edge speeds when extrapolating new departure times " +
                              "(default 1.0)")
    optParser.add_option("--default.stop-duration", type=float, default=0.0, dest="defaultStopDuration",
                         help="default duration for stops in stand-alone routes")
    optParser.add_option("--default.departLane", default="best", dest="defaultDepartLane",
                         help="default departure lane for cut routes")
    optParser.add_option("--default.departSpeed", default="max", dest="defaultDepartSpeed",
                         help="default departure speed for cut routes")
    optParser.add_option("--orig-net", help="complete network for retrieving edge lengths")
    optParser.add_option("-b", "--big", action="store_true", default=False,
                         help="Perform out-of-memory sort using module sort_routes (slower but more memory efficient)")
    optParser.add_option("-d", "--disconnected-action", default='discard',
                         choices=['discard', 'keep', "keep.walk"],  # XXX 'split', 'longest'
                         help="How to deal with routes that are disconnected in the subnetwork. If 'keep' is chosen " +
                              "a disconnected route generates several routes in the subnetwork corresponding to " +
                              "its parts.")
    optParser.add_option("-e", "--heterogeneous", action="store_true", default=False,
                         help="this option has no effect and only exists for backward compatibility")
    optParser.add_option("--missing-edges", type=int, metavar="N",
                         default=0, help="print N most missing edges")
    optParser.add_option("--discard-exit-times", action="store_true",
                         default=False, help="do not use exit times")
    optParser.add_argument("network", category='input', help="Provide an input network")
    optParser.add_argument("routeFiles", nargs="*", category='input',
                           help="If the given routes contain exit times, "
                                "these will be used to compute new departure times")
    # optParser.add_option("--orig-weights",
    # help="weight file for the original network for extrapolating new departure times")
    options = optParser.parse_args(args=args)

    if options.heterogeneous:
        print("Warning, the heterogeneous option is now enabled by default. Please do not use it any longer.")
    if options.trips_output is not None and options.routes_output is not None:
        sys.exit("Only one of the options --trips-output or --routes-output can be given")
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


def _cutEdgeList(areaEdges, oldDepart, exitTimes, edges, orig_net, options, stats, disconnected_action,
                 startIdx=None, endIdx=None):
    startIdx = 0 if startIdx is None else int(startIdx)
    endIdx = len(edges) - 1 if endIdx is None else int(endIdx)
    firstIndex = getFirstIndex(areaEdges, edges[startIdx:endIdx + 1])
    if firstIndex is None or firstIndex > endIdx:
        return []  # route does not touch the area
    lastIndex = endIdx - getFirstIndex(areaEdges, reversed(edges[:endIdx + 1]))
    if lastIndex < startIdx:
        return []  # route does not touch the area
    firstIndex += startIdx
    # check for connectivity
    route_parts = [(firstIndex + i, firstIndex + j)
                   for i, j in missingEdges(areaEdges, edges[firstIndex:(lastIndex + 1)],
                                            stats.missingEdgeOccurrences)]
    if len(route_parts) > 1:
        stats.multiAffectedRoutes += 1
        if disconnected_action == 'discard':
            return None
    # loop over different route parts
    result = []
    for fromIndex, toIndex in route_parts:
        if not hasMinLength(fromIndex, toIndex, edges, orig_net, options):
            stats.too_short += 1
            continue
        # compute new departure
        if exitTimes is not None:
            departTimes = [oldDepart] + exitTimes.split()[:-1]
            teleportFactor = len(departTimes) / float(len(edges))
            stats.teleportFactorSum += teleportFactor
            # assume teleports were spread evenly across the vehicles route
            newDepart = parseTime(departTimes[int(fromIndex * teleportFactor)])
        if (exitTimes is None) or (newDepart == -1):
            if orig_net is not None:
                # extrapolate new departure using default speed
                newDepart = parseTime(oldDepart)
                for e in edges[startIdx:fromIndex]:
                    if orig_net.hasEdge(e):
                        edge = orig_net.getEdge(e)
                        newDepart += edge.getLength() / (edge.getSpeed() * options.speed_factor)
                    else:
                        stats.broken += 1
                        return None
            else:
                newDepart = parseTime(oldDepart)
        result.append((newDepart, edges[fromIndex:toIndex + 1]))
        stats.num_returned += 1
    return result


def cut_routes(aEdges, orig_net, options, busStopEdges=None, ptRoutes=None, oldPTRoutes=None, collectPT=False):
    areaEdges = set(aEdges)
    stats = Statistics()
    standaloneRoutes = {}  # routeID -> routeObject
    standaloneRoutesDepart = {}  # routeID -> time or 'discard' or None
    vehicleTypes = {}
    if options.additional_input:
        for addFile in options.additional_input.split(","):
            parse_standalone_routes(addFile, standaloneRoutes, vehicleTypes)
    for routeFile in options.routeFiles:
        parse_standalone_routes(routeFile, standaloneRoutes, vehicleTypes)
    for _, t in sorted(vehicleTypes.items()):
        yield -1, t

    for routeFile in options.routeFiles:
        print("Parsing routes from %s" % routeFile)
        for moving in parse(routeFile, (u'vehicle', u'person', u'flow'),
                            {u"walk": (u"edges", u"busStop", u"trainStop")}):
            if options.verbose and stats.total() > 0 and stats.total() % 100000 == 0:
                print("%s items read" % stats.total())
            old_route = None
            if moving.name == 'person':
                stats.num_persons += 1
                oldDepart = moving.depart
                newDepart = None
                remaining = set()
                newPlan = []
                isDiscoBefore = True
                isDiscoAfter = False
                params = []
                for planItem in moving.getChildList():
                    if planItem.name == "param":
                        params.append(planItem)
                        continue
                    if planItem.name == "walk":
                        disco = "keep" if options.disconnected_action == "keep.walk" else options.disconnected_action
                        routeParts = _cutEdgeList(areaEdges, oldDepart, None,
                                                  planItem.edges.split(), orig_net, options, stats, disco)
                        if routeParts is None:
                            # the walk itself is disconnected and the disconnected_action says not to keep the person
                            newPlan = []
                            break
                        walkEdges = []
                        for depart, edges in routeParts:
                            if newDepart is None:
                                newDepart = depart
                            walkEdges += edges
                        if walkEdges:
                            if walkEdges[-1] != planItem.edges.split()[-1]:
                                planItem.busStop = None
                                planItem.trainStop = None
                                isDiscoAfter = True
                            if walkEdges[0] != planItem.edges.split()[0]:
                                isDiscoBefore = True
                            remaining.update(walkEdges)
                            planItem.edges = " ".join(walkEdges)
                            if planItem.busStop and busStopEdges.get(planItem.busStop) not in areaEdges:
                                planItem.busStop = None
                                isDiscoAfter = True
                            if planItem.trainStop and busStopEdges.get(planItem.trainStop) not in areaEdges:
                                planItem.trainStop = None
                                isDiscoAfter = True
                        else:
                            planItem = None
                    elif planItem.name == "ride":
                        # "busStop" / "trainStop" overrides "to"
                        toEdge = busStopEdges.get(planItem.busStop) if planItem.busStop else planItem.to
                        if planItem.trainStop:
                            toEdge = busStopEdges.get(planItem.trainStop)
                        try:
                            if toEdge not in areaEdges:
                                if planItem.lines in ptRoutes:
                                    ptRoute = ptRoutes[planItem.lines]
                                    oldPTRoute = oldPTRoutes[planItem.lines]
                                    # test whether ride ends before new network
                                    if oldPTRoute.index(toEdge) < oldPTRoute.index(ptRoute[0]):
                                        planItem = None
                                    else:
                                        planItem.busStop = None
                                        planItem.trainStop = None
                                        planItem.setAttribute("to", ptRoute[-1])
                                        isDiscoAfter = True
                                else:
                                    planItem = None
                            if planItem is not None:
                                if planItem.attr_from and planItem.attr_from not in areaEdges:
                                    if planItem.lines in ptRoutes:
                                        ptRoute = ptRoutes[planItem.lines]
                                        oldPTRoute = oldPTRoutes[planItem.lines]
                                        # test whether ride starts after new network
                                        if oldPTRoute.index(planItem.attr_from) > oldPTRoute.index(ptRoute[-1]):
                                            planItem = None
                                        else:
                                            planItem.setAttribute("from", ptRoute[0])
                                            if planItem.intended:
                                                planItem.lines = planItem.intended
                                            isDiscoBefore = True
                                    else:
                                        planItem = None
                                elif planItem.attr_from is None and len(newPlan) == 0:
                                    if planItem.lines in ptRoutes:
                                        planItem.setAttribute("from", ptRoutes[planItem.lines][0])
                                    else:
                                        planItem = None
                        except ValueError as e:
                            print("Error handling ride in '%s'" % moving.id, e)
                            planItem = None
                        if planItem is not None and newDepart is None and planItem.depart is not None:
                            newDepart = parseTime(planItem.depart)
                    if planItem is None:
                        isDiscoAfter = True
                    else:
                        newPlan.append(planItem)
                    if len(newPlan) > 1 and isDiscoBefore and options.disconnected_action == "discard":
                        newPlan = []
                        break
                    isDiscoBefore = isDiscoAfter
                    isDiscoAfter = False
                if not newPlan:
                    continue
                moving.setChildList(params + newPlan)
                cut_stops(moving, busStopEdges, remaining)
                if newDepart is None:
                    newDepart = parseTime(moving.depart)
                if newPlan[0].name == "ride" and newPlan[0].lines == newPlan[0].intended:
                    moving.depart = "triggered"
                else:
                    moving.depart = "%.2f" % newDepart
                yield newDepart, moving
            else:
                if moving.name == 'vehicle':
                    stats.num_vehicles += 1
                    oldDepart = parseTime(moving.depart)
                else:
                    stats.num_flows += 1
                    oldDepart = parseTime(moving.begin)
                if moving.routeDistribution is not None:
                    old_route = moving.addChild("route", {"edges": moving.routeDistribution[0].route[-1].edges})
                    moving.removeChild(moving.routeDistribution[0])
                    routeRef = None
                elif isinstance(moving.route, list):
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
                            newDepart += oldDepart
                            moving.depart = "%.2f" % newDepart
                        else:
                            if moving.end:
                                moving.end = "%.2f" % (newDepart + parseTime(moving.end))
                            newDepart += oldDepart
                            moving.begin = "%.2f" % newDepart
                        if collectPT and moving.line and moving.line not in oldPTRoutes:
                            oldPTRoutes[moving.line] = standaloneRoutes[moving.route].edges.split()
                        cut_stops(moving, busStopEdges, set(standaloneRoutes[moving.route].edges.split()))
                        moving.departEdge = None  # the cut already removed the unused edges
                        moving.arrivalEdge = None  # the cut already removed the unused edges
                        yield newDepart, moving
                        continue
                    else:
                        old_route = routeRef = standaloneRoutes[moving.route]
                if options.discard_exit_times:
                    old_route.exitTimes = None
                if collectPT and moving.line and moving.line not in oldPTRoutes:
                    oldPTRoutes[moving.line] = old_route.edges.split()
                routeParts = _cutEdgeList(areaEdges, oldDepart, old_route.exitTimes,
                                          old_route.edges.split(), orig_net, options,
                                          stats, options.disconnected_action, moving.departEdge, moving.arrivalEdge)
                if options.verbose and routeParts and old_route.exitTimes is None and orig_net is None:
                    print("Could not reconstruct new departure time for %s '%s'. Using old departure time." %
                          (moving.name, moving.id))
                old_route.exitTimes = None
                if routeRef and not routeParts:
                    standaloneRoutesDepart[moving.route] = 'discard'
                for ix_part, (newDepart, remaining) in enumerate(routeParts or []):
                    departShift = cut_stops(moving, busStopEdges, remaining)
                    if routeRef:
                        departShift = cut_stops(routeRef, busStopEdges, remaining,
                                                newDepart - oldDepart, options.defaultStopDuration, True)
                        standaloneRoutesDepart[moving.route] = departShift
                        newDepart = oldDepart + departShift
                        routeRef.edges = " ".join(remaining)
                        yield -1, routeRef
                    else:
                        if ix_part > 0 or old_route.edges.split()[0] != remaining[0]:
                            moving.setAttribute("departLane", options.defaultDepartLane)
                            moving.setAttribute("departSpeed", options.defaultDepartSpeed)
                        newDepart = max(newDepart, departShift)
                        old_route.edges = " ".join(remaining)
                    if moving.name == 'vehicle':
                        moving.depart = "%.2f" % newDepart
                    else:
                        moving.begin = "%.2f" % newDepart
                        if moving.end:
                            moving.end = "%.2f" % (newDepart - oldDepart + parseTime(moving.end))
                    moving.departEdge = None  # the cut already removed the unused edges
                    moving.arrivalEdge = None  # the cut already removed the unused edges
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
    print("Number of disconnected routes: %s, broken routes: %s." % (stats.multiAffectedRoutes, stats.broken))
    if options.missing_edges > 0:
        print("Most frequent missing edges:")
        counts = sorted([(v, k) for k, v in stats.missingEdgeOccurrences.items()], reverse=True)
        for count, edge in itertools.islice(counts, options.missing_edges):
            print(count, edge)


def cut_stops(vehicle, busStopEdges, remaining, departShift=0, defaultDuration=0, isStandalone=False):
    if vehicle.stop:
        skippedStopDuration = 0
        haveStop = False
        for stop in list(vehicle.stop):
            until = None if stop.until is None else parseTime(stop.until)
            if stop.busStop:
                if not busStopEdges:
                    print("No bus stop locations parsed, skipping bus stop '%s'." % stop.busStop)
                elif stop.busStop not in busStopEdges:
                    print("Skipping bus stop '%s', which could not be located." % stop.busStop)
                elif busStopEdges[stop.busStop] in remaining:
                    if departShift > 0 and until is not None and isStandalone:
                        stop.until = max(0, until - departShift)
                    haveStop = True
                    continue
                elif stop.duration is not None:
                    skippedStopDuration += parseTime(stop.duration)
                else:
                    skippedStopDuration += defaultDuration
            elif stop.lane[:-2] in remaining:
                haveStop = True
                continue
            if until is not None and not haveStop:
                if departShift < until:
                    departShift = until
            vehicle.removeChild(stop)
    return departShift


def getFirstIndex(areaEdges, edges):
    for i, edge in enumerate(edges):
        if edge in areaEdges:
            return i
    return None


def missingEdges(areaEdges, edges, missingEdgeOccurrences):
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
            missingEdgeOccurrences[edge] += 1
        else:
            if not lastEdgePresent:
                # this is a start of a present interval
                start = j
                lastEdgePresent = True
    if lastEdgePresent:
        #         print("edges = %s"%str(edges))
        route_intervals.append((start, len(edges) - 1))
    return route_intervals


def write_trip(file, vehicle):
    edges = vehicle.route[0].edges.split()
    file.write(u'    <trip depart="%s" id="%s" from="%s" to="%s" type="%s"' %
               (vehicle.depart, vehicle.id, edges[0], edges[-1], vehicle.type))
    if vehicle.departLane:
        file.write(u' departLane="%s"' % vehicle.departLane)
    if vehicle.departSpeed:
        file.write(u' departSpeed="%s"' % vehicle.departSpeed)
    if vehicle.stop:
        file.write(u'>\n')
        for stop in vehicle.stop:
            file.write(stop.toXML(u'        '))
        file.write(u'</trip>\n')
    else:
        file.write(u'/>\n')


def write_route(file, vehicle):
    file.write(vehicle.toXML(u'    '))


def parse_standalone_routes(file, into, typesMap):
    for element in parse(file, ('vType', 'route')):
        if element.id is not None:
            if element.name == 'vType':
                typesMap[element.id] = element
            else:
                into[element.id] = element


def main(options):
    if options.verbose:
        print("Reading reduced network from", options.network)
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])
    if options.orig_net is not None:
        if options.verbose:
            print("Reading original network from", options.orig_net)
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
        busStops = io.open(options.stops_output, 'w', encoding="utf8")
        writeHeader(busStops, os.path.basename(__file__), 'additional')
    if options.additional_input:
        num_busstops = 0
        kept_busstops = 0
        num_taz = 0
        kept_taz = 0
        for addFile in options.additional_input.split(","):
            for busStop in parse(addFile, ('busStop', 'trainStop')):
                num_busstops += 1
                edge = busStop.lane[:-2]
                busStopEdges[busStop.id] = edge
                if options.stops_output and edge in edges:
                    kept_busstops += 1
                    if busStop.access:
                        busStop.access = [acc for acc in busStop.access if acc.lane[:-2] in edges]
                    busStops.write(busStop.toXML(u'    '))
            for taz in parse(addFile, 'taz'):
                num_taz += 1
                taz_edges = [e for e in taz.edges.split() if e in edges]
                if taz_edges:
                    taz.edges = " ".join(taz_edges)
                    if options.stops_output:
                        kept_taz += 1
                        busStops.write(taz.toXML(u'    '))
        if num_busstops > 0 and num_taz > 0:
            print("Kept %s of %s busStops and %s of %s tazs" % (
                kept_busstops, num_busstops, kept_taz, num_taz))
        elif num_busstops > 0:
            print("Kept %s of %s busStops" % (kept_busstops, num_busstops))
        elif num_taz > 0:
            print("Kept %s of %s tazs" % (kept_taz, num_taz))

    if options.stops_output:
        busStops.write(u'</additional>\n')
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
                f.write(v.toXML(u'    '))
            else:
                writer(f, v)
        f.write(u'</routes>\n')
        if numRefs:
            print("Wrote", ", ".join(["%s %ss" % (k[1], k[0]) for k in sorted(numRefs.items())]))
        else:
            print("Wrote nothing")

    ptRoutes = {}
    oldPTRoutes = {}
    if options.pt_input:
        allRouteFiles = options.routeFiles
        options.routeFiles = options.pt_input.split(",")
        ptExternalRoutes = {}
        with io.open(options.pt_output if options.pt_output else options.pt_input + ".cut", 'w', encoding="utf8") as f:
            writeHeader(f, os.path.basename(__file__), 'routes')
            for _, v in cut_routes(edges, orig_net, options, busStopEdges, None, oldPTRoutes, True):
                f.write(v.toXML(u'    '))
                if v.name == "route":
                    ptExternalRoutes[v.id] = v.edges.split()
                elif isinstance(v.route, list):
                    ptRoutes[v.line] = v.route[0].edges.split()
                elif v.route is not None:
                    ptRoutes[v.line] = ptExternalRoutes[v.route]
            f.write(u'</routes>\n')
        options.routeFiles = allRouteFiles

    if options.output:
        if options.big:
            # write output unsorted
            tmpname = options.output + ".unsorted"
            with io.open(tmpname, 'w', encoding="utf8") as f:
                write_to_file(cut_routes(edges, orig_net, options, busStopEdges, ptRoutes, oldPTRoutes), f)
            # sort out of memory
            sort_routes.main([tmpname, '--big', '--outfile', options.output])
        else:
            routes = list(cut_routes(edges, orig_net, options, busStopEdges, ptRoutes, oldPTRoutes))
            routes.sort(key=lambda v: v[0])
            with io.open(options.output, 'w', encoding="utf8") as f:
                write_to_file(routes, f)


if __name__ == "__main__":
    main(get_options())
