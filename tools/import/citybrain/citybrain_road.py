#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    citybrain_road.py
# @author  Jakob Erdmann
# @date    2021-05-07

import os
import sys
import subprocess
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(description="Import citybrains network")
    optParser.add_argument("-n", "--net-file", dest="netfile",
                           help="citybrains network file to import")
    optParser.add_argument("-o", "--output", dest="output",
                           default="net.net.xml", help="define the output sumo network filename")
    optParser.add_argument("-p", "--prefix", dest="prefix",
                           default="net", help="prefix for plain xml files")
    optParser.add_argument("-j", "--junction-type", dest="junctionType",
                           default="allway_stop", help="the default type for junctions without traffic light")
    optParser.add_argument("-t", "--temp-network", dest="tmp",
                           default="tmp.net.xml", help="intermediate network file")
    optParser.add_argument("-x", "--ignore-connections", dest="ignoreCons", action="store_true", default=False,
                           help="use connections guessed by netconvert instead of the specified connections")
    options = optParser.parse_args(args=args)
    if not options.netfile:
        optParser.print_help()
        sys.exit(1)

    return options


def main(options):

    nodefile = options.prefix + ".nod.xml"
    edgefile = options.prefix + ".edg.xml"

    edg = open(edgefile, "w")

    sumolib.writeXMLHeader(edg, "$Id$", "edges")  # noqa

    numNodes = 0
    numEdges = 0
    lastEdge = 0
    edgeLine = 0
    edgeID1 = ""
    edgeID2 = ""
    nodes = []
    nodeEdges = defaultdict(lambda: 0)
    connections = {}  # edge -> laneDirections
    for i, line in enumerate(open(options.netfile)):
        if i == 0:
            numNodes = int(line)
        elif i <= numNodes:
            nodes.append(line)
        elif i == numNodes + 1:
            numEdges = int(line)
            lastEdge = numNodes + 2 + numEdges * 3
            edgeLine = 0
        elif i < lastEdge:
            if edgeLine == 0:
                fromID, toID, length, speed, nLanes1, nLanes2, edgeID1, edgeID2 = line.split()
                nodeEdges[toID] += 1
                nodeEdges[fromID] += 1
                edg.write('    <edge id="%s" from="%s" to="%s" speed="%s" numLanes="%s" length="%s"/>\n' % (
                    edgeID1, fromID, toID, speed, nLanes1, length))
                edg.write('    <edge id="%s" from="%s" to="%s" speed="%s" numLanes="%s" length="%s"/>\n' % (
                    edgeID2, toID, fromID, speed, nLanes2, length))
            elif edgeLine == 1:
                connections[edgeID1] = list(map(int, line.split()))
            elif edgeLine == 2:
                connections[edgeID2] = list(map(int, line.split()))
            edgeLine = (edgeLine + 1) % 3
        elif i == lastEdge:
            # extract traffic signal approach ids
            break

    edg.write('</edges>\n')
    edg.close()

    with open(nodefile, "w") as nod:
        sumolib.writeXMLHeader(nod, "$Id$", "nodes")  # noqa
        for line in nodes:
            lat, lon, nodeID, signalized = line.split()
            nodeType = options.junctionType
            if signalized == "1":
                nodeType = "traffic_light"
            elif nodeEdges[nodeID] < 4:
                nodeType = "priority"
            nod.write('    <node id="%s" x="%s" y="%s" type="%s"/>\n' % (
                nodeID, lon, lat, nodeType))

        nod.write('</nodes>\n')

    NETCONVERT = sumolib.checkBinary('netconvert')

    # the sample route data didn't include a single turn-around so let's not build any
    args = [NETCONVERT,
            '-e', edgefile,
            '-n', nodefile,
            '--proj.utm',
            '--junctions.corner-detail', '0',
            '--no-turnarounds',
            '--no-internal-links',
            ]

    if options.ignoreCons:
        subprocess.call(args + ['-o', options.output])
    else:
        # connections are encoded relative to driving direction.
        # We build the network once to obtain directions and then build again with the connections
        subprocess.call(args + ['-o', options.tmp, '--no-warnings', ])

        net = sumolib.net.readNet(options.tmp)
        connfile = options.prefix + ".con.xml"
        con = open(connfile, "w")
        sumolib.writeXMLHeader(con, "$Id$", "connections")  # noqa

        for edgeID in sorted(connections.keys()):
            edge = net.getEdge(edgeID)
            directionTargets = 3 * [None]
            directionTargetLanes = [[], [], []]

            targetIndex = []
            for target, cons in edge.getOutgoing().items():
                targetLanes = []
                for c in cons:
                    targetLanes.append(c.getToLane().getIndex())
                targetIndex.append([cons[0].getJunctionIndex(), target.getID(), targetLanes,
                                    cons[0].getDirection()])
            targetIndex.sort()

            numTargets = len(targetIndex)

            if numTargets == 1:
                # interpret the single target as "straight"
                targetIndex = [[None] * 4] + targetIndex
            elif numTargets == 2:
                if targetIndex[0][-1] == 's':
                    targetIndex.insert(0, [None] * 4)
                elif targetIndex[1][-1] != 's':
                    targetIndex.insert(1, [None] * 4)

                # check which direction is missing

            for i, [linkIndex, target, targetLanes, targetDir] in enumerate(targetIndex):
                if i == 3:
                    break
                directionTargets[i] = target
                directionTargetLanes[i] = targetLanes

            code = connections[edgeID]
            lanes = edge.getLanes()
            assert(len(code) == len(lanes) * 3)
            for laneIndex, lane in enumerate(lanes):
                for index in [0, 1, 2]:
                    if code[3 * laneIndex + index] == 1:
                        if directionTargets[index] is not None:
                            # target lane is not specified, we resuse the target lanes from sumo
                            targetLanes = directionTargetLanes[index]
                            toLane = targetLanes[0]
                            if len(targetLanes) > 1:
                                directionTargetLanes[index] = targetLanes[1:]
                            con.write('    <connection from="%s" to="%s" fromLane="%s" toLane="%s"/>\n' % (
                                edgeID, directionTargets[index], laneIndex, toLane))
                        # else:
                        #    sys.stderr.write("Warning: Could not find target from edge %s laneIndex %s for direction index %s\n" % (edgeID, laneIndex, index))  # noqa

        con.write('</connections>\n')
        con.close()

        subprocess.call(args + ['-o', options.output, '-x', connfile])

        print("Built network with %s nodes and %s edges" % (numNodes, numEdges * 2))


if __name__ == "__main__":
    if not main(get_options()):
        sys.exit(1)
