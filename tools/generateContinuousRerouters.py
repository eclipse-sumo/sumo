#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    generateContinuousRerouters.py
# @author  Jakob Erdmann
# @date    2019-11-23
# @version $Id$

"""
This script generates rerrouters ahead of every intersection with routes to each of
the downstream intersections.
"""
from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import optparse

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import euclidean  # noqa
from sumolib.geomhelper import naviDegree, minAngleDegreeDiff  # noqa


def get_options(args=None):
    parser = optparse.OptionParser()
    parser.add_option("-n", "--net-file", dest="netfile",
                      help="define the net file (mandatory)")
    parser.add_option("-o", "--output-file", dest="outfile", default="rerouters.xml",
                      help="define the output rerouter filename")
    parser.add_option("-T", "--turn-defaults", dest="turnDefaults", default="30,50,20",
                      help="Use STR[] as default turn probabilities [right,straight,left[,turn]]")
    parser.add_option("-b", "--begin",  default=0, help="begin time")
    parser.add_option("-e", "--end",  default=3600, help="end time (default 3600)")
    (options, args) = parser.parse_args(args=args)
    if not options.netfile:
        parser.print_help()
        sys.exit(1)

    options.turnDefaults = map(float, options.turnDefaults.split(','))
    if len(options.turnDefaults) not in [3, 4]:
        sys.exit("turn-defaults must be defined as 3 or 4  numbers")
    if len(options.turnDefaults) == 3:
        options.turnDefaults.append(0)  # turn with 0 probability

    return options


def getEdgesToIntersection(edge):
    result = [edge]
    while len(edge.getOutgoing().keys()) == 1:
        edge = edge.getOutgoing().keys()[0]
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


def main(options):
    net = sumolib.net.readNet(options.netfile)
    with open(options.outfile, 'w') as outf:
        outf.write('<additional>\n')
        for junction in net.getNodes():
            if len(junction.getOutgoing()) > 1:
                routes = []
                for edge in junction.getOutgoing():
                    routes.append(getEdgesToIntersection(edge))
                for edge in junction.getIncoming():
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
                            outf.write('            <routeProbReroute id="%s" probability="%s"/>\n' % (routeID, prob))
                        outf.write('        </interval>\n')
                        outf.write('    </rerouter>\n')
        outf.write('</additional>\n')


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
