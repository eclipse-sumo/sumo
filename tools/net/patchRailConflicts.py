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
Identifies all rail crossings (two one-directional tracks crossing each other) and conflict switches (two tracks merging
at a switch) and converts the junction type to the given value
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
SPLIT_TOL = 0.1


def get_options():
    ap = ArgumentParser()
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                    help="the network to read lane and edge permissions")
    ap.add_argument("--stop-file", category="input", dest="stopfile", type=ap.additional_file,
                    help="input stop file to patch when splitting edges")
    ap.add_argument("-o", "--output-file", category="output", dest="output", required=True, type=ap.file,
                    help="output network file")
    ap.add_argument("-O", "--stop-output", category="output", dest="stopOutput", type=ap.additional_file,
                    help="output file for patched stops (requires --stop-file)")
    ap.add_argument("--vclass", default="tram",
                    help="the vehicle class that restricts which junctions are considered")
    ap.add_argument("-t", "--junction-type", dest="junctionType", default="rail_signal",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-k", "--keep-junction-type", dest="keepJunctionType",
                    default="traffic_light,traffic_light_unregulated",
                    help="the new junction type for rail/rail crossings")
    ap.add_argument("-e", "--end-offset", dest="endOffset", type=float, default=0,
                    help="move back the stop line from the crossing")
    ap.add_argument("-j", "--join-distance", dest="joinDist", type=float, default=200,
                    help="The distance for joining clusters which are guarded only from the outside")
    ap.add_argument("-s", "--split-offset", type=float, default=0, dest="splitOffset",
                    help=("If set to positive value, split edges FLOAT m before traffic lights " +
                          "if they are part of a rail conflict (requires repairing routes)."))
    ap.add_argument("--split-range", type=float, default=None, dest="splitRange",
                    help=("Do not split if an existing rail_signal lies within FLOAT m upstream " +
                          "of the node (defaults to --split-offset)."))
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args()

    if options.stopOutput and not options.stopfile:
        ap.exit("Error: --stop-output requires --stop-file")

    if options.splitRange is None:
        options.splitRange = options.splitOffset

    outputBase = options.output
    if outputBase[-8:] == ".net.xml":
        outputBase = outputBase[:-8]
    elif outputBase[-11:] == ".net.xml.gz":
        outputBase = outputBase[:-11]

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


def allUpstreamGuarded(edge, distAtFromNode, maxRange, vclass, signalNodes, seenEdges):
    """Return True if all upstream vclass paths from edge.getFromNode() pass a signal within maxRange."""
    fromNode = edge.getFromNode()
    if fromNode in signalNodes:
        return True
    if distAtFromNode >= maxRange:
        return False
    incoming = [e for e in edge.getAllowedIncoming(vclass) if e != edge.getBidi()]
    if not incoming:
        return False
    for upEdge in incoming:
        if upEdge in seenEdges:
            return False
        dist2 = distAtFromNode + upEdge.getLength()
        if dist2 > maxRange:
            return False
        if not allUpstreamGuarded(upEdge, dist2, maxRange, vclass, signalNodes, seenEdges | {upEdge}):
            return False
    return True


def isEligibleSignalNode(node, options):
    if node.getType() == "dead_end" or node.getType() in options.keepJunctionType:
        return False
    return all(options.vclass in e.getPermissions() for e in node.getIncoming() + node.getOutgoing())


def addEdgeSplit(edgeSplits, usedNodeIDs, edge, offsetFromEnd):
    """Record a split on edge at -offsetFromEnd, keeping bidi alignment when present."""
    approxStartPos = edge.getLength() - offsetFromEnd
    for existingStartPos, _, _ in edgeSplits[edge]:
        if abs(existingStartPos - approxStartPos) < 0.5:
            return
    bidi = edge.getBidi()
    if bidi is None:
        edgeSplits[edge].append((approxStartPos, -offsetFromEnd, None))
        return
    baseID = "%s.%s" % (edge.getID(), int(-offsetFromEnd))
    nodeID = baseID
    idx = 1
    while nodeID in usedNodeIDs:
        nodeID = "%s#%s" % (baseID, idx)
        idx += 1
    usedNodeIDs.add(nodeID)
    edgeSplits[edge].append((approxStartPos, -offsetFromEnd, nodeID))
    for bStartPos, _, _ in edgeSplits[bidi]:
        if abs(bStartPos - offsetFromEnd) < 0.5:
            return
    edgeSplits[bidi].append((offsetFromEnd, offsetFromEnd, nodeID))


