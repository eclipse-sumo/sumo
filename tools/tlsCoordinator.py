#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    tlsCoordinator.py
# @author  Martin Taraz (martin@taraz.de)
# @author  Jakob Erdmann
# @date    2015-09-07

from __future__ import absolute_import
from __future__ import print_function

import sys
import subprocess
from collections import namedtuple

import sumolib
from sumolib.output import parse_fast
from sumolib.options import ArgumentParser

TLTuple = namedtuple('TLTuple', ['edgeID', 'dist', 'time', 'connection'])
PairKey = namedtuple('PairKey', ['edgeID', 'edgeID2', 'dist'])
PairData = namedtuple('PairData', ['otl', 'oconnection', 'tl', 'connection', 'betweenOffset', 'startOffset',
                                   'travelTime', 'prio', 'numVehicles', 'ogreen', 'green'])


def pair2str(p, full=True):
    brief = "%s,%s s=%.1f b=%.1f t=%.1f" % (
        p.otl.getID(), p.tl.getID(), p.startOffset, p.betweenOffset, p.travelTime)
    if full:
        return brief + " og=%s g=%s p=%s n=%s" % (p.ogreen, p.green, p.prio, p.numVehicles)
    else:
        return brief


def logAddedPair(TLSP, sets, operation):
    print("added pair %s,%s with operation %s" %
          (TLSP.otl.getID(), TLSP.tl.getID(), operation))
    for s in sets:
        print("    " + "   ".join([pair2str(p, False) for p in s]))


def get_options(args=None):
    optParser = ArgumentParser()
    optParser.add_option("-n", "--net-file", category="input", dest="netfile", required=True,
                         type=ArgumentParser.net_file, help="define the net file (mandatory)")
    optParser.add_option("-o", "--output-file", category="output", dest="outfile",
                         default="tlsOffsets.add.xml", type=ArgumentParser.file, help="define the output filename")
    optParser.add_option("-r", "--route-file", category="input", dest="routefile", required=True,
                         type=ArgumentParser.route_file, help="define the input route file (mandatory)")
    optParser.add_option("-a", "--additional-file", category="input", dest="addfile",
                         type=ArgumentParser.additional_file, help="define replacement tls plans to be coordinated")
    optParser.add_option("-v", "--verbose", category="processing", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-i", "--ignore-priority", category="processing", dest="ignorePriority", action="store_true",
                         default=False, help="ignore road priority when sorting TLS pairs")
    optParser.add_option("--speed-factor", category="processing", type=float,
                         default=0.8, help="avg ratio of vehicle speed in relation to the speed limit")
    optParser.add_option("-e", "--evaluate", category="processing", action="store_true",
                         default=False, help="run the scenario and print duration statistics")
    optParser.add_option("--pair-output", category="output", dest="pairOutput",
                         type=ArgumentParser.file, help="write coordinated and halfCoordinated TLS pairs to XML file")
    return optParser.parse_args(args=args)


def locate(tlsToFind, sets):
    """return
        - the set in which the given traffic light exists
        - the pair in which it was found
        - the index within the pair
        """
    for s in sets:
        for pair in s:
            if tlsToFind == pair.otl:
                return s, pair, 0
            elif tlsToFind == pair.tl:
                return s, pair, 1
    return None, None, None


def coordinateAfterSet(TLSP, l1, l1Pair, l1Index):
    # print "coordinateAfter\n  TLSP: %s\n  l1Pair: %s\n  l1Index=%s" % (
    #        pair2str(TLSP), pair2str(l1Pair), l1Index)
    if l1Index == 0:
        TLSPdepart = l1Pair.startOffset - TLSP.ogreen
        TLSParrival = TLSPdepart + TLSP.travelTime
        TLSPstartOffset2 = TLSParrival - TLSP.green
        TLSP = TLSP._replace(startOffset=l1Pair.startOffset,
                             betweenOffset=TLSPstartOffset2 - l1Pair.startOffset)
    else:
        l1depart = l1Pair.startOffset + l1Pair.betweenOffset + TLSP.ogreen
        TLSParrival = l1depart + TLSP.travelTime
        TLSPstartOffset = TLSParrival - TLSP.green
        TLSP = TLSP._replace(startOffset=l1depart, betweenOffset=TLSPstartOffset - l1depart)
    l1.append(TLSP)
    return TLSP


