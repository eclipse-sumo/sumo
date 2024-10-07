#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @date    2012-02-15

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import random
import subprocess
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

netFile = "ttDistro.net.xml"
routeFile = "ttDistro.rou.xml"
configFile = "ttDistro.sumocfg"
dumpFile = "ttDump.xml"


def generateNet(numLanes=2):
    with open('ttDistro.edg.xml', 'w', encoding="utf8") as fp:
        rest = 'numLanes="%s" speed="40" />' % numLanes
        print('<edges>', file=fp)
        print('    <edge from="node1" id="1to2" to="node2" ' + rest, file=fp)
        print('    <edge from="node2" id="2to3" to="node3" ' + rest, file=fp)
        print('    <edge from="node0" id="0to1" to="node1" ' + rest, file=fp)
        print('</edges>', file=fp)

    with open('ttDistro.nod.xml', 'w', encoding="utf8") as fp:
        print('<nodes>', file=fp)
        print('    <node id="node0" x="-200.0" y="0.0" />', file=fp)
        print('    <node id="node1" x="0.0" y="0.0" />', file=fp)
        print('    <node id="node2" x="+9800.0" y="0.0" />', file=fp)
        print('    <node id="node3" x="+10000.0" y="0.0" />', file=fp)
        print('</nodes>', file=fp)
    subprocess.call([sumolib.checkBinary("netconvert"),
                     '-n', 'ttDistro.nod.xml', '-e', 'ttDistro.edg.xml', '-o', netFile])


def generateDemand(pin, nLanes):
    with open(routeFile, 'w', encoding="utf8") as fp:
        sumolib.writeXMLHeader(fp, root="routes")
        print('''
    <vType accel="2." decel="5." id="fast" length="5." maxSpeed="35." sigma="0.8" />
    <vType accel="2." decel="5." id="med" length="5." maxSpeed="30." sigma="0.8" />
    <vType accel="1." decel="4." id="truck" length="11." maxSpeed="25." sigma="0.6" />
    <route id="route01" edges="0to1 1to2 2to3"/>''', file=fp)

        # now, the vehicles...
        # generate nMax vehicles for each demand ranging from 0.03 to 0.66 veh/s/lane in steps of 0.03 vehs/s/lane
        # use only the last 1000 or so for the statistical analysis
        pTruck = 0.1
        pMed = 0.5
        pFast = 0.4
        nMax = 1500
        vehID = 0

        for t in range(100000):
            for lane in range(0, nLanes):
                if random.random() < pin:
                    vehID += 1
                    s = '    <vehicle depart="' + \
                        repr(t) + '" arrivalPos="-1" id="' + repr(vehID) + \
                        '" route="route01"'
                    if lane == 0:
                        if random.random() < pTruck / (pTruck + 0.5 * pMed):
                            s = s + ' type="truck" departLane="' + \
                                repr(lane) + '" departSpeed="max" />'
                        else:
                            s = s + ' type="med" departLane="' + \
                                repr(lane) + '" departSpeed="max" />'
                    else:  # currently: the left lane only in this else cause
                        if random.random() < pFast / (pFast + 0.5 * pMed):
                            s = s + ' type="fast" departLane="' + \
                                repr(lane) + '" departSpeed="max" />'
                        else:
                            s = s + ' type="med" departLane="' + \
                                repr(lane) + '" departSpeed="max" />'
                    print(s, file=fp)
            if vehID >= nMax:
                break
        print('</routes>', file=fp)


def analyzeData(pp):
    n0 = 500
    with open('gw.txt', 'w', encoding="utf8") as fp, open(dumpFile) as dump:
        for line in dump:
            ll = line.split('id="')
            if len(ll) > 1:
                vNr = int(ll[1].split('"')[0])
                lll = ll[1].split('duration="')
                tT = float(lll[1].split('"')[0])
                tmp = line.split('depart="')[1]
                t1 = float(tmp.split('"')[0])
                if vNr > n0 and t1 > 0.0:
                    print(pp, vNr, t1, tT, file=fp)


def writeVSSFile():
    # currently not needed.
    # and the vss file,
    fp = open('input_vss.add.xml', 'w')
    print('<additional>', file=fp)
    s = '    <variableSpeedSign id="vss" lanes="2to3_0'
    maxLanes = None
    for lane in range(1, maxLanes):
        s = s + ' 2to3_' + repr(lane)
    print(s + '" file="laneFlows.vss.xml"/>', file=fp)
    print('</additional>', file=fp)
    fp.close()


# here is "main"
nLanes = 2
print("# preparing the simulation...")
generateNet(nLanes)
for p in range(3, 67, 3):
    generateDemand(0.01 * p, nLanes)
    print("# running the simulation...")
    subprocess.call([sumolib.checkBinary("sumo"), "-n", netFile, "-r", routeFile, "--tripinfo-output", dumpFile,
                     "--no-duration-log", "--no-step-log", "--max-depart-delay", "0"])
    print("# analyzing the results...")
    analyzeData(p)
    break
print("# All DONE.")
