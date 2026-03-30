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

# @file    patchRailPriorities.py
# @author  Jakob Erdmann
# @date    2026-03-28

"""
Identifies single-track routes and attempts to find sidings for passing in
opposite directions. For found sidings
- the routingType and priorities in the network are patched so that trains prefer driving on the right
- if there is a ptStop on the main line but not on the siding, a new ptStop is added

"""

import os
import sys
import subprocess
from collections import defaultdict
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
sys.path.append(os.path.join(SUMO_HOME, 'tools', 'trigger'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.geomhelper import sub, crossProduct2D, positionAtShapeOffset, polygonOffsetAndDistanceToPoint  # noqa
from sumolib.net import lane2edge  # noqa
from createOvertakingReroutes import parseRoutes, findSwitches, findSidings, filterSidings

try:
    sys.stdout.reconfigure(encoding='utf-8')
except:  # noqa
    pass

NETCONVERT = sumolib.checkBinary('netconvert')


def get_options():
    ap = ArgumentParser()
    ap.add_argument("-n", "--net-file", category="input", dest="netfile", required=True, type=ap.net_file,
                    help="the network to read lane and edge permissions")
    ap.add_argument("-r", "--route-files", category="input", dest="routes", required=True, type=ap.route_file_list,
                    help="The file to road routes from (can also be a ptline-file)")
    ap.add_argument("-s", "--stop-file", category="input", dest="stopfile", type=ap.additional_file,
                    help="The file to road stops from")
    ap.add_argument("-o", "--output-file", category="output", dest="outfile", required=True, type=ap.net_file,
                    help="output network file")
    ap.add_argument("-p", "--patchfile-prefix", category="output", dest="prefix",
                    help="prefix for generated patch files")
    ap.add_argument("-O", "--stop-output", category="output", dest="stopOutput", type=ap.additional_file,
                    help="stop output file (requires stop-file)")
    ap.add_argument("--vclass", default="rail",
                    help="only consider edges which permit the given vehicle class")
    ap.add_argument("--min-length", dest="minLength", metavar="FLOAT", default=100.0,
                    type=float, help="minimum siding length")
    ap.add_argument("--max-length", dest="maxLength", metavar="FLOAT",
                    type=float, help="maximum siding length")
    ap.add_argument("--max-detour-factor", dest="maxDetour", metavar="FLOAT", default=2,
                    type=float, help="Maximum factor by which the siding may be longer than the main path")
    ap.add_argument("--min-priority", dest="minPrio", metavar="INT",
                    type=int, help="Minimum edge priority value to be eligible as siding")
    ap.add_argument("-x", "--exclude-usage", dest="excludeUsage", type=int,
                    help="Exclude all edges that are used at least INT times by loaded routes")
    ap.add_argument("-X", "--exclude-reverse-usage", dest="excludeRevUsage", type=int, default="1",
                    help="Exclude all edges that are used at least INT times in reverse by loaded routes")
    ap.add_argument("--reversal-penalty", dest="reversalPenalty", metavar="FLOAT", default=-1,
                    type=float, help="Set penalty for reversals, by default sidings with reversals are forbidden")
    ap.add_argument("--use-left", action="store_true", default=False, dest="useLeft",
                    help="Drive on the left instead of on the right")
    ap.add_argument("--add-stop-signals", action="store_true", default=False, dest="addStopSignals",
                    help="If a siding has a stop on the mainline, add signals in case they are missing")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="tell me what you are doing")
    options = ap.parse_args()

    options.routes = options.routes.split(',')
    if options.prefix is None:
        options.prefix = sumolib.miscutils.getBaseName(options.outfile)
    options.edges_file = options.prefix + ".edg.xml"
    options.nodes_file = options.prefix + ".nod.xml"
    return options


def filterNoSignalidings(options, net, sidings, noSignal):
    """keep only sidings that have a signal of that have a stop on the main track"""
    sidings2 = {}
    for main, (rid, fromIndex, siding) in sidings.items():
        for eid in main:
            if net.hasEdge(eid):
                e = net.getEdge(eid)
                b = e.getBidi()
                if b is not None and edgeUsage.get(b.getID(), 0) > 0:
                    sidings2[main] = (rid, fromIndex, siding)
                    break
    return sidings2;


def filterBidiSidings(options, net, sidings, edgeUsage):
    """keep only sidings where the main track is used in both directions"""
    sidings2 = {}
    for main, (rid, fromIndex, siding) in sidings.items():
        for eid in main:
            if net.hasEdge(eid):
                e = net.getEdge(eid)
                b = e.getBidi()
                if b is not None and edgeUsage.get(b.getID(), 0) > 0:
                    sidings2[main] = (rid, fromIndex, siding)
                    break
    return sidings2;


def getGeom(net, edges):
    result = []
    for eid in edges:
        if net.hasEdge(eid):
            e = net.getEdge(eid)
            result += e.getShape(True)
    return result


def isSidingRight(net, main, siding):
    geom = getGeom(net, main)
    geom2 = getGeom(net, siding)
    mainDir = sub(geom[-1], geom[0])
    directions = [crossProduct2D(mainDir, sub(p, geom[0])) for p in geom2]
    directions.sort()
    return directions[len(directions) // 2] < 0


def writePatch(options, net, sidings):
    rTypes = dict() # eid -> routingType
    for main, (rid, fromIndex, siding) in sidings.items():
        if isSidingRight(net, main, siding) != options.useLeft:
            rtMain = "0"
            rtSiding = "4"
        else:
            rtMain = "4"
            rtSiding = "0"
        for eid in main:
            rTypes[eid] = rtMain
        for eid in siding:
            rTypes[eid] = rtSiding

    rTypes2 = dict()
    for eid, rt in rTypes.items():
        if net.hasEdge(eid):
            e = net.getEdge(eid)
            b = e.getBidi()
            if b is not None:
                if b.getID() not in rTypes:
                    rTypes2[b.getID()] = rtMain if rt == rtSiding else rtSiding
    rTypes.update(rTypes2)

    with open(options.edges_file, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for eid in sorted(rTypes.keys()):
            routingType = rTypes[eid]
            outf.write('    <edge id="%s" priority="%s" routingType="%s"/>\n' % (
                eid, routingType, routingType))
        outf.write("</edges>\n")


def getUnique(baseID, stopIDs):
    index = 1
    sid = baseID + '#%s' % index
    while sid in stopIDs:
        index += 1
        sid = baseID + '#%s' % index
    stopIDs.add(sid)
    return sid


def getClosest(net, stopEdge, startPos, endPos, siding):
    e = net.getEdge(stopEdge)
    if startPos < 0:
        startPos += e.getLength()
    if endPos < 0:
        endPos += e.getLength()
    pos = (startPos + endPos) / 2
    halfLength = (endPos - startPos) / 2
    x,y = positionAtShapeOffset(e.getShape(True), pos)
    bestDist = 1e400
    bestEdge = None
    bestPos = None
    for sEdge in siding:
        se = net.getEdge(sEdge)
        sPos, dist = polygonOffsetAndDistanceToPoint((x,y), se.getShape(True))
        if dist < bestDist:
            bestDist = dist
            bestEdge = sEdge
            bestPos = sPos

    bestLength = net.getEdge(bestEdge).getLength()
    return bestEdge, max(0, bestPos - halfLength), min(bestPos + halfLength, bestLength)


def writeStops(options, net, sidings, stopIDs, stops):
    if options.stopfile and options.stopOutput:
        sidingDict = defaultdict(set)  # mainEdge -> [siding, ]
        for main, (rid, fromIndex, siding) in sidings.items():
            for e in main:
                if e in stops:
                    sidingDict[e].add(siding)

        with open(options.stopOutput, 'w') as outf:
            sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
            # copy existing stops
            for stopsOnEdge in stops.values():
                for stop in stopsOnEdge:
                    outf.write(stop.toXML('    '))

            for stopEdge in sorted(sidingDict.keys()):
                for stop in stops[stopEdge]:
                    sidingEdges = set()
                    for siding in sidingDict[stopEdge]:
                        sid = getUnique(stop.id, stopIDs)
                        sidingEdge, startPos, endPos = getClosest(net, stopEdge, float(stop.startPos), float(stop.endPos), siding)
                        if sidingEdge in sidingEdges:
                            continue
                        sidingEdges.add(sidingEdge)
                        name = ' name="%s"' % stop.name if stop.name else ''
                        lines = ' lines="%s"' % stop.lines if stop.lines else ''
                        outf.write('    <%s id="%s" lane="%s_0" startPos="%.2f" endPos="%.2f"%s%s/>\n' % (
                            stop.name, sid, sidingEdge, startPos, endPos, name, lines))
            outf.write("</additional>\n")


def parseStops(options):
    stopIDs = set()
    stops = defaultdict(list)  # stopEdge -> object
    if options.stopfile:
        for stop in sumolib.xml.parse(options.stopfile, ['busStop', 'trainStop']):
            stops[lane2edge(stop.lane)].append(stop)
            stopIDs.add(stop.id)
    return stopIDs, stops


def main(options):
    net = sumolib.net.readNet(options.netfile)
    noSignal = None
    extraArgs = []

    routes, edgeUsage = parseRoutes(options)
    stopIDs, stops = parseStops(options)
    # print("\n".join(map(str, routes.items())))
    switches = findSwitches(options, routes, net)
    # print("\n".join(map(str, switches.items())))
    sidings, sidingRoutes = findSidings(options, routes, switches, net, edgeUsage)
    # print("\n".join(map(str, sidings.items())))

    if options.addStopSignals:
        noSignal = set()

    sidings = filterSidings(options, net, sidings, noSignal)
    # print("\n".join(map(str, sidings.items())))

    if options.addStopSignals and stopIDs:
        sidings = filterNoSignalidings(options, net, sidings, noSignal, stops)
        extraArgs += ['-n', options.nodes_file]
        # print("\n".join(map(str, sidings.items())))

    sidings = filterBidiSidings(options, net, sidings, edgeUsage)
    # print("\n".join(map(str, sidings.items())))
    writePatch(options, net, sidings)
    writeStops(options, net, sidings, stopIDs, stops)

    if options.verbose:
        print("Building new net")
    sys.stderr.flush()

    args =[NETCONVERT,
           '-s', options.netfile,
           '-e', options.edges_file,
           '-o', options.outfile] + extraArgs

    if options.addStopSignals:
        add

    subprocess.call(args, stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    main(get_options())