def coordinateBeforeSet(TLSP, l2, l2Pair, l2Index):
    # print "coordinateBeforeSet\n  TLSP: %s\n  l2Pair: %s\n  l2Index=%s" % (
    #        pair2str(TLSP), pair2str(l2Pair), l2Index)
    if l2Index == 0:
        l2arrival = l2Pair.startOffset + TLSP.green
        TLSPdepart = l2arrival - TLSP.travelTime
        TLSPstartOffset = TLSPdepart - TLSP.ogreen
        TLSP = TLSP._replace(
            startOffset=TLSPstartOffset, betweenOffset=l2Pair.startOffset - TLSPstartOffset)
    else:
        l2arrival = l2Pair.startOffset + l2Pair.betweenOffset + TLSP.green
        TLSPdepart = l2arrival - TLSP.travelTime
        TLSPstartOffset = TLSPdepart - TLSP.ogreen
        TLSP = TLSP._replace(
            startOffset=TLSPstartOffset, betweenOffset=l2arrival - TLSPstartOffset)
    l2.append(TLSP)
    return TLSP


def computePairOffsets(TLSPList, verbose, pairRecords=None):
    c1, c2, c3, c4, c5 = 0, 0, 0, 0, 0
    sets = []  # sets of coordinate TLPairs
    operation = ""
    for TLSP in TLSPList:
        l1, l1Pair, l1Index = locate(TLSP.otl, sets)
        l2, l2Pair, l2Index = locate(TLSP.tl, sets)
        # print(l1)
        if l1 is None and l2 is None:
            # new set
            newlist = []
            newlist.append(TLSP)
            sets.append(newlist)
            c1 += 1
            operation = "newSet"
        elif l2 is None and l1 is not None:
            # add to set 1 - add after existing set
            TLSP = coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
            c2 += 1
            operation = "addAfterSet"
        elif l1 is None and l2 is not None:
            # add to set 2 - add before existing set
            TLSP = coordinateBeforeSet(TLSP, l2, l2Pair, l2Index)
            c3 += 1
            operation = "addBeforeSet"
        else:
            if l1 == l2:
                # cannot uncoordinated both tls. coordinate the first
                # arbitrarily
                TLSP = coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                c4 += 1
                operation = "addHalfCoordinated"
            else:
                # merge sets
                TLSP = coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                if verbose:
                    logAddedPair(TLSP, sets, "addAfterSet (intermediate)")

                # print "merge\n  TLSP: %s\n  l1Pair: %s\n  l1Index=%s\n  l2Pair: %s\n  l2Index=%s" % (
                # pair2str(TLSP), pair2str(l1Pair), l1Index, pair2str(l2Pair),
                # l2Index)

                if l2Index == 0:
                    dt = TLSP.startOffset + \
                        TLSP.betweenOffset - l2Pair.startOffset
                else:
                    dt = TLSP.startOffset + TLSP.betweenOffset - \
                        (l2Pair.startOffset + l2Pair.betweenOffset)

                merge(sets, l1, l2, dt)
                c5 += 1
                operation = "mergeSets"

        if pairRecords is not None:
            status = "halfCoordinated" if operation == "addHalfCoordinated" else "coordinated"
            pairRecords.append((TLSP, status, operation))

        if verbose:
            logAddedPair(TLSP, sets, operation)

    print("operations: newSet=%s addToSet=%s addToSet2=%s addHalfCoordinated=%s mergeSets=%s" % (
        c1, c2, c3, c4, c5))
    return sets


def merge(sets, list1, list2, dt):
    for elem in list2:
        list1.append(elem._replace(startOffset=elem.startOffset + dt))
    sets.remove(list2)


