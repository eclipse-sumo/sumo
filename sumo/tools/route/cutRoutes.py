#!/usr/bin/env python
"""
@file    cutRoutes.py
@author  Jakob Erdmann
@author  Michael Behrisch
@date    2012-08-15
@version $Id$

Cut down routes from a large scenario to a sub-scenario optionally using exitTimes
Output can be a route file or a tripfile.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,sys
from optparse import OptionParser
from collections import defaultdict
import sort_routes

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    from sumolib.output import parse, parse_fast
    from sumolib.net import readNet
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    USAGE = """Usage %prog [options] <new_net.xml> <routes> [<routes2> ...]
If the given routes contain exit times these will be used to compute new
departure times. If the option --orig-net is given departure times will be
extrapolated based on edge-lengths and maximum speeds multiplied with --speed-factor"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
            default=False, help="Give more output")
    optParser.add_option("--trips-output", help="output trip file")
    optParser.add_option("--min-length", type='int', default=0, help="minimum route length in the subnetwork")
    optParser.add_option("--routes-output", help="output route file")
    optParser.add_option("--speed-factor", type='float', default=1.0, 
            help="Factor for modifying maximum edge speeds when extrapolating new departure times (default 1.0)")
    optParser.add_option("--orig-net", help="complete network for retrieving edge lengths")
    optParser.add_option("-b", "--big", action="store_true", default=False, 
            help="Perform out-of-memory sort using module sort_routes (slower but more memory efficient)")
    optParser.add_option("-d", "--disconnected-action", type='choice', default='discard', 
            choices=['discard', 'keep'], # XXX 'split', 'longest'
            help="How to deal with routes that are disconnected in the subnetwork. If 'keep' is chosen the route file must be processed with DUAROUTER with option --repair")
    #optParser.add_option("--orig-weights", help="weight file for the original network for extrapolating new departure times")
    options, args = optParser.parse_args()
    try:
        options.network = args[0]
        options.routeFiles = args[1:]
    except:
        sys.exit(USAGE)
    if ((options.trips_output is None and options.routes_output is None)
            or (options.trips_output is not None and options.routes_output is not None)):
        sys.exit("Exactly one of the options --output-trips or --output-routes must be given")
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


def cut_routes(areaEdges, orig_net, options):
    num_vehicles = 0
    num_returned = 0
    missingEdgeOccurences = defaultdict(lambda:0)
    multiAffectedRoutes = 0 # routes which enter the sub-scenario multiple times
    teleportFactorSum = 0.0
    too_short = 0
    for routeFile in options.routeFiles:
        print "Parsing routes from %s" % routeFile
        for vehicle in parse(routeFile, 'vehicle'):
            num_vehicles += 1
            edges = vehicle.route[0].edges.split()
            fromIndex = getFirstIndex(areaEdges, edges)
            if fromIndex is None:
                continue # route does not touch the area
            toIndex = len(edges) - 1 - getFirstIndex(areaEdges, reversed(edges))
            # check for minimum length
            if toIndex - fromIndex + 1 < options.min_length:
                too_short += 1
                continue
            # check for connectivity
            if missingEdges(areaEdges, edges[fromIndex:toIndex], missingEdgeOccurences):
                multiAffectedRoutes += 1
                if options.disconnected_action == 'discard':
                    continue
            # compute new departure
            if vehicle.route[0].exitTimes is None:
                if orig_net is not None:
                    # extrapolate new departure using default speed
                    newDepart = (float(vehicle.depart) +
                            sum([(orig_net.getEdge(e).getLength() / 
                                (orig_net.getEdge(e).getSpeed() * options.speed_factor)) 
                                for e in edges[:fromIndex]]))
                else:
                    print "Could not reconstruct new departure for vehicle '%s'. Using old departure" % vehicle.id
                    newDepart = vehicle.depart
            else:
                exitTimes = vehicle.route[0].exitTimes.split()
                departTimes = [vehicle.depart] + exitTimes[:-1]
                teleportFactor = len(departTimes) / float(len(edges))
                teleportFactorSum += teleportFactor
                # assume teleports were spread evenly across the vehicles route
                newDepart = int(float(departTimes[int(fromIndex * teleportFactor)]))
            yield (newDepart, vehicle.id, vehicle.type, edges[fromIndex:toIndex+1])
            num_returned += 1

    if teleportFactorSum > 0:
        teleports = " (avg teleportFactor %s)" % (1 - teleportFactorSum/num_returned)
    else:
        teleports = ""

    print "Parsed %s vehicles and kept %s routes%s" % (num_vehicles, num_returned, teleports)
    if too_short > 0:
        print "Discared %s routes because they have less than %s edges" % (too_short, options.min_length)
    print "Number of disconnected routes: %s. Most frequent missing edges:" % multiAffectedRoutes
    printTop(missingEdgeOccurences)

def getFirstIndex(areaEdges, edges):
    for i,edge in enumerate(edges):
        if edge in areaEdges:
            return i
    return None

def missingEdges(areaEdges, edges, missingEdgeOccurences):
    hasBeenAffectedOnce = False
    for edge in edges:
        if not edge in areaEdges:
            hasBeenAffectedOnce = True
            missingEdgeOccurences[edge] += 1
    return hasBeenAffectedOnce


def printTop(missingEdgeOccurences, num=1000):
    counts = sorted([(v, k) for k,v in missingEdgeOccurences.iteritems()], reverse=True)
    counts.sort(reverse=True)
    for count, edge in counts[:num]:
        print count, edge


def write_trip(file, depart, id, type, edges):
    print >>file, '    <trip depart="%s" id="%s" from="%s" to="%s" type="%s"/>' % (
            depart, id, edges[0], edges[-1], type)

def write_route(file, depart, id, type, edges):
    print >>file, '''    
    <vehicle depart="%s" id="%s" type="%s">
        <route edges="%s"/>
    </vehicle>''' % (depart, id, type, ' '.join(edges))


def main():
    options = get_options()
    net = readNet(options.network)
    edges = set([e.getID() for e in net.getEdges()])
    if options.orig_net is not None:
        orig_net = readNet(options.orig_net)
    else:
        orig_net = None
    print "Valid area contains %s edges" % len(edges)

    if options.trips:
        start_tag = '<trips>'
        end_tag = '</trips>'
        output_type = 'trips'
        writer = write_trip
    else:
        start_tag = '<routes>'
        end_tag = '</routes>'
        output_type = 'routes'
        writer = write_route

    def write_to_file(routes, f):
        comment = '<!-- generated with %s for %s from %s -->' % (os.path.basename(__file__), options.network, options.routeFiles)
        print >>f, comment
        print >>f, start_tag
        num_routes = 0
        for route in routes:
            num_routes += 1
            writer(f, *route)
        print >>f, end_tag
        print "Wrote %s %s" % (num_routes, output_type)

    if options.big:
        # write output unsorted
        tmpname = options.output + ".unsorted"
        with open(tmpname, 'w') as f:
            write_to_file(cut_routes(edges, orig_net, options), f)
        # sort out of memory
        sort_routes.main([tmpname, '--big', '--outfile', options.output])
    else:
        routes = list(cut_routes(edges, orig_net, options))
        routes.sort()
        with open(options.output, 'w') as f:
            write_to_file(routes, f)

if __name__ == "__main__":
    main()

