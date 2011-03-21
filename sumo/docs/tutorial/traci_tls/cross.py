#!/usr/bin/env python
"""
@file    cross.py
@author  Lena Kalleske
@date    2009-03-26
@version $Id$

Tutorial for traffic light control via the TraCI interface.

Copyright (C) 2009-2011 DLR/TS, Germany
All rights reserved
"""

import os, subprocess, sys, socket, time, struct, random
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "tools"))
import traci

PORT = 8813

NSGREEN = "GrGr" 
NSYELLOW = "yryr"
WEGREEN = "rGrG" 
WEYELLOW = "ryry"

PROGRAM = [WEYELLOW,WEYELLOW,WEYELLOW,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSYELLOW,NSYELLOW,WEGREEN]

N = 9000


pWE = 1./10
pEW = 1./11
pNS = 1./30

routes = open("cross.rou.xml", "w")
print >> routes, """<routes>
    <vtype id="typeWE" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxspeed="16.67" guiShape="passenger"/>
    <vtype id="typeNS" accel="0.8" decel="4.5" sigma="0.5" length="20" maxspeed="25" guiShape="bus"/>

    <route id="right" edges="51o 1i 2o 52i" />
    <route id="left" edges="52o 2i 1o 51i" />
    <route id="down" edges="54o 4i 3o 53i" />
"""
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


sumoExe = "sumo-gui"
if "SUMO" in os.environ:
    sumoExe = os.path.join(os.environ["SUMO"], "sumo-gui")
sumoConfig = "cross.sumo.cfg"
sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)


traci.init(PORT)

programPointer = len(PROGRAM)-1
veh = []
step = 0
while not (step > lastVeh and veh == []):
    veh = traci.simulationStep(1000)
    programPointer = min(programPointer+1, len(PROGRAM)-1)
    no = traci.inductionloop.getLastStepVehicleNumber("0")
    if no > 0:
        programPointer = (0 if programPointer == len(PROGRAM)-1 else 3)
    traci.trafficlights.setRedYellowGreenState("0", PROGRAM[programPointer])
    step += 1

traci.close()
