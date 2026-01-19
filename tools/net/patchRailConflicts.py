#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    patchRailConflicts.py
# @author  Jakob Erdmann
# @date    2026-01-17

"""
Identifies all rail crossings (two one-directional tracks crossing each other)
and converts the junction type to the given value
"""
import os
import sys
import subprocess
from collections import defaultdict
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa

try:
    sys.stdout.reconfigure(encoding='utf-8')
except:  # noqa
    pass

NETCONVERT = sumolib.checkBinary('netconvert')


def get_options():
    ap = ArgumentParser()
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                    help="the network to read lane and edge permissions")
    ap.add_argument("-o", "--output-file", category="output", dest="output", required=True, type=ap.file,
                    help="output network file")
    ap.add_argument("--vclass", default="tram",
                    help="the vehicle class that restricts which junctions are considered")
    ap.add_argument("-t", "--junction-type", dest="junctionType", default="rail_signal",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-k", "--keep-junction-type", dest="keepJunctionType",
                    default="traffic_light,traffic_light_unregulated,rail_signal",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-e", "--end-offset", dest="endOffset", type=float, default=0,
                    help="move back the stop line from the crossing")
    ap.add_argument("-j", "--join-distance", dest="joinDist", type=float, default=200,
                    help="The distance for joining clusters which are guarded only from the outside")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args()

    outputBase = options.output
    if outputBase[-8:] == ".net.xml":
        outputBase = outputBase[:-8]
    elif outputBase[-11:] == ".net.xml.gz":
        outputBase = outputBase[:-11]

    options.vclass = [options.vclass]
    options.keepJunctionType = set(options.keepJunctionType.split(','))
    options.output_nodes = outputBase + ".nod.xml"
    options.output_edges = outputBase + ".edg.xml"
    options.output_conns = outputBase + ".con.xml"
    return options


def getDownstream(node, joinDist, nodes):
    result = set()
    seen = set()
    check = [(node, 0)]
    while check:
        n, dist = check.pop()
        seen.add(n)
        if n != node and n in nodes:
            result.add(n)
        for e in n.getOutgoing():
            dist2 = dist + e.getLength()
            if dist2 < joinDist and e.getToNode() not in seen:
                check.append((e.getToNode(), dist2))
    return result


def findClusters(joinDist, nodes):
    downstreamNodes = {n: getDownstream(n, joinDist, nodes) for n in nodes}
    check = list(downstreamNodes.keys())
    while check:
        n = check.pop(0)
        if n not in downstreamNodes:
            # already merged
            continue
        down = downstreamNodes[n]
        down2 = set(down)
        for d in down:
            if d == n:
                continue
            if d in downstreamNodes:
                down2.update(downstreamNodes[d])
                del downstreamNodes[d]
        if down != down2:
            downstreamNodes[n] = down2
            check.append(n)

    result = []
    unconnectedNodes = []
    for node, cluster in downstreamNodes.items():
        cluster.add(node)
        if len(cluster) > 1:
            incoming = sum([n.getIncoming() for n in cluster], start=[])
            result.append((list(cluster), [e for e in incoming if e.getFromNode() not in cluster]))
        else:
            unconnectedNodes.append(node)

    result.append((unconnectedNodes, sum([n.getIncoming() for n in unconnectedNodes], start=[])))
    return result


def main(options):
    net = sumolib.net.readNet(options.netfile)
    crossingNodes = []
    skipped = defaultdict(lambda: 0)

    for node in net.getNodes():
        if any([e.getPermissions() != options.vclass for e in node.getIncoming() + node.getOutgoing()]):
            continue
        if not node.hasFoes():
            continue
        nIn = len(node.getIncoming())
        nOut = len(node.getOutgoing())
        nBidi = len([e for e in node.getIncoming() + node.getOutgoing() if e.getBidi() is not None]) / 2
        if nIn >= 2 and nOut >= 1 and ((nIn + nOut - nBidi) >= 3):
            if node.getType() in options.keepJunctionType:
                skipped[node.getType()] += 1
                continue
            crossingNodes.append(node)

    clusters = findClusters(options.joinDist,  crossingNodes)

    outf_nod = open(options.output_nodes, 'w')
    outf_edg = open(options.output_edges, 'w')
    outf_con = open(options.output_conns, 'w')
    sumolib.writeXMLHeader(outf_nod, "$Id$", "nodes", options=options)
    sumolib.writeXMLHeader(outf_edg, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
    sumolib.writeXMLHeader(outf_con, "$Id$", "connections", options=options)

    for outerNodes, incomingEdges in clusters:
        for nodeID in sorted([n.getID() for n in outerNodes]):
            outf_nod.write('    <node id="%s" type="%s"/>\n' % (nodeID, options.junctionType))
        outf_nod.write('\n')

        if options.endOffset > 0:
            for edgeID in sorted([e.getID() for e in incomingEdges]):
                outf_edg.write('    <edge id="%s" endOffset="%s"/>\n' % (edgeID, options.endOffset))
            outf_edg.write('\n')

        allIncoming = set()
        for node in outerNodes:
            allIncoming.update(node.getIncoming())
        uncontrolled = allIncoming.difference(incomingEdges)

        if uncontrolled:
            for edge in uncontrolled:
                for con in sum(edge.getOutgoing().values(), start=[]):
                    outf_con.write('    <connection from="%s" fromLane="%s" to="%s" toLane="%s" uncontrolled="1"/>\n' % (  # noqa
                        con.getFrom().getID(), con.getFromLane().getIndex(),
                        con.getTo().getID(), con.getToLane().getIndex()))
            outf_con.write('\n')

    outf_nod.write("</nodes>\n")
    outf_edg.write("</edges>\n")
    outf_con.write("</connections>\n")
    outf_nod.close()
    outf_edg.close()
    outf_con.close()

    if options.verbose:
        if skipped:
            for t, n in skipped.items():
                print("Skipped %s crossings of type %s" % (n, t))

        print("Building new net")
    sys.stderr.flush()
    subprocess.call([NETCONVERT,
                     '-s', options.netfile,
                     '-n', options.output_nodes,
                     '-e', options.output_edges,
                     '-x', options.output_conns,
                     '-o', options.output,
                     ], stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    main(get_options())
