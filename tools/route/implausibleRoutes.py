#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2014-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    implausibleRoutes.py
# @author  Jakob Erdmann
# @date    2017-03-28

"""
Find routes that are implausible due to:
 - being longer than the shortest path between the first and last edge
 - being longer than the air-distance between the first and the last edge

The script computes an implausibility-score from configurable factors and
reports all routes above the specified threshold.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from optparse import OptionParser
import subprocess

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
    import sumolib  # noqa
    from sumolib.output import parse  # noqa
    from sumolib.net import readNet  # noqa
    from sumolib.miscutils import Statistics, euclidean, Colorgen  # noqa
    from route2poly import generate_poly  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    USAGE = """Usage %prog [options] <net.xml> <rou.xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--threshold", type="float", default=2.5,
                         help="Routes with an implausibility-score above treshold are reported")
    optParser.add_option("--airdist-ratio-factor", type="float", default=1, dest="airdist_ratio_factor",
                         help="Implausibility factor for the ratio of routeDist/airDist ")
    optParser.add_option("--detour-ratio-factor", type="float", default=1, dest="detour_ratio_factor",
                         help="Implausibility factor for the ratio of routeDuration/shortestDuration ")
    optParser.add_option("--detour-factor", type="float", default=0.01, dest="detour_factor",
                         help="Implausibility factor for the absolute detour time in (routeDuration-shortestDuration)" +
                              " in seconds")
    optParser.add_option("--min-dist", type="float", default=0, dest="min_dist",
                         help="Minimum shortest-path distance below which routes are implausible")
    optParser.add_option("--min-air-dist", type="float", default=0, dest="min_air_dist",
                         help="Minimum air distance below which routes are implausible")
    optParser.add_option("--standalone", action="store_true",
                         default=False, help="Parse stand-alone routes that are not define as child-element of " +
                                             "a vehicle")
    optParser.add_option("--blur", type="float", default=0,
                         help="maximum random disturbance to output polygon geometry")
    optParser.add_option("--ignore-routes", dest="ignore_routes",
                         help="List of route IDs (one per line) that are filtered when generating polygons and " +
                              "command line output (they will still be added to restrictions-output)")
    optParser.add_option("--restriction-output", dest="restrictions_output",
                         help="Write flow-restriction output suitable for passing to flowrouter.py to FILE")
    optParser.add_option("--od-restrictions", action="store_true", dest="odrestrictions",
                         default=False, help="Write restrictions for origin-destination relations rather than " +
                                             "whole routes")
    options, args = optParser.parse_args()

    if len(args) != 2:
        sys.exit(USAGE)
    options.network = args[0]
    options.routeFile = args[1]
    # options for generate_poly
    options.layer = 100
    options.geo = False
    options.internal = False
    options.spread = None

    return options


def getRouteLength(net, edges):
    return sum([net.getEdge(e).getLength() for e in edges])


class RouteInfo:
    pass


