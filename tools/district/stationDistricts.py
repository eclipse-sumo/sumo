#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
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


def get_options():
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
    ap.add_option("--vclasses", default="rail,rail_urban", type=str,
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
    options = ap.parse_args()

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
            sum([c[0] for c in coords]) / len(coords),
            sum([c[1] for c in coords]) / len(coords))

        for edge, dist in net.getNeighboringEdges(station.coord[0], station.coord[1], options.parallelRadius):
            station.edges.add(edge)


def findGroup(mergedStations, station):
    for group in mergedStations:
        if station in group:
            return group
    assert(False)


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


def assignByDistance(options, net, stations):
    """assign edges to closest station"""
    edgeStation = dict()
    for station in stations.values():
        for edge in station.edges:
            assert(edge not in edgeStation or not options.merge)
            edgeStation[edge] = station.name

    remaining = set()
    for edge in net.getEdges():
        if edge not in edgeStation and allowsAny(edge, options.vclasses):
            remaining.add(edge)

    seen = set(edgeStation.keys())
    heap = []
    for edge, station in edgeStation.items():
        for neigh in chain(edge.getOutgoing().keys(), edge.getIncoming().keys()):
            if neigh not in seen:
                heappush(heap, (neigh.getLength(), neigh.getID(), station))

    while heap:
        dist, candID, station = heappop(heap)
        cand = net.getEdge(candID)
        seen.add(cand)
        if cand in remaining:
            stations[station].edges.add(cand)
            remaining.remove(cand)
        for neigh in chain(cand.getOutgoing().keys(), cand.getIncoming().keys()):
            if neigh not in seen:
                heappush(heap, (neigh.getLength() + dist, neigh.getID(), station))


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
