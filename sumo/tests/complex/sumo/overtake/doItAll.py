#!/usr/bin/env python
"""
@file    doItAll.py
@author  Michael Behrisch
@date    2012-02-15
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys, os
import math, random
import numpy as np
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib

netFile = "ttDistro.net.xml"
routeFile = "ttDistro.rou.xml"
configFile = "ttDistro.sumocfg"
dumpFile = "ttDump.xml"

def generateNet(nLanes = 2):
# writing the node file is not needed
# writing edge file (for different numbers of lanes!!),
    fp = open('ttDistro.edg.xml','w')
    rest = 'numLanes="' + repr(nLanes) + '" speed="40" />'
    print >> fp, '<edges>'
    print >> fp, '\t<edge from="node1" id="1to2" to="node2" ' + rest
    print >> fp, '\t<edge from="node2" id="2to3" to="node3" ' + rest
    print >> fp, '\t<edge from="node0" id="0to1" to="node1" ' + rest
    print >> fp, '</edges>'
    fp.close()

    fp = open('ttDistro.nod.xml','w')
    print >> fp, '<nodes>'
    print >> fp, '\t<node id="node0" x="-200.0" y="0.0" />'
    print >> fp, '\t<node id="node1" x="0.0" y="0.0" />'
    print >> fp, '\t<node id="node2" x="+9800.0" y="0.0" />'
    print >> fp, '\t<node id="node3" x="+10000.0" y="0.0" />'
    print >> fp, '</nodes>'
    fp.close()
    os.system('%s -n ttDistro.nod.xml -e ttDistro.edg.xml -o %s' % (sumolib.checkBinary("netconvert"),
                                                                 netFile))

def generateDemand(pin,nLanes):
    fp = open(routeFile, 'w')
    fp.write('<routes>\n')
    # normal vehicles
    lFast = 5.0
    lMed = 5.0
    lSlow = 11.0
    vSlow = 25.0
    vMed = 30.0
    vFast = 35.0
    pTruck = 0.1
    pMed = 0.5
    pFast = 0.4

    # fast passenger cars
    s0 = '\t<vType accel="2.0" decel="5.0" id="fast" length="' + repr(lFast) + '" maxSpeed="' + repr(vFast)
    print >> fp, s0 + '" sigma="0.8" />'

    # slow passenger cars
    s0 = '\t<vType accel="2.0" decel="5.0" id="med" length="' + repr(lMed) + '" maxSpeed="' + repr(vMed)
    print >> fp, s0 + '" sigma="0.8" />'

    # trucks
    s0 = '\t<vType accel="1.0" decel="4.0" id="truck" length="' + repr(lSlow) + '" maxSpeed="' + repr(vSlow)
    print >> fp, s0 + '" sigma="0.6" />'

    # the one and only route...
    fp.write('\t<route id="route01" edges="0to1 1to2 2to3"/>\n')

    # now, the vehicles...
    # generate nMax vehicles for each demand ranging from 0.03 to 0.66 veh/s/lane in steps of 0.03 vehs/s/lane
    # use only the last 1000 or so for the statistical analysis
    nMax = 1500
    vehID = 0

    for t in range(100000):
        for lane in range(0,nLanes):
            if random.random()<pin:
                vehID += 1
                s = '\t<vehicle depart="' + repr(t) + '" arrivalPos="-1" id="' + repr(vehID) +'" route="route01" departPos="pwagSimple"'
                if (lane==0):
                    if random.random()<pTruck/(pTruck + 0.5*pMed):
                        s = s + ' type="truck" departLane="'+repr(lane)+'" departSpeed="max" />'
                    else:
                        s = s + ' type="med" departLane="'+repr(lane)+'" departSpeed="max" />'
                else: # currently: the left lane only in this else cause
                    if random.random()<pFast/(pFast + 0.5*pMed):
                        s = s + ' type="fast" departLane="'+repr(lane)+'" departSpeed="max" />'
                    else:
                        s = s + ' type="med" departLane="'+repr(lane)+'" departSpeed="max" />'
                print >> fp, s
        if vehID >= nMax:
             break
    print >> fp, '</routes>'
    fp.close()

def runSim():
    fp = open(configFile, 'w')
    print >> fp, """<configuration>
    <input>
        <net-file value="%s"/>
        <route-files value="%s"/>
    </input>
    <report>
        <no-duration-log value="true"/>
        <no-step-log value="true"/>
        <max-depart-delay value="0"/>
        <tripinfo-output value="%s"/>
    </report>
</configuration>""" % (netFile, routeFile, dumpFile)
    fp.close()
    os.system('%s -c %s' % (sumolib.checkBinary("sumo"), configFile))

def analyzeData(pp):
    fp = open('gw.txt','w')
    meanTT = 0.0
    cntTT = 0
    n0 = 500    
    for line in open(dumpFile):
        ll = line.split('id="')
        if len(ll)>1:
            vNr = int(ll[1].split('"')[0])
            lll = ll[1].split('duration="')
            tT = float(lll[1].split('"')[0])
            tmp = line.split('depart="')[1]
            t1 = float(tmp.split('"')[0])
            if vNr>n0 and t1>0.0:
                print >>fp, pp, vNr, t1, tT
    fp.close()


def writeVSSFile():
# currently not needed.
# and the vss file,
     fp = open('input_vss.add.xml','w')
     print >> fp, '<additional>'
     s = '\t<variableSpeedSign id="vss" lanes="2to3_0'
     for lane in range(1,maxLanes):
          s = s + ' 2to3_'+repr(lane)
     print >> fp, s + '" file="laneFlows.vss.xml"/>'
     print >> fp, '</additional>'
     fp.close()

# here is "main"
nLanes = 2
print "# preparing the simulation..."
generateNet(nLanes)
for p in range(3,67,3):
    generateDemand(0.01*p,nLanes)
    print "# running the simulation..."
    runSim()
    print "# analyzing the results..."
    analyzeData(p)
    break
print "# All DONE."
