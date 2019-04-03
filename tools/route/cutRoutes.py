#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    cutRoutes.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Leonhard Luecken
# @date    2017-04-11
# @version $Id$

"""
Cut down routes from a large scenario to a sub-scenario optionally using exitTimes
Output can be a route file or a tripfile.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import codecs
import copy

from optparse import OptionParser
from collections import defaultdict
import sort_routes

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.output import parse, parse_fast  # noqa
    from sumolib.net import readNet  # noqa
    import sumolib
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options(args=sys.argv[1:]):
    USAGE = """Usage %prog [options] <new_net.xml> <routes> [<routes2> ...]
If the given routes contain exit times these will be used to compute new
departure times. If the option --orig-net is given departure times will be
extrapolated based on edge-lengths and maximum speeds multiplied with --speed-factor"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--trips-output", help="output trip file")
    optParser.add_option("--min-length", type='int', dest="min_length",
                         default=0, help="minimum route length in the subnetwork (in #edges)")
    optParser.add_option("--min-air-dist", type='int', dest="min_air_dist",
                         default=0, help="minimum route length in the subnetwork (in meters)")
    optParser.add_option("--routes-output", help="output route file")
    optParser.add_option("--stops-output", help="output filtered stop file")
    optParser.add_option(
        "-a", "--additional-input", help="additional file (for bus stop locations)")
    optParser.add_option("--speed-factor", type='float', default=1.0,
                         help="Factor for modifying maximum edge speeds when extrapolating new departure times " +
                              "(default 1.0)")
    optParser.add_option("--default.stop-duration", type='float', default=0.0, dest="defaultStopDuration",
                         help="default duration for stops in stand-alone routes")
    optParser.add_option(
        "--orig-net", help="complete network for retrieving edge lengths")
    optParser.add_option("-b", "--big", action="store_true", default=False,
                         help="Perform out-of-memory sort using module sort_routes (slower but more memory efficient)")
    optParser.add_option("-d", "--disconnected-action", type='choice', default='discard',
                         choices=['discard', 'keep'],  # XXX 'split', 'longest'
                         help="How to deal with routes that are disconnected in the subnetwork. If 'keep' is chosen " +
                              "a disconnected route generates several routes in the subnetwork corresponding to " +
                              "its parts.")
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


def cut_routes(aEdges, orig_net, options, busStopEdges=None):
    areaEdges = set(aEdges)
    num_vehicles = 0
    num_returned = 0
    missingEdgeOccurences = defaultdict(lambda: 0)
    # routes which enter the sub-scenario multiple times
    multiAffectedRoutes = 0
    teleportFactorSum = 0.0
    too_short = 0
    too_short_airdist = 0
    standaloneRoutes = {}  # routeID -> routeObject
    standaloneRoutesDepart = {}  # routeID -> time or 'discard' or None
    if options.additional_input:
        parse_standalone_routes(options.additional_input, standaloneRoutes)
    for routeFile in options.routeFiles:
        parse_standalone_routes(routeFile, standaloneRoutes)

    for routeFile in options.routeFiles:
        print("Parsing routes from %s" % routeFile)
        for vehicle in parse(routeFile, 'vehicle'):
            num_vehicles += 1
            if type(vehicle.route) == list:
                edges = vehicle.route[0].edges.split()
                routeRef = False
            else:
                newDepart = standaloneRoutesDepart.get(vehicle.route)
                if newDepart is 'discard':
                    # route was already checked and discared
                    continue
                elif newDepart is not None:
                    # route was already treated
                    vehicle.depart = "%.2f" % (newDepart + float(vehicle.depart))
                    yield vehicle.depart, vehicle
                    continue
                else:
                    routeRef = standaloneRoutes[vehicle.route]
                    edges = routeRef.edges.split()
            firstIndex = getFirstIndex(areaEdges, edges)
            if firstIndex is None:
                continue  # route does not touch the area
            lastIndex = len(edges) - 1 - \
                getFirstIndex(areaEdges, reversed(edges))
            # check for connectivity
            route_parts = [(firstIndex + i, firstIndex + j)
                           for (i, j) in missingEdges(areaEdges, edges[firstIndex:(lastIndex + 1)],
                                                      missingEdgeOccurences)]
