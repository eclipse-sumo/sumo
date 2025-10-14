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

# @file    createOvertakingReroutes.py
# @author  Jakob Erdmann
# @date    2025-09-30

"""
Parses a network and routefile to find overtaking locations and create <overtakingReroute> elements
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import euclidean, parseTime, intIfPossible, openz  # noqa
from sumolib.geomhelper import naviDegree, minAngleDegreeDiff  # noqa
from sumolib.net.lane import is_vehicle_class  # noqa

DUAROUTER = sumolib.checkBinary('duarouter')


def get_options(args=None):
    op = sumolib.options.ArgumentParser(description="Generate rerouters with overtakeReroute definitions",
                                        allowed_programs=['duarouter', 'marouter'])
    # input
    op.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=op.net_file,
                    help="define the net file (mandatory)")
    op.add_argument("-r", "--route-files", category="input", dest="routes", required=True,  type=op.route_file,
                    help="define additional files to be loaded by the router")
    # output
    op.add_argument("-o", "--output-file", category="output", dest="outfile", required=True, type=op.route_file,
                    help="define the output filename")
    # processing
    op.add_argument("--vclass", default="rail",
                    help="only consider edges which permit the given vehicle class")
    op.add_argument("--min-length", dest="minLength", metavar="FLOAT", default=100.0,
                    type=float, help="minimum siding distance")
    op.add_argument("--max-detour-factor", dest="maxDetour", metavar="FLOAT", default=2,
                    type=float, help="Maximum factor by which the siding may be longer than the main path")
    op.add_argument("--min-priority", dest="minPrio", metavar="INT",
                    type=int, help="Minimum edge priority value to be eligible as siding")
    op.add_argument("-x", "--exclude-all-routes", dest="excludeRoutes", action="store_true", default=False,
                    help="Exclude all edges that are part of input routes from sidings")
    # attributes
    op.add_argument("--prefix", category="attributes", dest="prefix", default="rr",
                    help="prefix for the rerouter ids")
    op.add_argument("-b", "--begin", category="attributes", default=0, type=op.time,
                    help="interval begin time")
    op.add_argument("-e", "--end", category="attributes", default="1:0:0:0", type=op.time,
                    help="interval end time (default 1 day)")
    options = op.parse_args(args=args)
    return options


def parseRoutes(options):
    routes = dict()
    # assign unique ids (vehicles, flows and routes have separate namespace)
    ids = set()
    for veh in sumolib.xml.parse(options.routes, ['vehicle', 'flow']):
        if veh.hasChild('route'):
            edges = tuple(veh.getChild('route')[0].edges.split())
            rid = "%s:%s" % (veh.name, veh.id)
            while rid in ids:
                rid += "#"
            ids.add(rid)
            routes[edges] = rid
    for route in sumolib.xml.parse(options.routes, ['route']):
        if route.id:
            edges = tuple(route.edges.split())
            rid = "route:%s" % route.id
            while rid in ids:
                rid += "#"
            routes[edges] = rid
    # reverse dict because (ids are are uniqe)
    return dict((rid, edges) for (edges, rid) in routes.items())


def findSwitches(options, routes, net):
    # boolean flag isOff indicates that the switch exits the main route rather than enters it
    switches = defaultdict(lambda: [])  # rid -> [(edge, isOff), ...]
    numIgnoredEdges = 0
    numRoutesIgnoredEdges = 0
    for rid, edges in routes.items():
        ni = 0
        lastI = len(edges) - 1
        for i, eid in enumerate(edges):
            if not net.hasEdge(eid):
                print("Unknown edge '%s' in route '%s'" % (eid, rid), file=sys.stderr)
                continue
            e = net.getEdge(eid)
            if not e.allows(options.vclass):
                ni += 1
                continue
            if i > 0 and len(e.getIncoming()) > 1:
                for inc in e.getIncoming().keys():
                    if inc.getID() != edges[i - 1] and inc.allows(options.vclass):
                        switches[rid].append((i, inc.getID(), False))
            if i < lastI and len(e.getOutgoing()) > 1:
                for out in e.getOutgoing().keys():
                    if out.getID() != edges[i + 1] and out.allows(options.vclass):
                        switches[rid].append((i, out.getID(), True))
        numIgnoredEdges += ni
        if ni:
            numRoutesIgnoredEdges += 1

    if numIgnoredEdges:
        print("Ignored %s edges in %s routes because they do not allow vClass '%s'" % (
            numIgnoredEdges, numRoutesIgnoredEdges, options.vclass))
    return switches


def findSidings(options, routes, switches, net):
    """use duarouter to compute paths that exit and re-enter each route"""
    fromTo = defaultdict(lambda: set())  # from -> set(to)
    fromToRoutes = defaultdict(lambda: [])  # (from, to) -> [(rid, fromIndex), ]
    for rid, swlist in switches.items():
        on = {}  # eid, toIndex
        for i, eid, isOff in reversed(swlist):
            if isOff:
                fromTo[eid].update(on.keys())
                for onEdge in on:
                    fromToRoutes[(eid, onEdge)].append((rid, i + 1, on[onEdge] - 1))
            else:
                on[eid] = i

    tmpTrips = options.outfile + ".tmp.rou.xml"
    with openz(tmpTrips, 'w') as tf:
        sumolib.writeXMLHeader(tf, "$Id$", "routes", options=options)
        tf.write('  <vType id="t" vClass="%s" maxSpeed="1"/>\n' % options.vclass)
        i = 0
        for fromE in sorted(fromTo.keys()):
            for toE in sorted(fromTo[fromE]):
                tf.write('  <trip id="%s" depart="0" type="t" from="%s" to="%s"/>\n' % (i, fromE, toE))
                i += 1
        tf.write('</routes>')

    # write weights that discourage using the (main) route edges
    mainEdges = set()
    for rid, edges in routes.items():
        mainEdges.update(edges)

    tmpWeights = options.outfile + ".tmp.weights.xml"
    with openz(tmpWeights, 'w') as tf:
        sumolib.writeXMLHeader(tf, "$Id$", "weights", options=options, rootAttrs=None)
        tf.write('  <interval id="dummy" begin="0" end="1000:0:0:0">\n')
        for eid in sorted(mainEdges):
            if net.hasEdge(eid):
                e = net.getEdge(eid)
            tf.write('    <edge id="%s" traveltime="%s"/>\n' % (eid, e.getLength() * 100))
        tf.write('  </interval>\n')
        tf.write('</weights>\n')

    tmpOut = options.outfile + ".tmp.out.rou.xml"
    subprocess.call([DUAROUTER,
                     '-n', options.netfile,
                     '-r', tmpTrips,
                     '-w', tmpWeights,
                     '-o', tmpOut,
                     '--alternatives-output', 'NUL',
                     '--ignore-errors',
                     '--no-warnings'],
                    stdout=subprocess.DEVNULL)

    # permitted siding edges
    permitted = set()
    if options.minPrio is not None:
        for e in net.getEdges():
            if e.getPriority() >= options.minPrio:
                permitted.add(e.getID())
    # collect sidings after routing
    sidings = dict()  # main -> siding
    sidingRoutes = defaultdict(lambda: [])  # main -> [(rid1, fromIndex1), ...]
    warnings = set()
    for route in sumolib.xml.parse(tmpOut, ['route']):
        edges = tuple(route.edges.split())
        if options.minPrio and any(e not in permitted for e in edges):
            # avoid abusing the opposite direciton track as siding
            continue
        if options.excludeRoutes and not any(e in mainEdges for e in edges):
            # do not obstruct any main routes
            continue
        fromTo = (edges[0], edges[-1])
        for rid, fromIndex, toIndex in fromToRoutes[fromTo]:
            # filter out paths that touch the route
            intersect = usesRoute(routes, rid, fromIndex, toIndex, edges)
            if intersect:
                warning = (fromTo[0], fromTo[1], intersect)
                if warning not in warnings:
                    print("Discarding candidate siding from '%s' to '%s' because it intersects route '%s' at edge '%s'" % (  # noqa
                        fromTo[0], fromTo[1], rid, intersect))
                    warnings.add(warning)
                continue
            main = routes[rid][fromIndex:toIndex + 1]
            sidings[main] = (rid, fromIndex, edges)
            # different routes may have the same main section but diverge downstream
            sidingRoutes[main].append((rid, fromIndex))

    return sidings, sidingRoutes


def usesRoute(routes, rid, fromIndex, toIndex, edges):
    rSet = set(routes[rid][fromIndex:toIndex])
    for e in edges:
        if e in rSet:
            return e
    return False


def filterSidings(options, net, sidings):
    sidings2 = {}
    for main, (rid, fromIndex, edges) in sidings.items():
        sidingLength = 0  # total length
        usableLength = 0
        foundSignal = False
        for eid in reversed(edges):
            if net.hasEdge(eid):
                e = net.getEdge(eid)
                sidingLength += e.getLength()
                if e.getToNode().getType() == 'rail_signal':
                    foundSignal = True
                if foundSignal:
                    usableLength += e.getLength()

        warningStart = "Discarding candidate siding from '%s' to '%s' for route '%s' because it" % (
                edges[0], edges[-1], rid)
        if foundSignal:
            if usableLength >= options.minLength:
                mainLength = 0
                for eid in main:
                    if net.hasEdge(eid):
                        e = net.getEdge(eid)
                        mainLength += e.getLength()
                    else:
                        print("Missing edge '%s' in route '%s'" % (eid, rid), file=sys.stderr)

                if mainLength == 0:
                    print("Empty main edges in route '%s'" % rid, file=sys.stderr)
                    continue

                detourFactor = sidingLength / mainLength
                if detourFactor <= options.maxDetour:
                    sidings2[main] = (rid, fromIndex, edges)
                else:
                    print("%s is longer than the main route by factor %s" % (warningStart, detourFactor))
            else:
                print("%s is only %sm long" % (warningStart, usableLength))
        else:
            print("%s has no rail_signal" % warningStart)

    return sidings2


def findFollowerSidings(options, routes, sidings, sidingRoutes):
    routeSidings = defaultdict(lambda: [])  # rid -> [mainEdges, mainEdges2, ...]
    for main, rlist in sidingRoutes.items():
        if main not in sidings:
            continue
        for rid, fromIndex in rlist:
            routeSidings[rid].append((fromIndex, main))
    for rid in routeSidings.keys():
        routeSidings[rid].sort()
    # print("\n".join(map(str, sidingRoutes.items())))
    # print("\n".join(map(str, routeSidings.items())))

    followerSidings = defaultdict(lambda: [])  # mainEdgse -> [mainEdges2, ...]
    for main, (rid, fromIndex, edges) in sidings.items():
        for rid2, fromIndex2 in sidingRoutes[main]:
            for fromIndex3, main2 in routeSidings[rid2]:
                if fromIndex3 > fromIndex2:
                    if main2 not in followerSidings[main]:
                        followerSidings[main].append(main2)
    return followerSidings


def writeSidings(options, routes, sidings, followerSidings):
    with openz(options.outfile, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        i = 0
        for main, (rid, fromIndex, edges) in sidings.items():
            rrEdge = routes[rid][fromIndex - 1]
            outf.write('  <rerouter id="%s_%s" edges="%s"> <!-- route: %s --> \n' % (options.prefix, i, rrEdge, rid))
            outf.write('    <interval begin="%s" end="%s">\n' % (options.begin, options.end))
            outf.write('       <overtakingReroute main="%s" siding="%s"/>\n' % (" ".join(main), " ".join(edges)))
            for main2 in followerSidings[main]:
                outf.write('       <overtakingReroute main="%s" siding="%s"/>\n' % (
                    " ".join(main2), " ".join(sidings[main2][2])))
            outf.write('    </interval>\n')
            outf.write('  </rerouter>\n')
            i += 1
        outf.write('</additional>\n')


def main(options):
    """
    - parse routes (edges, an associated id)
    - find switches (entering and leaving the route) for each route,
      ordered along the route (route index, enter-exit-type)
    - for each exiting switch find the shortest to all entering switches that come after it
      (from the first edge outside the route to the last edge outside the route)
      - may use duarouter bulk routing since there mulitple targets for each exiting switch
    - filter found paths (sidings) according to
      - maximum permitted space detour (user defined threshold)
      - maximum physical distance of the siding from the main route (user defined threshold)
      - presence of a signal on the siding
      - minimum length of siding
      - user defined vClass
    - write unique main/siding pairs as overtakingReroute
      (all other attributes of the overtakingReroute should be configurable with options)
      """

    net = sumolib.net.readNet(options.netfile)
    routes = parseRoutes(options)
    # print("\n".join(map(str, routes.items())))
    switches = findSwitches(options, routes, net)
    # print("\n".join(map(str, switches.items())))
    sidings, sidingRoutes = findSidings(options, routes, switches, net)
    # print("\n".join(map(str, sidings.items())))
    sidings = filterSidings(options, net, sidings)
    followerSidings = findFollowerSidings(options, routes, sidings, sidingRoutes)
    writeSidings(options, routes, sidings, followerSidings)


if __name__ == "__main__":
    try:
        main(get_options())
    except ValueError as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(1)