def collectSplitsForEdge(edge, dist, options, signalNodes, seenEdges, edgeSplits, extraSignalNodes, usedNodeIDs):
    """Walk upstream from edge to place rail_signal splits at splitOffset unless guarded within splitRange."""
    dEnd = dist + edge.getLength()
    fromNode = edge.getFromNode()
    if fromNode in signalNodes and dEnd <= options.splitRange:
        return

    if abs(dEnd - options.splitOffset) <= SPLIT_TOL:
        if dEnd <= options.splitRange and allUpstreamGuarded(
                edge, dEnd, options.splitRange, options.vclass, signalNodes, seenEdges):
            return
        if isEligibleSignalNode(fromNode, options):
            extraSignalNodes.add(fromNode)
        elif edge.getLength() > 0.4:
            addEdgeSplit(edgeSplits, usedNodeIDs, edge, edge.getLength() - 0.2)
        return

    if dEnd > options.splitOffset + SPLIT_TOL:
        if dEnd <= options.splitRange and allUpstreamGuarded(
                edge, dEnd, options.splitRange, options.vclass, signalNodes, seenEdges):
            return
        addEdgeSplit(edgeSplits, usedNodeIDs, edge, options.splitOffset - dist)
        return

    for upEdge in sorted(edge.getAllowedIncoming(options.vclass), key=lambda e: e.getID()):
        if upEdge != edge.getBidi() and upEdge not in seenEdges:
            collectSplitsForEdge(
                upEdge, dEnd, options, signalNodes, seenEdges | {upEdge},
                edgeSplits, extraSignalNodes, usedNodeIDs)