def finalizeOffsets(sets):
    offsetDict = {}
    for singleSet in sets:
        singleSet.sort(
            key=lambda pd: (pd.prio, pd.numVehicles / pd.travelTime), reverse=True)
        for pair in singleSet:
            # print "   %s,%s:%s,%s" % (pair.otl.getID(), pair.tl.getID(),
            # pair.startOffset, pair.betweenOffset)
            tl1 = pair.otl.getID()
            tl2 = pair.tl.getID()
            betweenOffset = pair.betweenOffset
            startOffset = pair.startOffset
            if tl1 not in offsetDict:
                # print "     added %s offset %s" % (tl1, startOffset)
                offsetDict[tl1] = startOffset
            if tl2 not in offsetDict:
                # print "     added %s offset %s" % (tl2, startOffset +
                # betweenOffset)
                offsetDict[tl2] = startOffset + betweenOffset
    return offsetDict


def getTLSInRoute(net, edge_ids):
    rTLSList = []  # list of traffic lights along the current route
    dist = 0
    time = 0
    edgesSeen = set()
    for edgeID, nextEdgeID in zip(edge_ids[:-1], edge_ids[1:]):
        edge = net.getEdge(edgeID)
        nextEdge = net.getEdge(nextEdgeID)
        if nextEdge not in edge.getOutgoing():
            sys.stderr.write("Warning: Skipping disconnected route (edges %s, %s)\n" % (edgeID, nextEdgeID))
            return []
        connection = edge.getOutgoing()[nextEdge][0]

        TLS = None if edge.getToNode().getType() in ("rail_crossing", "rail_signal") else edge.getTLS()
        dist += edge.getLength()
        time += edge.getLength() / edge.getSpeed()
        if TLS and edgeID not in edgesSeen:
            rTLSList.append(TLTuple(edgeID, dist, time, connection))
            edgesSeen.add(edgeID)
            dist = 0
            time = 0
    return rTLSList


def getFirstGreenOffset(tl, connection):
    index = connection.getTLLinkIndex()
    tlp = tl.getPrograms()
    if len(tlp) != 1:
        raise RuntimeError("Found %s programs for tl %s" %
                           (len(tlp), connection._tls))
    phases = list(tlp.values())[0].getPhases()
    start = 0
    for p in phases:
        if p.state[index] in ['G', 'g']:
            return start
        else:
            start += p.duration
    raise RuntimeError(
        "No green light for tlIndex %s at tl %s" % (index, connection._tls))


def getTLPairs(net, routeFile, speedFactor, ignorePriority):
    # pairs of traffic lights
    TLPairs = {}  # PairKey -> PairData

    for route in parse_fast(routeFile, 'route', ['edges']):
        rTLSList = getTLSInRoute(net, route.edges.split())

        for oldTL, TLelement in zip(rTLSList[:-1], rTLSList[1:]):
            tl = net.getEdge(TLelement.edgeID).getTLS()
            otl = net.getEdge(oldTL.edgeID).getTLS()
            if otl == tl:
                continue
            key = PairKey(oldTL.edgeID, TLelement.edgeID, TLelement.dist)
            numVehicles = 0 if key not in TLPairs else TLPairs[key].numVehicles

            edge = net.getEdge(TLelement.edgeID)
            connection = TLelement.connection
            oconnection = oldTL.connection

            ogreen = getFirstGreenOffset(otl, oconnection)
            green = getFirstGreenOffset(tl, connection)

            travelTime = TLelement.time / speedFactor
            betweenOffset = travelTime + ogreen - green
            startOffset = 0
            # relevant data for a pair of traffic lights
            prio = 1 if ignorePriority else edge.getPriority()
            TLPairs[key] = PairData(otl, oconnection, tl, connection, betweenOffset, startOffset, travelTime,
                                    prio, numVehicles + 1, ogreen, green)

    return TLPairs


