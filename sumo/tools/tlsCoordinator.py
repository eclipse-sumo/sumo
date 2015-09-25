#!/usr/bin/env python
"""
@file    tlsCoordinator.py
@author  Martin Taraz (martin@taraz.de)
@date    2015-09-07
@version $Id$

Coordinates traffic lights in a sumo net for a given demand

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, sys
import subprocess
import xml.etree.cElementTree as ET

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    import sumolib
else:   
    sys.exit("please declare environment variable 'SUMO_HOME'")

from sumolib.miscutils import Statistics
from sumolib.output import parse_fast
from operator import itemgetter
from collections import namedtuple

TLTuple = namedtuple('TLTuple', ['edgeID', 'dist', 'time', 'connection'])
PairKey = namedtuple('PairKey', ['edgeID', 'edgeID2', 'dist'])
PairData = namedtuple('PairData', ['otl', 'oconnection', 'tl', 'connection', 'betweenOffset', 'startOffset',
                                   'prio', 'timeBetween', 'numVehicles', 'ogreen', 'green'])

def locate(tlsToFind, sets):
    """return 
        - the set in which the given traffic light exists 
        - the pair in which it was found
        - the index within the pair
        """
    for s in sets:
        for pair in s:
            for i, tls in enumerate(pair):
                if tls == tlsToFind:
                    return s, pair, i
    return None, None, None


def coordinateAfterSet(TLSP, l1, l1Pair, l1Index):
    #print "add ", TLSP.otl.getID(),TLSP.tl.getID(), " to ", l1Pair.otl.getID(),l1Pair.tl.getID(), " l1green ", l1Pair.green, " tlsp.ogreen", TLSP.ogreen
    if l1Index == 0:
        TLSP = TLSP._replace(
                startOffset=l1Pair.startOffset, 
                betweenOffset=TLSP.betweenOffset - l1Pair.green + TLSP.ogreen - TLSP.green)
    else:
        TLSP = TLSP._replace(
                startOffset=l1Pair.startOffset + l1Pair.betweenOffset,
                betweenOffset=TLSP.betweenOffset - l1Pair.green + TLSP.ogreen - TLSP.green)
    l1.append(TLSP)


def coordinateBeforeSet(TLSP, l2, l2Pair, l2Index):
    if l2Index == 0:
        TLSP = TLSP._replace(
                startOffset=l2Pair.startOffset - betweenOffset,
                betweenOffset=TLSP.betweenOffset + l2Pair.ogreen + TLSP.ogreen - TLSP.green)
    else:
        TLSP = TLSP._replace(
                startOffset=l2Pair.startOffset + l2Pair.betweenOffset - betweenOffset,
                betweenOffset=TLSP.betweenOffset + l2Pair.ogreen + TLSP.ogreen - TLSP.green)
    l2.append(TLSP)

                       
def computeOffsets(TLSPList):
    c1, c2 , c3 , c4 , c5 = 0 , 0 , 0 , 0 , 0
    sets = []
    operation = ""
    for TLSP in TLSPList:
        betweenOffset = TLSP.betweenOffset
        startOffset = TLSP.startOffset
        l1, l1Pair, l1Index = locate(TLSP.otl, sets)
        l2, l2Pair, l2Index = locate(TLSP.tl, sets)
        #print(l1)
        if l1 == None and l2 == None:
            # new set
            newlist = []
            newlist.append(TLSP)
            sets.append(newlist)
            c1 += 1
            operation = "newSet"
        elif l2 == None and not l1 == None:
            # add to set 1 - add after existing set
            coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
            c2 += 1
            operation = "addToSet"
        elif l1 == None and not l2 == None:
            # add to set 2 - add before existing set
            coordinateBeforeSet(TLSP, l2, l2Pair, l2Index)
            c3 += 1
            operation = "addToSet2"
        else:
            if l1 == l2:
                # cannot uncoordinated both tls. coordinate the first arbitrarily
                coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                c4 +=1
                operation = "addHalfCoordinated"
            else:
                # merge sets
                coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                #for s in sets:
                #    print "       ", ["%s,%s:%s,%s" % (pd.otl.getID(), pd.tl.getID(), pd.startOffset, pd.betweenOffset) for pd in s]

                if l1Index == 0:
                    if l2Index == 0:
                        #print "    case a)"
                        dt = l1Pair.startOffset                        + betweenOffset - l2Pair.startOffset
                    else:
                        #print "    case b)"
                        dt = l1Pair.startOffset + l1Pair.betweenOffset + betweenOffset - l2Pair.startOffset
                else:
                    if l2Index == 0:
                        #print "    case c)", l2Pair.green, TLSP.ogreen
                        dt = l1Pair.startOffset                        + betweenOffset - (l2Pair.startOffset + l2Pair.betweenOffset) + l2Pair.green - TLSP.ogreen
                    else:
                        #print "    case d)"
                        dt = l1Pair.startOffset + l1Pair.betweenOffset + betweenOffset - (l2Pair.startOffset + l2Pair.betweenOffset)

                merge(sets, l1, l2, dt)
                c5 += 1
                operation = "mergeSets"
        #print(TLSP[4])

        #print "added pair %s,%s with operation %s" % (TLSP.otl.getID(), TLSP.tl.getID(), operation)
        #for s in sets:
            #print "   ", ["%s,%s:%s,%s" % (pd.otl.getID(), pd.tl.getID(), pd.startOffset, pd.betweenOffset) for pd in s]
            #print "  ", ["%s,%s" % (pd.otl.getID(), pd.tl.getID()) for pd in s]

    print("operations: newSet=%s addToSet=%s addToSet2=%s addHalfCoordinated=%s mergeSets=%s" % (c1, c2 , c3 , c4, c5))
    return(sets)


def merge(sets, list1 , list2 , dt):
    for elem in list2:
        list1.append(elem._replace(startOffset=elem.startOffset + dt))
    sets.remove(list2)


def setsToDic(sets):
    TLSPDic = {}
    for singleSet in sets:
        singleSet.sort(key=lambda pd:(pd.prio, pd.numVehicles / pd.timeBetween), reverse=True)
        for pair in singleSet:
            #print "   %s,%s:%s,%s" % (pair.otl.getID(), pair.tl.getID(), pair.startOffset, pair.betweenOffset)
            tl1 = pair.otl.getID()
            tl2 = pair.tl.getID()       
            betweenOffset = pair.betweenOffset
            startOffset = pair.startOffset
            if not tl1 in TLSPDic:
                TLSPDic[tl1] = startOffset
            if not tl2 in TLSPDic:
                TLSPDic[tl2] = startOffset + betweenOffset
    return TLSPDic


def getTLSInRoute(net, edge_ids):
    rTLSList = [] # list of traffic lights along the current route
    dist = 0
    time = 0
    for edgeID, nextEdgeID in zip(edge_ids[:-1], edge_ids[1:]):
        edge = net.getEdge(edgeID)
        nextEdge = net.getEdge(nextEdgeID)
        connection = edge.getOutgoing()[nextEdge][0]

        TLS = edge.getTLS()
        dist += edge.getLength()
        time += edge.getLength() / edge.getSpeed()
        alreadyFound = [item for item in rTLSList if item[0] == edgeID]
        if TLS and not alreadyFound:
            rTLSList.append(TLTuple(edgeID, dist , time, connection))
            dist = 0
            time = 0
    return rTLSList


def getFirstGreenOffset(tl, connection):
    index = connection._tlLink
    tlp = tl.getPrograms()
    if len(tlp) != 1:
        raise RuntimeError("Found %s programs for tl %s" % (len(tlp), connection._tls))
    phases = tlp.values()[0].getPhases()
    start = 0
    for state, duration in phases:
        if state[index] in ['G', 'g']:
            return start
        else:
            start += duration
    raise RuntimeError("No green light for tlIndex %s at tl %s" % (index, connection._tls))


def routeToDic(net, routeFile):
    # pairs of traffic lights
    TLPairs = {} # PairKey -> PairData

    for route in sumolib.output.parse_fast(routeFile, 'route', ['edges']):
        rTLSList = getTLSInRoute(net, route.edges.split())

        for oldTL, TLelement in zip(rTLSList[:-1], rTLSList[1:]):
            key = PairKey(oldTL.edgeID, TLelement.edgeID, oldTL.dist)
            numVehicles = 0 if not key in TLPairs else TLPairs[key].numVehicles

            tl = net.getEdge(TLelement.edgeID).getTLS()
            otl = net.getEdge(oldTL.edgeID).getTLS()
            edge = net.getEdge(TLelement.edgeID)
            connection = TLelement.connection
            oconnection = oldTL.connection

            ogreen = getFirstGreenOffset(otl, oconnection)
            green = getFirstGreenOffset(tl, connection)

            betweenOffset = TLelement.time + ogreen - green
            startOffset = 0
            # relevant data for a pair of traffic lights
            TLPairs[key] = PairData(otl, oconnection,  tl, connection, betweenOffset, startOffset,
                    edge.getPriority(), TLelement.time, numVehicles + 1, ogreen, green)

    return TLPairs

def removeDuplicates(TLPairs):
    # @todo: for multiple pairs with the same edges but different dist, keep only the one with the largest numVehicles
    return TLPairs




def main(netfile1, demand, outfile):
    net = sumolib.net.readNet(netfile1, withPrograms = True)
    TLPairs = routeToDic(net, demand)
    TLPairs = removeDuplicates(TLPairs)

    sortHelper = [(
        (pairData.prio, pairData.numVehicles / pairData.timeBetween), # sortKey
        (pairKey, pairData)) # payload 
        for pairKey, pairData in TLPairs.items()]

    tlPairsList = [value for sortKey, value in sorted(sortHelper, reverse=True)]


    print("number of tls-pairs: %s" % (len(tlPairsList)))
    #print '\n'.join(["edges=%s,%s prio=%s numVehicles/time=%s" % (
    #    pairKey.edgeID, pairKey.edgeID2, pairData.prio, pairData.numVehicles / pairData.timeBetween) 
    #    for pairKey, pairData in tlPairsList])

    sets = computeOffsets([pairData for pairKey, pairData in tlPairsList])

    TLSPDic = setsToDic(sets)
    netfile2 = 'temp.'+ str(netfile1)

    with open(outfile, 'w') as outf:
        outf.write('<additional>\n')
        for ID, betweenOffset in TLSPDic.items():
            outf.write('    <tlLogic id="%s" programID="0" offset="%s"/>\n' %
                    (ID, str(betweenOffset)))
        outf.write('</additional>\n')

    sumo = sumolib.checkBinary('sumo')
    netconvert = sumolib.checkBinary('netconvert')
    #subprocess.call([netconvert, '-s', netfile1, '-o', netfile1])
    #subprocess.call(['build.bat'])
    #subprocess.call([netconvert, '-s', netfile1, '--tllogic-files', outfile, '-o', netfile2])
    #subprocess.call([sumo, '-c','osm.sumocfg', '-n', netfile1,  '--tripinfo-output',  'tripinfos.xml'])
    #subprocess.call([sumo, '-c','osm.sumocfg', '-n', netfile2, '--tripinfo-output', 'tripinfos2.xml'])

    #durationStats = Statistics(' Traveltimes')
    #for trip in parse_fast('tripinfos.xml', 'tripinfo', ['id', 'duration']):
    #   durationStats.add(float(trip.duration), trip.id)
    #print durationStats
    #durationStats1 = Statistics(' Traveltimes')
    #for trip in parse_fast('tripinfos2.xml', 'tripinfo', ['id', 'duration']):
    #   durationStats1.add(float(trip.duration), trip.id)
    #print durationStats1

if __name__ == "__main__":
    main(*sys.argv[1:])