def main(options):
    net = sumolib.net.readNet(options.netfile)
    crossingNodes = []
    splitNodes = []
    splitNodesSet = set()
    skipped = defaultdict(lambda: 0)
    warnMixed = defaultdict(lambda: 0)
    warnMixedEdge = {}

    for node in net.getNodes():
        if node.getType() == "rail_signal":
            continue
        if not node.hasFoes():
            continue

        inVClass = [e for e in node.getIncoming() if options.vclass in e.getPermissions()]
        outVClass = [e for e in node.getOutgoing() if options.vclass in e.getPermissions()]
        nIn = len(inVClass)
        nOut = len(outVClass)
        nBidi = len([e for e in inVClass + outVClass if e.getBidi() is not None]) / 2
        isConflict = nIn >= 2 and nOut >= 1 and ((nIn + nOut - nBidi) >= 3)

        if options.splitOffset > 0 and isConflict and node.getType() in options.keepJunctionType:
            if node not in splitNodesSet:
                splitNodes.append(node)
                splitNodesSet.add(node)

        if len(node.getIncoming()) != nIn or len(node.getOutgoing()) != nOut:
            # do not build rail_signal at mixed nodes
            continue

        for e in node.getIncoming() + node.getOutgoing():
            if e.getPermissions() != [options.vclass]:
                warnMixed[tuple(e.getPermissions())] += 1
                warnMixedEdge[tuple(e.getPermissions())] = e.getID()

                if options.splitOffset > 0 and node not in splitNodesSet:
                    splitNodes.append(node)
                    splitNodesSet.add(node)

        if isConflict:
            if node.getType() in options.keepJunctionType:
                skipped[node.getType()] += 1
                continue
            crossingNodes.append(node)

    clusters = findClusters(options.joinDist, crossingNodes)
    signalNodes = {n for n in net.getNodes() if n.getType() == "rail_signal"}
    clusterData = []
    for outerNodes, incomingEdges in clusters:
        allIncoming = set()
        for node in outerNodes:
            allIncoming.update(node.getIncoming())
        uncontrolled = allIncoming.difference(incomingEdges)
        controlledNodes = []
        for n in outerNodes:
            if set(n.getIncoming()).difference(uncontrolled):
                controlledNodes.append(n.getID())
                if options.junctionType == "rail_signal":
                    signalNodes.add(n)
        clusterData.append((controlledNodes, incomingEdges, uncontrolled))

    edgeSplits = defaultdict(list)
    extraSignalNodes = set()
    if options.splitOffset > 0:
        usedNodeIDs = {n.getID() for n in net.getNodes()}
        for node in splitNodes:
            for e in sorted(node.getIncoming(), key=lambda e: e.getID()):
                if options.vclass in e.getPermissions():
                    collectSplitsForEdge(
                        e, 0.0, options, signalNodes, {e},
                        edgeSplits, extraSignalNodes, usedNodeIDs)

    outf_nod = open(options.output_nodes, 'w')
    outf_edg = open(options.output_edges, 'w')
    outf_con = open(options.output_conns, 'w')
    sumolib.writeXMLHeader(outf_nod, "$Id$", "nodes", options=options)
    sumolib.writeXMLHeader(outf_edg, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
    sumolib.writeXMLHeader(outf_con, "$Id$", "connections", options=options)

    for controlledNodes, incomingEdges, uncontrolled in clusterData:
        for nodeID in sorted(controlledNodes):
            outf_nod.write('    <node id="%s" type="%s"/>\n' % (nodeID, options.junctionType))
        outf_nod.write('\n')

        if options.endOffset > 0:
            for edgeID in sorted([e.getID() for e in incomingEdges]):
                outf_edg.write('    <edge id="%s" endOffset="%s"/>\n' % (edgeID, options.endOffset))
            outf_edg.write('\n')

        if uncontrolled:
            for edge in uncontrolled:
                for con in sum(edge.getOutgoing().values(), start=[]):
                    outf_con.write('    <connection from="%s" fromLane="%s" to="%s" toLane="%s" uncontrolled="1"/>\n' % (  # noqa
                        con.getFrom().getID(), con.getFromLane().getIndex(),
                        con.getTo().getID(), con.getToLane().getIndex()))
            outf_con.write('\n')

    for node in sorted(extraSignalNodes, key=lambda n: n.getID()):
        if node not in signalNodes:
            outf_nod.write('    <node id="%s" type="rail_signal"/>\n' % node.getID())

    for edge in sorted(edgeSplits.keys(), key=lambda e: e.getID()):
        outf_edg.write('    <edge id="%s">\n' % edge.getID())
        for _, xmlPos, nodeID in sorted(edgeSplits[edge]):
            idAttr = ' id="%s"' % nodeID if nodeID is not None else ''
            outf_edg.write('       <split pos="%s"%s type="rail_signal"/>\n' % (xmlPos, idAttr))
        outf_edg.write('    </edge>\n')

    outf_nod.write("</nodes>\n")
    outf_edg.write("</edges>\n")
    outf_con.write("</connections>\n")
    outf_nod.close()
    outf_edg.close()
    outf_con.close()

    for p, count in warnMixed.items():
        print("Warning: %s occurences of mixed permissions '%s' (example edge '%s')" % (
            count, ' '.join(p), warnMixedEdge[p]), file=sys.stderr)

    if options.verbose:
        if skipped:
            for t, n in skipped.items():
                print("Skipped %s crossings of type %s" % (n, t))

        print("Building new net")
    sys.stderr.flush()
    args = [NETCONVERT,
            '-s', options.netfile,
            '-n', options.output_nodes,
            '-e', options.output_edges,
            '-x', options.output_conns,
            '-o', options.output]
    if options.stopfile:
        args += ['--ptstop-files', options.stopfile]
        if options.stopOutput:
            args += ['--ptstop-output', options.stopOutput, '--ptstop-output.no-bidi']
    subprocess.call(args, stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    main(get_options())