def main():
    DUAROUTER = sumolib.checkBinary('duarouter')
    options = get_options()
    net = readNet(options.network)

    routeInfos = {}  # id-> RouteInfo
    if options.standalone:
        for route in parse(options.routeFile, 'route'):
            ri = RouteInfo()
            ri.edges = route.edges.split()
            routeInfos[route.id] = ri
    else:
        for vehicle in parse(options.routeFile, 'vehicle'):
            ri = RouteInfo()
            ri.edges = vehicle.route[0].edges.split()
            routeInfos[vehicle.id] = ri

    for rInfo in routeInfos.values():
        rInfo.airDist = euclidean(
            net.getEdge(rInfo.edges[0]).getShape()[0],
            net.getEdge(rInfo.edges[-1]).getShape()[-1])
        rInfo.length = getRouteLength(net, rInfo.edges)
        rInfo.airDistRatio = rInfo.length / rInfo.airDist

    duarouterInput = options.routeFile
    if options.standalone:
        # generate suitable input file for duarouter
        duarouterInput += ".vehRoutes.xml"
        with open(duarouterInput, 'w') as outf:
            outf.write('<routes>\n')
            for rID, rInfo in routeInfos.items():
                outf.write('    <vehicle id="%s" depart="0">\n' % rID)
                outf.write('        <route edges="%s"/>\n' % ' '.join(rInfo.edges))
                outf.write('    </vehicle>\n')
            outf.write('</routes>\n')

    duarouterOutput = options.routeFile + '.rerouted.rou.xml'
    duarouterAltOutput = options.routeFile + '.rerouted.rou.alt.xml'

    subprocess.call([DUAROUTER,
                     '-n', options.network,
                     '-r', duarouterInput,
                     '-o', duarouterOutput,
                     '--no-step-log'])

    for vehicle in parse(duarouterAltOutput, 'vehicle'):
        routeAlts = vehicle.routeDistribution[0].route
        if len(routeAlts) == 1:
            routeInfos[vehicle.id].detour = 0
            routeInfos[vehicle.id].detourRatio = 1
            routeInfos[vehicle.id].shortest_path_distance = routeInfos[vehicle.id].length
        else:
            oldCosts = float(routeAlts[0].cost)
            newCosts = float(routeAlts[1].cost)
            assert(routeAlts[0].edges.split() == routeInfos[vehicle.id].edges)
            routeInfos[vehicle.id].shortest_path_distance = getRouteLength(net, routeAlts[1].edges.split())
            if oldCosts <= newCosts:
                routeInfos[vehicle.id].detour = 0
                routeInfos[vehicle.id].detourRatio = 1
                if oldCosts < newCosts:
                    sys.stderr.write(("Warning: fastest route for '%s' is slower than original route " +
                                      "(old=%s, new=%s). Check vehicle types\n") % (
                        vehicle.id, oldCosts, newCosts))
            else:
                routeInfos[vehicle.id].detour = oldCosts - newCosts
                routeInfos[vehicle.id].detourRatio = oldCosts / newCosts

    implausible = []
    allRoutesStats = Statistics("overal implausiblity")
    implausibleRoutesStats = Statistics("implausiblity above threshold")
    for rID in sorted(routeInfos.keys()):
        ri = routeInfos[rID]
        ri.implausibility = (options.airdist_ratio_factor * ri.airDistRatio +
                             options.detour_factor * ri.detour +
                             options.detour_ratio_factor * ri.detourRatio +
                             max(0, options.min_dist / ri.shortest_path_distance - 1) +
                             max(0, options.min_air_dist / ri.airDist - 1))
        allRoutesStats.add(ri.implausibility, rID)
        if ri.implausibility > options.threshold:
            implausible.append((ri.implausibility, rID, ri))
            implausibleRoutesStats.add(ri.implausibility, rID)

    # generate restrictions
    if options.restrictions_output is not None:
        with open(options.restrictions_output, 'w') as outf:
            for score, rID, ri in sorted(implausible):
                edges = ri.edges
                if options.odrestrictions and len(edges) > 2:
                    edges = [edges[0], edges[-1]]
                outf.write("0 %s\n" % " ".join(edges))

    if options.ignore_routes is not None:
        numImplausible = len(implausible)
        ignored = set([r.strip() for r in open(options.ignore_routes)])
        implausible = [r for r in implausible if r not in ignored]
        print("Loaded %s routes to ignore. Reducing implausible from %s to %s" % (
            len(ignored), numImplausible, len(implausible)))

    # generate polygons
    polyOutput = options.routeFile + '.implausible.add.xml'
    colorgen = Colorgen(("random", 1, 1))
    with open(polyOutput, 'w') as outf:
        outf.write('<additional>\n')
        for score, rID, ri in sorted(implausible):
            generate_poly(options, net, rID, colorgen(), ri.edges, outf, score)
        outf.write('</additional>\n')

    sys.stdout.write('score\troute\t(airDistRatio, detourRatio, detour, shortestDist, airDist)\n')
    for score, rID, ri in sorted(implausible):
        # , ' '.join(ri.edges)))
        sys.stdout.write('%.7f\t%s\t%s\n' % (score, rID, (ri.airDistRatio, ri.detourRatio,
                                                          ri.detour, ri.shortest_path_distance, ri.airDist)))

    print(allRoutesStats)
    print(implausibleRoutesStats)


if __name__ == "__main__":
    main()