#             print("areaEdges: %s"%str(areaEdges))
#             print("routeEdges: %s"%str(edges))
#             print("firstIndex = %d"%firstIndex)
#             print("route_parts = %s"%str(route_parts))
            if len(route_parts) > 1:
                multiAffectedRoutes += 1
                if options.disconnected_action == 'discard':
                    if routeRef:
                        standaloneRoutesDepart[vehicle.route] = 'discard'
                    continue
            # loop over different route parts
            for ix_part, ix_interval in enumerate(route_parts):
                fromIndex, toIndex = ix_interval
                # print("(fromIndex,toIndex) = (%d,%d)"%(fromIndex,toIndex))
                # check for minimum length
                if toIndex - fromIndex + 1 < options.min_length:
                    too_short += 1
                    if routeRef:
                        standaloneRoutesDepart[vehicle.route] = 'discard'
                    continue
                if options.min_air_dist > 0:
                    fromPos = orig_net.getEdge(edges[fromIndex]).getFromNode().getCoord()
                    toPos = orig_net.getEdge(edges[toIndex]).getToNode().getCoord()
                    if sumolib.miscutils.euclidean(fromPos, toPos) < options.min_air_dist:
                        too_short_airdist += 1
                        if routeRef:
                            standaloneRoutesDepart[vehicle.route] = 'discard'
                        continue
                # compute new departure
                if routeRef or vehicle.route[0].exitTimes is None:
                    if orig_net is not None:
                        # extrapolate new departure using default speed
                        newDepart = (float(vehicle.depart) +
                                     sum([(orig_net.getEdge(e).getLength() /
                                           (orig_net.getEdge(e).getSpeed() * options.speed_factor))
                                          for e in edges[:fromIndex]]))
                    else:
                        print(
                            "Could not reconstruct new departure time for vehicle '%s'. Using old departure time." %
                            vehicle.id)
                        newDepart = float(vehicle.depart)
                else:
                    exitTimes = vehicle.route[0].exitTimes.split()
                    departTimes = [vehicle.depart] + exitTimes[:-1]
                    teleportFactor = len(departTimes) / float(len(edges))
                    teleportFactorSum += teleportFactor
                    # assume teleports were spread evenly across the vehicles route
                    newDepart = float(departTimes[int(fromIndex * teleportFactor)])
                    del vehicle.route[0].exitTimes
                departShift = None
                if routeRef:
                    departShift = newDepart - float(vehicle.depart)
                    standaloneRoutesDepart[vehicle.route] = departShift
                remaining = edges[fromIndex:toIndex + 1]
                stops = cut_stops(vehicle, busStopEdges, remaining)
                if routeRef:
                    routeRef.stop = cut_stops(routeRef, busStopEdges, remaining,
                                              departShift, options.defaultStopDuration)
                    routeRef.edges = " ".join(remaining)
                    yield None, routeRef
                else:
                    vehicle.route[0].edges = " ".join(remaining)
                vehicle.stop = stops
                vehicle.depart = "%.2f" % newDepart
                if len(route_parts) > 1:
                    # return copies of the vehicle for each route part
                    yield_veh = copy.deepcopy(vehicle)
                    yield_veh.id = vehicle.id + "_part" + str(ix_part)
                    yield newDepart, yield_veh
                else:
                    yield newDepart, vehicle
                num_returned += 1

    if teleportFactorSum > 0:
        teleports = " (avg teleportFactor %s)" % (
            1 - teleportFactorSum / num_returned)
    else:
        teleports = ""

    print("Parsed %s vehicles and kept %s routes%s" %
          (num_vehicles, num_returned, teleports))
    if too_short > 0:
        print("Discarded %s routes because they have less than %s edges" %
              (too_short, options.min_length))
    if too_short_airdist > 0:
        print("Discarded %s routes because the air-line distance between start and end is less than %s" %
              (too_short_airdist, options.min_air_dist))
    print("Number of disconnected routes: %s. Most frequent missing edges:" %
          multiAffectedRoutes)
    printTop(missingEdgeOccurences)


def cut_stops(vehicle, busStopEdges, remaining, departShift=0, defaultDuration=0):
    stops = []
    if vehicle.stop:
        skippedStopDuration = 0
        for stop in vehicle.stop:
            if stop.busStop:
                if not busStopEdges:
                    print(
                        "No bus stop locations parsed, skipping bus stop '%s'." % stop.busStop)
                    continue
                if stop.busStop not in busStopEdges:
                    print(
                        "Skipping bus stop '%s', which could not be located." % stop.busStop)
                    continue
                if busStopEdges[stop.busStop] in remaining:
                    if departShift > 0 and stop.until is not None:
                        stop.until = max(0, float(stop.until) - (departShift + skippedStopDuration))
                    stops.append(stop)
                elif stop.duration is not None:
                    skippedStopDuration += float(stop.duration)
                else:
                    skippedStopDuration += defaultDuration

            elif stop.lane[:-2] in remaining:
                stops.append(stop)
    return stops


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


def parse_standalone_routes(file, into):
    for route in parse(file, 'route'):
        into[route.id] = route


def main(options):
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])
    if options.orig_net is not None:
        orig_net = readNet(options.orig_net)
    else:
        orig_net = None
    print("Valid area contains %s edges" % len(edges))

    if options.trips:
        output_type = 'trips'
        writer = write_trip
    else:
        output_type = 'routes'
        writer = write_route

    busStopEdges = {}
    if options.stops_output:
        busStops = codecs.open(options.stops_output, 'w', encoding='utf8')
        busStops.write(
            '<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
            'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/additional_file.xsd">\n')
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
                busStops.write(busStop.toXML('    ').decode('utf8'))
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
        f.write('<!-- generated with %s for %s from %s -->\n' %
                (os.path.basename(__file__), options.network, options.routeFiles))
        f.write(
            ('<routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ' +
             'xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/routes_file.xsd">\n'))
        num_routeRefs = 0
        num_vehicles = 0
        for _, v in vehicles:
            if v.name == 'route':
                num_routeRefs += 1
            else:
                num_vehicles += 1
            writer(f, v)
        f.write('</routes>\n')
        if num_routeRefs > 0:
            print("Wrote %s standalone-routes and %s vehicles" % (num_routeRefs, num_vehicles))
        else:
            print("Wrote %s %s" % (num_vehicles, output_type))

    if options.big:
        # write output unsorted
        tmpname = options.output + ".unsorted"
        with codecs.open(tmpname, 'w', encoding='utf8') as f:
            write_to_file(
                cut_routes(edges, orig_net, options, busStopEdges), f)
        # sort out of memory
        sort_routes.main([tmpname, '--big', '--outfile', options.output])
    else:
        routes = list(cut_routes(edges, orig_net, options, busStopEdges))
        routes.sort(key=lambda v: v[0])
        with codecs.open(options.output, 'w', encoding='utf8') as f:
            write_to_file(routes, f)


if __name__ == "__main__":
    main(get_options())