def removeDuplicates(TLPairs):
    # Merge multiple pairs connecting the same ordered (otl, tl) pair:
    # first combine identical (edgeID, edgeID2) with different dist by keeping the one with largest numVehicles
    # while summing counts, then keep the dominant pair per ordered (otl, tl) pair.
    by_edges = {}
    for key, data in TLPairs.items():
        ekey = (key.edgeID, key.edgeID2)
        if ekey not in by_edges:
            by_edges[ekey] = (key, data, data.numVehicles)
        else:
            best_key, best_data, total_veh = by_edges[ekey]
            total_veh += data.numVehicles
            if (data.numVehicles, -data.travelTime) > (best_data.numVehicles, -best_data.travelTime):
                best_key, best_data = key, data
            by_edges[ekey] = (best_key, best_data, total_veh)

    by_tls = {}
    for _, (key, data, total_veh) in by_edges.items():
        data = data._replace(numVehicles=total_veh)
        tkey = (data.otl.getID(), data.tl.getID())
        if tkey not in by_tls:
            by_tls[tkey] = (key, data)
        else:
            best_key, best_data = by_tls[tkey]
            score_new = (data.prio, data.numVehicles / max(data.travelTime, 1e-6), data.numVehicles, key)
            score_old = (best_data.prio, best_data.numVehicles / max(best_data.travelTime, 1e-6),
                         best_data.numVehicles, best_key)
            merged_veh = best_data.numVehicles + data.numVehicles
            if score_new > score_old:
                by_tls[tkey] = (key, data._replace(numVehicles=merged_veh))
            else:
                by_tls[tkey] = (best_key, best_data._replace(numVehicles=merged_veh))

    return dict(by_tls.values())


def writePairOutput(outfile, pairRecords, options):
    with open(outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="tlsPairs", rootAttrs=None, options=options)
        for pair, status, operation in pairRecords:
            fromEdge = pair.oconnection.getFrom().getID()
            toEdge = pair.connection.getFrom().getID()
            outf.write(('    <pair from="%s" to="%s" fromEdge="%s" toEdge="%s" status="%s" operation="%s" '
                        'numVehicles="%s" travelTime="%.2f" startOffset="%.2f" betweenOffset="%.2f"/>\n') % (
                pair.otl.getID(), pair.tl.getID(), fromEdge, toEdge, status, operation,
                pair.numVehicles, pair.travelTime, pair.startOffset, pair.betweenOffset))
        outf.write('</tlsPairs>\n')


def main(options):
    net = sumolib.net.readNet(options.netfile, withLatestPrograms=True)
    if options.addfile is not None:
        sumolib.net.readNet(options.addfile, withLatestPrograms=True, net=net)

    TLPairs = getTLPairs(net, options.routefile, options.speed_factor, options.ignorePriority)
    TLPairs = removeDuplicates(TLPairs)

    sortHelper = [(
        (pairData.prio, pairData.numVehicles / pairData.travelTime),  # sortKey
        (pairKey, pairData))  # payload
        for pairKey, pairData in TLPairs.items()]

    tlPairsList = [
        value for sortKey, value in sorted(sortHelper, reverse=True)]

    print("number of tls-pairs: %s" % len(tlPairsList))
    if options.verbose:
        print('\n'.join(["edges=%s,%s prio=%s numVehicles/time=%s" % (
            pairKey.edgeID, pairKey.edgeID2, pairData.prio, pairData.numVehicles / pairData.travelTime)
            for pairKey, pairData in tlPairsList]))

    pairRecords = [] if options.pairOutput else None
    coordinatedSets = computePairOffsets(
        [pairData for pairKey, pairData in tlPairsList], options.verbose, pairRecords)

    if options.pairOutput:
        writePairOutput(options.pairOutput, pairRecords, options)

    offsetDict = finalizeOffsets(coordinatedSets)

    with open(options.outfile, 'w') as outf:
        sumolib.xml.writeHeader(outf, root="additional", options=options)
        for ID, startOffset in sorted(offsetDict.items()):
            programID = list(net.getTLSSecure(ID).getPrograms().keys())[0]
            outf.write('    <tlLogic id="%s" programID="%s" offset="%.2f"/>\n' %
                       (ID, programID, startOffset))
        outf.write('</additional>\n')

    sumo = sumolib.checkBinary('sumo')
    if options.evaluate:
        additionals = [options.outfile]
        if options.addfile:
            additionals = [options.addfile] + additionals
        subprocess.call([sumo,
                         '-n', options.netfile,
                         '-r', options.routefile,
                         '-a', ','.join(additionals),
                         '-v', '--no-step-log', '--duration-log.statistics'], stdout=sys.stdout)


if __name__ == "__main__":
    options = get_options()
    main(options)
