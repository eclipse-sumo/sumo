#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    generateContinuousRerouters.py
# @author  Jakob Erdmann
# @date    2019-11-23

"""
This script generates rerouters ahead of every intersection with routes to each of
the downstream intersections.
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import euclidean  # noqa
from sumolib.geomhelper import naviDegree, minAngleDegreeDiff  # noqa


def get_options(args=None):
    ap = sumolib.options.ArgumentParser()
    ap.add_option("-n", "--net-file", dest="netfile", required=True, category="input", type=ap.net_file,
                  help="define the net file (mandatory)")
    ap.add_option("-o", "--output-file", dest="outfile", default="rerouters.xml", category="output", type=ap.file,
                  help="define the output rerouter filename")
    ap.add_option("-T", "--turn-defaults", dest="turnDefaults", default="30,50,20",
                  help="Use STR[] as default turn probabilities [right,straight,left[,turn]]")
    ap.add_option("-l", "--long-routes", action="store_true", dest="longRoutes", default=False,
                  help="place rerouters further upstream (after the previous decision point) to increase " +
                       "overlap of routes when rerouting and thereby improve anticipation of intersections")
    ap.add_option("--vclass", help="only create routes which permit the given vehicle class")
    ap.add_option("-b", "--begin",  default=0, help="begin time")
    ap.add_option("-e", "--end",  default=3600, help="end time (default 3600)")
    options = ap.parse_args(args=args)

    options.turnDefaults = list(map(float, options.turnDefaults.split(',')))
    if len(options.turnDefaults) not in [3, 4]:
        sys.exit("turn-defaults must be defined as 3 or 4  numbers")
    if len(options.turnDefaults) == 3:
        options.turnDefaults.append(0)  # turn with 0 probability

    return options


def getEdgesToIntersection(edge, vclass):
    result = [edge]
    seen = set()
    seen.add(edge)
    while len(edge.getOutgoing().keys()) == 1:
        edge = list(edge.getOutgoing().keys())[0]
        if edge in seen:
            break
        elif vclass is not None and not edge.allows(vclass):
            break
        else:
            seen.add(edge)
            result.append(edge)

    return result


def getTurnIndex(fromEdge, toEdge):
    cons = fromEdge.getOutgoing()[toEdge]
    con = cons[0]
    dir = con.getDirection()
    if dir == con.LINKDIR_RIGHT or dir == con.LINKDIR_PARTRIGHT:
        return 0
    elif dir == con.LINKDIR_STRAIGHT:
        return 1
    elif dir == con.LINKDIR_LEFT or dir == con.LINKDIR_PARTLEFT:
        return 2
    else:
        return 3


def getNumAlternatives(edge, routes):
    numAlternatives = 0
    for edges in routes:
        if edges[0] in edge.getOutgoing().keys():
            numAlternatives += 1
    return numAlternatives


def getNumSiblings(edge):
    """return number of outgoing edges at the fromNode of this edge that can be
    reached from a common predecessor of the given edge"""
    siblings = set()
    for cons in edge.getIncoming().values():
        for con in cons:
            for outCons in con.getFrom().getOutgoing().values():
                for outCon in outCons:
                    siblings.add(outCon.getTo())
    return len(siblings)


def main(options):
    net = sumolib.net.readNet(options.netfile)
    incomingRoutes = defaultdict(set)  # edge : set(route0, route1, ...)
    if options.longRoutes:
        # build dictionary of routes leading from an intersection to each edge
        for junction in net.getNodes():
            isEntry = len(junction.getIncoming()) == 0
            if len(junction.getOutgoing()) > 1 or isEntry:
                for edge in junction.getOutgoing():
                    if isEntry or getNumSiblings(edge) > 1:
                        edges = getEdgesToIntersection(edge, options.vclass)
                        edgeIDs = tuple([e.getID() for e in edges])
                        incomingRoutes[edges[-1]].add(edgeIDs)

    with open(options.outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="additional")
        for junction in net.getNodes():
            if len(junction.getOutgoing()) > 1:
                routes = []
                for edge in junction.getOutgoing():
                    if options.vclass is None or edge.allows(options.vclass):
                        routes.append(getEdgesToIntersection(edge, options.vclass))

                for edge in junction.getIncoming():
                    if options.longRoutes:
                        # overlapping routes: start behind an intersection and
                        # route across the next intersection to the entry of the
                        # 2nd intersection (more rerouters and overlapping routes)
                        if getNumAlternatives(edge, routes) > 1:
                            for incomingRoute in sorted(incomingRoutes[edge]):
                                assert incomingRoute[-1] == edge.getID()
                                firstEdgeID = incomingRoute[0]
                                routeIDs = []
                                for edges in routes:
                                    if edges[0] in edge.getOutgoing().keys():
                                        routeID = "%s_%s_%s" % (firstEdgeID, edge.getID(), edges[0].getID())
                                        prob = options.turnDefaults[getTurnIndex(edge, edges[0])]
                                        edgeIDs = list(incomingRoute) + [e.getID() for e in edges]
                                        outf.write('    <route id="%s" edges="%s"/>\n' % (routeID, ' '.join(edgeIDs)))
                                        routeIDs.append((routeID, prob))

                                outf.write('    <rerouter id="rr_%s_%s" edges="%s">\n' %
                                           (firstEdgeID, edge.getID(), firstEdgeID))
                                outf.write('        <interval begin="%s" end="%s">\n' % (options.begin, options.end))
                                for routeID, prob in routeIDs:
                                    outf.write('            <routeProbReroute id="%s" probability="%s"/>\n' %
                                               (routeID, prob))
                                outf.write('        </interval>\n')
                                outf.write('    </rerouter>\n')

                    else:
                        # minimal routes: start ahead of an intersection and
                        # continue up to the entry of the next intersection
                        routeIDs = []
                        for edges in routes:
                            if edges[0] in edge.getOutgoing().keys():
                                routeID = "%s_%s" % (edge.getID(), edges[0].getID())
                                prob = options.turnDefaults[getTurnIndex(edge, edges[0])]
                                edgeIDs = [e.getID() for e in [edge] + edges]
                                outf.write('    <route id="%s" edges="%s"/>\n' % (routeID, ' '.join(edgeIDs)))
                                routeIDs.append((routeID, prob))
                        if len(routeIDs) > 1:
                            outf.write('    <rerouter id="rr_%s" edges="%s">\n' % (edge.getID(), edge.getID()))
                            outf.write('        <interval begin="%s" end="%s">\n' % (options.begin, options.end))
                            for routeID, prob in routeIDs:
                                outf.write('            <routeProbReroute id="%s" probability="%s"/>\n' %
                                           (routeID, prob))
                            outf.write('        </interval>\n')
                            outf.write('    </rerouter>\n')

        outf.write('</additional>\n')


if __name__ == "__main__":
    main(get_options())
