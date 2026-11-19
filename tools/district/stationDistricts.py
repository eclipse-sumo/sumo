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

# @file    stationDistricts.py
# @author  Jakob Erdmann
# @date    2023-02-22

"""
Segment a (railway) network in districts based on nearby stations
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import random
from heapq import heappush, heappop
from collections import defaultdict
from itertools import chain
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import sumolib  # noqa
from sumolib.options import ArgumentParser  # noqa
from sumolib.miscutils import Colorgen, openz  # noqa
import sumolib.geomhelper as gh  # noqa


def get_options(args=None):
    ap = ArgumentParser()
    ap.add_option("-v", "--verbose", action="store_true", default=False,
                  help="tell me what you are doing")
    ap.add_option("-n", "--net-file", dest="netfile", required=True, category="input", type=ap.net_file,
                  help="the network to read lane and edge permissions")
    ap.add_option("-s", "--stop-file", dest="stopfile", required=True, category="input", type=ap.additional_file,
                  help="the additional file with stops")
    ap.add_option("-o", "--output", required=True, category="output", type=ap.additional_file,
                  help="output taz file")
    ap.add_option("--split-output", dest="splitOutput", category="output", type=ap.file,
                  help="generate splits for edges assigned to multiple stations")
    ap.add_option("--poi-output", dest="poiOutput", category="output", type=ap.file,
                  help="generate a point of interest for every station")
    ap.add_option("--vclasses", default="rail,rail_urban,subway", type=str,
                  help="Include only edges allowing VCLASS")
    ap.add_option("--parallel-radius", type=float, default=100, dest="parallelRadius",
                  help="search radius for finding parallel edges")
    ap.add_option("--merge", action="store_true", default=False,
                  help="merge stations that have a common edge")
    ap.add_option("--hue", default="random", type=str,
                  help="hue for taz (float from [0,1] or 'random')")
    ap.add_option("--saturation", default=1, type=str,
                  help="saturation for taz (float from [0,1] or 'random')")
    ap.add_option("--brightness", default=1, type=str,
                  help="brightness for taz (float from [0,1] or 'random')")
    ap.add_option("--seed", type=int, default=42, help="random seed")
    options = ap.parse_args(args=args)

    if not options.netfile:
        ap.print_help()
        ap.exit("Error! setting net-file is mandatory")
    if not options.stopfile:
        ap.print_help()
        ap.exit("Error! setting stop-file is mandatory")
    if not options.output:
        ap.print_help()
        ap.exit("Error! setting output is mandatory")

    options.vclasses = options.vclasses.split(',')
    options.colorgen = Colorgen((options.hue, options.saturation, options.brightness))
    return options


class Station:
    def __init__(self):
        self.edges = set()
        self.name = None
        self.platforms = []
        self.coord = None

    def write(self, outf, outf_poi, index, color):
        outf.write(u'    <taz id="%s" name="%s" color="%s" edges="%s">\n' %
                   (index, self.name, color, ' '.join(sorted([e.getID() for e in self.edges]))))
        if self.coord:
            outf.write(u'       <param key="coord" value="%s"/>\n' % ' '.join(map(str, self.coord)))
        outf.write(u'    </taz>\n')

        if self.coord and outf_poi:
            outf_poi.write(u'    <poi id="%s" name="%s" x="%s" y="%s"/>\n' %
                           (index, self.name, self.coord[0], self.coord[1]))


def allowsAny(edge, vclasses):
    for vclass in vclasses:
        if edge.allows(vclass):
            return True
    return False


def initStations(options, net):
    stations = defaultdict(Station)

    numStops = 0
    numIgnoredStops = 0
    for stop in sumolib.xml.parse(options.stopfile, ['busStop', 'trainStop']):
        name = stop.getAttributeSecure("attr_name", stop.id)
        edgeID = stop.lane.rsplit('_', 1)[0]
        if not net.hasEdge(edgeID):
            sys.stderr.write("Unknown edge '%s' in stop '%s'" % (edgeID, stop.id))
            continue
        edge = net.getEdge(edgeID)
        if not allowsAny(edge, options.vclasses):
            numIgnoredStops += 1
            continue
        station = stations[name]
        station.edges.add(edge)
        station.name = name
        begCoord = sumolib.geomhelper.positionAtShapeOffset(edge.getShape(), float(stop.startPos))
        endCoord = sumolib.geomhelper.positionAtShapeOffset(edge.getShape(), float(stop.endPos))
        station.platforms.append([begCoord, endCoord])
        numStops += 1

    if options.verbose:
        print("Read %s stops and %s stations" % (numStops, len(stations)))
        if numIgnoredStops:
            print("Ignored %s stops because they did not allow any of the vclasses '%s'" % (
                numIgnoredStops, ','.join(options.vclasses)))

    return stations


def findParallel(options, net, stations):
    for station in stations.values():
        coords = sum(station.platforms, [])
        station.coord = (
            round(sum([c[0] for c in coords]) / len(coords), 3),
            round(sum([c[1] for c in coords]) / len(coords), 3))

        for edge, dist in net.getNeighboringEdges(station.coord[0], station.coord[1], options.parallelRadius):
            if allowsAny(edge, options.vclasses):
                station.edges.add(edge)


def findGroup(mergedStations, station):
    for group in mergedStations:
        if station in group:
            return group
    assert False


def mergeGroups(stations, mergedStations, group1, group2):
    if group1 is None or group1 == group2:
        return group2

    name1 = '|'.join(sorted(group1))
    name2 = '|'.join(sorted(group2))
    mergedGroup = group1
    mergedGroup.update(group2)
    mergedName = '|'.join(sorted(group1))
    mergedStations.remove(group2)

    station1 = stations[name1]
    station2 = stations[name2]
    mergedStation = Station()
    mergedStation.name = mergedName
    mergedStation.edges.update(station1.edges)
    mergedStation.edges.update(station2.edges)
    mergedStation.platforms = station1.platforms + station2.platforms
    del stations[name1]
    del stations[name2]
    stations[mergedName] = mergedStation
    return mergedGroup


def mergeStations(stations, verbose=False):
    """merge stations with shared edges"""

    initialStations = len(stations)
    edgeStation = defaultdict(set)
    for station in stations.values():
        for edge in station.edges:
            edgeStation[edge].add(station.name)

    mergedStations = [set([name]) for name in stations.keys()]

    for edge, stationNames in edgeStation.items():
        mergedGroup = None
        for station in stationNames:
            mergedGroup = mergeGroups(stations, mergedStations, mergedGroup, findGroup(mergedStations, station))

    finalStations = len(stations)
    if verbose and finalStations != initialStations:
        print("Merged %s stations" % (initialStations - finalStations))


def splitStations(options, stations):
    edgeStation = defaultdict(set)
    for station in stations.values():
        for edge in station.edges:
            edgeStation[edge].add(station)

    bidiSplits = defaultdict(list)
    with openz(options.splitOutput, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "edges", schemaPath="edgediff_file.xsd", options=options)
        for edge, stations in sorted(edgeStation.items(), key=lambda e: e[0].getID()):
            if len(stations) == 1:
                continue
            shape = edge.getShape(True)
            stationOffsets = []
            for station in stations:
                offset = gh.polygonOffsetWithMinimumDistanceToPoint(station.coord, shape, perpendicular=False)
                stationOffsets.append((offset, station.name))
            stationOffsets.sort()

            splits = []
            for (o1, n1), (o2, n2) in zip(stationOffsets[:-1], stationOffsets[1:]):
                if o1 != o2:
                    pos = (o1 + o2) / 2
                    splits.append((pos, "%s.%s" % (edge.getID(), int(pos))))
                else:
                    sys.stderr.write("Cannot split edge '%s' between stations '%s' and '%s'\n" % (
                        edge.getID(), n1, n2))

            if edge.getBidi():
                if edge.getBidi() in bidiSplits:
                    bidiLength = edge.getBidi().getLength()
                    splits = [(bidiLength - p, n) for p, n in reversed(bidiSplits[edge.getBidi()])]
                else:
                    bidiSplits[edge] = splits

            outf.write(u'    <edge id="%s">\n' % edge.getID())
            for pos, nodeID in splits:
                outf.write(u'        <split pos="%s" id="%s"/>\n' % (pos, nodeID))
            outf.write(u'    </edge>\n')

        outf.write(u"</edges>\n")


def _tangent_deg(shape, offset):
    plen = gh.polyLength(shape)
    if plen <= 0:
        return None
    rot = gh.rotationAtShapeOffset(shape, min(max(0.0, offset), max(0.0, plen - 1e-3)))
    return gh.naviDegree(rot) if rot is not None else None


def _is_parallel_angle(d1, d2, atol=20.0):
    if d1 is None or d2 is None:
        return False
    diff = gh.minAngleDegreeDiff(d1, d2)
    return diff <= atol or abs(diff - 180.0) <= atol


def assignByDistance(options, net, stations):
    """assign edges to closest station while keeping parallel tracks in the same TAZ"""
    allowedEdges = [e for e in net.getEdges() if allowsAny(e, options.vclasses)]
    midInfo = {}
    for edge in allowedEdges:
        shape = edge.getShape()
        plen = gh.polyLength(shape)
        mid = gh.positionAtShapeOffset(shape, 0.5 * plen)
        midInfo[edge] = (shape, plen, mid, _tangent_deg(shape, 0.5 * plen))

    lateralRadius = min(30.0, options.parallelRadius) if options.parallelRadius > 0 else 0.0
    lateralFrom = defaultdict(list)
    parallelPeers = defaultdict(list)
    if lateralRadius > 0:
        for peer in allowedEdges:
            _, _, midP, degP = midInfo[peer]
            for cand, _ in net.getNeighboringEdges(midP[0], midP[1], lateralRadius):
                if cand == peer or cand not in midInfo:
                    continue
                shapeC, _, _, _ = midInfo[cand]
                coff, cdist = gh.polygonOffsetAndDistanceToPoint(midP, shapeC, perpendicular=True)
                if coff != gh.INVALID_DISTANCE and cdist <= lateralRadius:
                    if _is_parallel_angle(degP, _tangent_deg(shapeC, coff)):
                        lateralFrom[cand].append((peer, coff, cdist))
                        parallelPeers[peer].append((cand, cdist))

    edgeStations = defaultdict(set)
    seedEdges = set()
    for station in stations.values():
        for edge in station.edges:
            assert edge not in edgeStations or not options.merge
            edgeStations[edge].add(station.name)
            seedEdges.add(edge)

    remaining = {e for e in allowedEdges if e not in seedEdges}
    heap = []
    for edge, stNames in edgeStations.items():
        shapeE = edge.getShape(True)
        lenE = edge.getLength()
        for stName in sorted(stNames):
            stObj = stations[stName]
            stCoords = [stObj.coord] if stObj.coord is not None else sum(stObj.platforms, [])
            offs = [gh.polygonOffsetWithMinimumDistanceToPoint(c, shapeE, perpendicular=False) for c in stCoords]
            dFrom = min(offs) if offs else 0.0
            dTo = min(max(0.0, lenE - o) for o in offs) if offs else 0.0
            for neigh in edge.getIncoming().keys():
                if neigh in remaining:
                    heappush(heap, (dFrom + 0.5 * neigh.getLength(), neigh.getID(), stName, dFrom, True))
            for neigh in edge.getOutgoing().keys():
                if neigh in remaining:
                    heappush(heap, (dTo + 0.5 * neigh.getLength(), neigh.getID(), stName, dTo, True))
            if len(stNames) == 1:
                for peer, coff, _ in lateralFrom[edge]:
                    if peer in remaining:
                        latDist = min(abs(coff - o) for o in offs) if offs else 0.0
                        heappush(heap, (latDist, peer.getID(), stName, latDist, False))

    assigned = {}
    while heap:
        midDist, candID, stName, entryDist, isTopo = heappop(heap)
        cand = net.getEdge(candID)
        if cand not in remaining:
            continue
        remaining.remove(cand)
        stations[stName].edges.add(cand)
        assigned[cand] = (stName, midDist)
        farDist = (entryDist + cand.getLength()) if isTopo else (midDist + 0.5 * cand.getLength())
        for neigh in chain(cand.getOutgoing().keys(), cand.getIncoming().keys()):
            if neigh in remaining:
                heappush(heap, (farDist + 0.5 * neigh.getLength(), neigh.getID(), stName, farDist, True))
        for peer, _, _ in lateralFrom[cand]:
            if peer in remaining:
                heappush(heap, (midDist, peer.getID(), stName, max(0.0, midDist - 0.5 * peer.getLength()), False))

    alignDist = min(20.0, lateralRadius)
    changed = True
    while changed and alignDist > 0:
        changed = False
        for edge in sorted(assigned.keys(), key=lambda x: (x.getLength(), x.getID())):
            curSt = assigned[edge][0]
            group = [edge] + [
                c for c, d in parallelPeers[edge]
                if d <= alignDist and (c in assigned or len(edgeStations[c]) == 1)
            ]
            if len(group) <= 1:
                continue
            def _rank(g):
                if g in seedEdges and len(edgeStations[g]) == 1:
                    return (0, 0.0, -g.getLength(), g.getID())
                return (1, assigned[g][1], -g.getLength(), g.getID())

            canon = min(group, key=_rank)
            if canon in seedEdges and len(edgeStations[canon]) == 1:
                targetSt = next(iter(edgeStations[canon]))
            else:
                targetSt = assigned[canon][0] if canon in assigned else curSt
            if targetSt != curSt:
                stations[curSt].edges.remove(edge)
                stations[targetSt].edges.add(edge)
                assigned[edge] = (targetSt, assigned[edge][1])
                changed = True


def main(options):
    random.seed(options.seed)

    if options.verbose:
        print("Reading net")
    net = sumolib.net.readNet(options.netfile)

    stations = initStations(options, net)
    findParallel(options, net, stations)
    if options.merge:
        mergeStations(stations, options.verbose)
    elif options.splitOutput:
        splitStations(options, stations)
    assignByDistance(options, net, stations)

    outf_poi = None
    if options.poiOutput:
        outf_poi = openz(options.poiOutput, 'w')
        sumolib.writeXMLHeader(outf_poi, "$Id$", "additional", options=options)

    with openz(options.output, 'w') as outf:
        sumolib.writeXMLHeader(outf, "$Id$", "additional", options=options)
        for i, name in enumerate(sorted(stations.keys())):
            stations[name].write(outf, outf_poi, i, options.colorgen())
        outf.write(u"</additional>\n")

    if outf_poi:
        outf_poi.close()


if __name__ == "__main__":
    main(get_options())
