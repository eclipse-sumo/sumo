#!/usr/bin/env python
"""
@file    runner.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-03-26
@version $Id$

Tutorial for traffic light control via the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2009-2012 DLR/TS, Germany
All rights reserved
"""

import os, subprocess, sys, random
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..', '..')), 'tools'))
from sumolib import checkBinary
import traci

PORT = 8813

NSGREEN = "GrGr" 
NSYELLOW = "yryr"
WEGREEN = "rGrG" 
WEYELLOW = "ryry"

PROGRAM = [WEYELLOW,WEYELLOW,WEYELLOW,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSYELLOW,NSYELLOW,WEGREEN]

if not traci.isEmbedded():
    N = 9000
    pWE = 1./10
    pEW = 1./11
    pNS = 1./30
    routes = open("data/cross.rou.xml", "w")
    print >> routes, """<routes>
    <vType id="typeWE" accel="0.8" decel="4.5" sigma="0.5" length="5" minGap="2.5" maxSpeed="16.67" guiShape="passenger"/>
    <vType id="typeNS" accel="0.8" decel="4.5" sigma="0.5" length="17" minGap="3" maxSpeed="25" guiShape="bus"/>

    <route id="right" edges="51o 1i 2o 52i" />
    <route id="left" edges="52o 2i 1o 51i" />
    <route id="down" edges="54o 4i 3o 53i" />"""
    lastVeh = 0
    vehNr = 0
    for i in range(N):
        if random.uniform(0,1) < pWE:
            print >> routes, '    <vehicle id="%i" type="typeWE" route="right" depart="%i" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i
        if random.uniform(0,1) < pEW:
            print >> routes, '    <vehicle id="%i" type="typeWE" route="left" depart="%i" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i
        if random.uniform(0,1) < pNS:
            print >> routes, '    <vehicle id="%i" type="typeNS" route="down" depart="%i" color="1,0,0"/>' % (vehNr, i)
            vehNr += 1
            lastVeh = i
    print >> routes, "</routes>"
    routes.close()

    sumoBinary = checkBinary('sumo')
    sumoConfig = "data/cross.sumocfg"
    if len(sys.argv) > 1:
        retCode = subprocess.call("%s -c %s --python-script %s" % (sumoBinary, sumoConfig, __file__), shell=True, stdout=sys.stdout)
        sys.exit(retCode)
    else:
        sumoProcess = subprocess.Popen("%s -c %s" % (sumoBinary, sumoConfig), shell=True, stdout=sys.stdout)
        traci.init(PORT)

programPointer = len(PROGRAM)-1
step = 0
while step == 0 or traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    programPointer = min(programPointer+1, len(PROGRAM)-1)
    no = traci.inductionloop.getLastStepVehicleNumber("0")
    if no > 0:
        programPointer = (0 if programPointer == len(PROGRAM)-1 else 3)
    traci.trafficlights.setRedYellowGreenState("0", PROGRAM[programPointer])
    step += 1

traci.close()
sys.stdout.flush()
