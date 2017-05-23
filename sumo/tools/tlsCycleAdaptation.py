#!/usr/bin/env python
"""
@file    tlsCycleAndGreenTimeOptimizator.py
@author  Yun-Pang Floetteroed
@author  Michael Behrisch
@date    2017-05-10
@version $Id: tlsCycleAndGreenTimeOptimizator.py

- The Webster's equation is used to optimize the cycle length
  and the green times of the traffic lights in a sumo network
  with a given route file.

- The traffic lights without traffic flows will not be optimized.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
import xml.etree.cElementTree as ET
import optparse
import collections

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
from sumolib.net import connection

def get_options(args=None):
    optParser = optparse.OptionParser()
    optParser.add_option("-n", "--net-file", dest="netfile",
                         help="define the net file (mandatory)")
    optParser.add_option("-o", "--output-file", dest="outfile",
                         default="tlsOptimization.add.xml", help="define the output filename")
    optParser.add_option("-r", "--route-file", dest="routefile",
                         help="define the route file (mandatory)")
    optParser.add_option("-b", "--begin", dest="begin", type="int",
                         help="begin time of the optmization period with unit second")
    optParser.add_option("-e", "--end", dest="end", type="int",
                         help="end time of the optmization period with unit second")
    optParser.add_option("-y", "--yellow-time", dest="yellowtime", type="int",
                         default=4, help="yellow time")
    optParser.add_option("-a", "--all-red", dest="allred", type="int",
                         default=0, help="all-red time")
    optParser.add_option("-l", "--lost-time", dest="losttime", type="int",
                         default=4, help="lost time for each phase")
    optParser.add_option("-g", "--min-green", dest="mingreen", type="int",
                         default=4, help=" minimal green time when there is no traffic volume")
    optParser.add_option("-c", "--min-cycle", dest="mincycle", type="int",
                         default=16, help=" minimal cycle length")
    optParser.add_option("-C", "--max-cycle", dest="maxcycle", type="int",
                         default=120, help=" maximal cycle length")
    optParser.add_option("-s", "--saturation-flows", dest="satflows", type="float",
                         default=1800, help=" saturation flows lane/hour")
    optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    (options, args) = optParser.parse_args(args=args)
    if not options.netfile or not options.routefile or not options.begin or not options.end:
        optParser.print_help()
        sys.exit()

    return options
    
def getFlows(net, routeFile, tlsList, begin, end):
    tlsFlowsMap = {}
    for tls in tlsList:
        tlsFlowsMap[tls._id] = collections.defaultdict(lambda:collections.defaultdict(int))

    for veh in sumolib.output.parse(routeFile, 'vehicle'):
        route = veh.route[0]
        if int(veh.depart) <= end and int(veh.depart) >= begin:
            for tls in tlsList:
                connsList = tls.getConnections()
                for c in sorted(connsList, key=lambda connsList: connsList[2]): # c: [[inLane, outLane, linkNo],[],...]
                    subRoute = c[0]._edge._id + ' ' + c[1]._edge._id
                    edgeList = route.edges.split()
                    if c[0]._edge._id in edgeList:
                        beginindex = edgeList.index(c[0]._edge._id)
                        if beginindex < 0:
                            print ("negtive beginindex: %s" %beginindex)
                        elif beginindex < len(edgeList)-1 and edgeList[beginindex+1] == c[1]._edge._id:
                            pce = 1
                            if veh.type == "bicycle":
                                pce = 0.2
                            elif veh.type in ["moped", "motorcycle"]:
                                pce = 0.5
                            elif veh.type in ["truck", "trailer", "bus", "coach"]:
                                pce = 3.5
                            tlsFlowsMap[tls._id][subRoute][c[2]] += pce
                    
    # remove the doubled counts
    connFlowsMap = {}
    for t in tlsList:
        connFlowsMap[t._id] = {}
        for subRoute in tlsFlowsMap[t._id]:
            totalConns = len(tlsFlowsMap[t._id][subRoute])
            for conn in tlsFlowsMap[t._id][subRoute]:
                tlsFlowsMap[t._id][subRoute][conn] /= totalConns
                connFlowsMap[t._id][conn] = tlsFlowsMap[t._id][subRoute][conn]

    return connFlowsMap
    
def getEffectiveTlsList(tlsList, connFlowsMap, verbose):
    effectiveTlsList = []
    for tl in tlsList:
        for conn in connFlowsMap[tl._id]:
            if connFlowsMap[tl._id][conn] > 0:
                effectiveTlsList.append(tl)
                break
    return effectiveTlsList

def getLaneGroupFlows(tl, connFlowsMap, phases):
    connsList = tl.getConnections()
    #programs = tl.getPrograms()
    groupFlowsMap = {}     #i(phase): duration, laneGroup1, laneGroup2, ....
    connsList = sorted(connsList, key=lambda connsList: connsList[2])

    #for pro in programs:
    #    phases = programs[pro].getPhases()
        
        # check if there are shared lane groups, i.e. some lane groups have only "g" (no "G")
    ownGreenConnsList = []
    for i, p in enumerate(phases):
        totalConns = len(p[0])
        for j, control in enumerate(p[0]):
            if control == "G":
                ownGreenConnsList.append(j)

    yellowRedTime = 0
    greenTime = 0
    phaseLaneIndexMap = collections.defaultdict(list)
    for i, p in enumerate(phases):
        if 'G' in p[0]:
            print ("check_phase:%s" %i)
            greenTime += p[1]   # todo: change to float? wait for micha
            groupFlowsMap[i] = [p[1]]
            groupFlows = 0
            laneIndexList = []
            for j, control in enumerate(p[0]):
                inEdge = connsList[j][0]._edge._id
                if j == 0:
                    exEdge = inEdge

                if (inEdge == exEdge and control =='G') or (inEdge == exEdge and control == 'g' and j not in ownGreenConnsList):
                    groupFlows += connFlowsMap[tl._id][j]
                    if connsList[j][0].getIndex() not in laneIndexList:
                        laneIndexList.append(connsList[j][0].getIndex())
                if exEdge != inEdge or j == len(p[0])-1:
                    if groupFlows > 0:
                        groupFlowsMap[i].append(groupFlows)
                    groupFlows = 0
                    if laneIndexList:
                        phaseLaneIndexMap[i].append(laneIndexList)
                    laneIndexList = []
                exEdge = inEdge
        elif 'G' not in p[0] and 'g' in p[0]:
            print ("Check: only g for all connections:%s " %tl._id)
        elif 'G' not in p[0] and 'g' not in p[0]:
            yellowRedTime += int(p[1])
            if options.verbose and i in groupFlowsMap:
                print ("phase: %s" %i)
                print ("group flows: %s" %groupFlowsMap[i])
                print ("The used lanes: %s" % phaseLaneIndexMap[i])
                
    return groupFlowsMap, phaseLaneIndexMap

def optimizeGreenTime(groupFlowsMap, phaseLaneIndexMap, options):
    # calculate the total lost time
    # lostTime = # of phases * the lost time per phase + the all-red time
    lostTime = len(groupFlowsMap)*options.losttime + options.allred

    # calculate the critial flow ratios and the respective sum
    critialFlowRateMap = {}
   
    for i in groupFlowsMap:   # [duration. groupFlow1, groupFlow2...]
        critialFlowRateMap[i] = 0.
        maxFlow = 0
        index = None
        for j, f in enumerate(groupFlowsMap[i][1:]):
            if f > maxFlow:
                maxFlow = f
                index = j
        # (maxFlow/# of lanes)/saturation flows per lane
        critialFlowRateMap[i] = (maxFlow/float((len(phaseLaneIndexMap[i][index]))))/options.satflows
    
    # calculate the optimal cycle length
    optCycle = int(round((1.5*lostTime + 5.)/(1. - sum(critialFlowRateMap.values()))))

    if optCycle < options.mincycle:
        optCycle = options.mincycle
    elif optCycle > options.maxcycle:
        optCycle = options.maxcycle

    #calcualte the effective and optimized green times
    effGreenTime = optCycle - lostTime
    totalLength = lostTime
    for i in critialFlowRateMap:
        groupFlowsMap[i][0] = effGreenTime * (critialFlowRateMap[i]/sum(critialFlowRateMap.values())) - options.yellowtime + options.losttime
        groupFlowsMap[i][0] = int(round(groupFlowsMap[i][0]))
        if groupFlowsMap[i][0] < options.mingreen:
            groupFlowsMap[i][0] = options.mingreen
        if options.verbose:
            print ("Green time for phase %s: %s" %(i, groupFlowsMap[i][0]))
        totalLength += groupFlowsMap[i][0]

    if options.verbose:
        print ("the optimal cycle lenth:%s" %totalLength)
        
    return groupFlowsMap
    
def main(options):
    net = sumolib.net.readNet(options.netfile, withPrograms=True)
    tlsList = net.getTrafficLights()
    nodesList = net.getNodes()
    print("the total number of tls: %s" % len(tlsList))
      
    # get traffic flows for each connection at each TL
    connFlowsMap = getFlows(net, options.routefile, tlsList, options.begin, options.end)
    
    # remove the tls where no traffic volumes exist
    effectiveTlsList = getEffectiveTlsList(tlsList, connFlowsMap, options.verbose)
    
    with open(options.outfile, 'w') as outf:
        outf.write('<additional>\n')

        if len(effectiveTlsList) > 0:
            for tl in effectiveTlsList:
                if options.verbose:
                    print ("Intersection name: %s" %tl._id)
                programs = tl.getPrograms()
                for pro in programs:
                    phases = programs[pro].getPhases()

                    # get the connection flows and group flows
                    groupFlowsMap, phaseLaneIndexMap = getLaneGroupFlows(tl, connFlowsMap, phases)
                    
                    # optimize the cycle length and green times
                    groupFlowsMap = optimizeGreenTime(groupFlowsMap, phaseLaneIndexMap, options)
                
                # write output
                programs = tl.getPrograms()   # is it possible to have 2 or more programs in a network file?
                for pro in programs:
                    outf.write('    <tlLogic id="%s" type="%s" programID="%s" offset="%.2f"/>\n' %
                                    (tl._id, programs[pro]._type, programs[pro]._id, programs[pro]._offset))
                                    
                    phases = programs[pro].getPhases()
                    for i, p in enumerate(phases):
                        duration = p[1]
                        if i in groupFlowsMap:
                            duration = groupFlowsMap[i][0]
                        outf.write('        <phase duration="%s" state="%s"/>\n' %
                                       (duration, p[0]))
        else:
            print("There are no flows at the given intersections. No green time optimization is done.")
        outf.write('</additional>\n')

if __name__ == "__main__":
    options = get_options(sys.argv)
    main(options)
