#!/usr/bin/env python
"""
@file    tlsCoordinator.py
@author  Martin Taraz (martin@taraz.de)
@date    2015-09-07
@version $Id: tlsCoordinator.py 18756 2015-08-31 19:16:33Z behrisch $

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
PairData = namedtuple('PairData', ['otl', 'oconnection', 'tl', 'connection', 'offset', 'offset1',
                                   'prio', 'timeBetween', 'numVehicles'])

def merge (list1 , list2 , dt):
    for elem in list1:
        elem = elem._replace(offset1=elem.offset1 + dt)
    for elem in list2:
        list1.append(elem)
        list2.remove(elem)

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
    if l1Index == 0:
        TLSP = TLSP._replace(offset1=l1Pair.offset1)
    else:
        TLSP = TLSP._replace(offset1=l1Pair.offset1 + l1Pair.offset)
    l1.append(TLSP)


                       
def computeOffsets(TLSPList):
    c1, c2 , c3 , c4 , c5 = 0 , 0 , 0 , 0 , 0
    sets = []
    for TLSP in TLSPList:
        offset = TLSP.offset
        l1, l1Pair, l1Index = locate(TLSP.otl, sets)
        l2, l2Pair, l2Index = locate(TLSP.tl, sets)
        #print(l1)
        if l1 == None and l2 == None:
            # new set
            newlist = []
            newlist.append(TLSP)
            sets.append(newlist)
            c1 += 1
        elif l2 == None and not l1 == None:
            # add to set 1 - add after existing set
            coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
            c2 += 1
        elif l1 == None and not l2 == None:
            if l2Index == 0:
                TLSP = TLSP._replace(offset1=l1Pair.offset1 - offset)
            else:
                TLSP = TLSP._replace(offset1=l1Pair.offset1 + l1Pair.offset - offset)
            # add to set 2 - add before existing set
            TLSP = TLSP._replace(offset1 = l2[0].offset1 + l2[0].offset - offset)
            l2.append(TLSP)
            c3 += 1
        else:
            if l1 == l2:
                # cannot uncoordinated both tls. coordinate the first arbitrarily
                coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                c4 +=1
            else:
                # merge sets
                coordinateAfterSet(TLSP, l1, l1Pair, l1Index)
                merge(l1 , l2 , offset)
                c5 += 1
        #print(TLSP[4])
        for s in sets:
            print ["%s,%s" % (pd.otl.getID(), pd.tl.getID()) for pd in s]
    print("operations: newSet=%s addToSet=%s addToSet2=%s addHalfCoordinated=%s mergeSets=%s" % (c1, c2 , c3 , c4, c5))
    return(sets)


def setsToDic(sets):
    TLSPDic = {}
    for singleSet in sets:
        for pair in singleSet:
            tl1 = pair.otl.getID()
            tl2 = pair.tl.getID()       
            offset = pair.offset
            offset1 = pair.offset1
            try:
                if TLSPDic[tl1] != 0:
                    a = TLSPDic[tl1]
                else:
                    TLSPDic.update({tl1 : offset1})
            except KeyError:
                TLSPDic.update({tl1 : offset1})
            try:
                if TLSPDic[tl2] != 0:
                    a = TLSPDic[tl2]
                else:
                    TLSPDic.update({tl2 : (offset1 + offset)})
            except KeyError:
                TLSPDic.update({tl2 :(offset1 + offset)})
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


def routeToDic(net, routeFile):
    # pairs of traffic lights
    TLPairs = {} # PairKey -> PairData

    for route in sumolib.output.parse_fast(routeFile, 'route', ['edges']):
        rTLSList = getTLSInRoute(net, route.edges.split())

        for oldTL, TLelement in zip(rTLSList[:-1], rTLSList[1:]):
            key = PairKey(oldTL.edgeID, TLelement.edgeID, oldTL.dist)
            numVehicles = 0 if not key in TLPairs else TLPairs[key].numVehicles

            tl = net.getEdge(TLelement.edgeID).getTLS()
            tlp = tl.getPrograms()
            otl = net.getEdge(oldTL.edgeID).getTLS()
            otlp = otl.getPrograms()
            edge = net.getEdge(TLelement.edgeID)
            connection = TLelement.connection
            oconnection = oldTL.connection
            index =connection._tlLink
            for k, programm in tlp.iteritems():
                phases = programm.getPhases()
            start = 0
            for phase in phases:
                duration = phase[1]
                phase = phase[0]
                try:
                    if phase[index] == 'G' or phase[index] == 'g':
                        t2 = start
                    else:
                        start += duration
                except IndexError:
                    t = 0  
            index = oconnection._tlLink
            for k , programm in otlp.iteritems():
                phases = programm.getPhases()
            start = 0
            for phase in phases:
                duration = phase[1]
                phase = phase[0]
                try:
                    if phase[index] == 'G' or phase[index] == 'g':
                        t1 = start
                    else:
                        start += duration
                except IndexError:
                    t = 0
            offset = TLelement.time + t1 - t2
            offset1 = 0
            # relevant data for a pair of traffic lights
            TLPairs[key] = PairData(otl, oconnection,  tl, connection, offset, offset1,
                    edge.getPriority(), TLelement.time, numVehicles + 1)

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
        for ID, offset in TLSPDic.items():
            outf.write('    <tlLogic id="%s" programID="0" offset="%s"/>\n' %
                    (ID, str(offset)))
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
