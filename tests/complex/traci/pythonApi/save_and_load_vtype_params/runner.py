#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2009-11-04
# @version $Id$

from __future__ import absolute_import

import os
import subprocess
import sys

sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))

import traci

ix = sys.argv.index(":")
saveParams = sys.argv[1:ix]
loadParams = sys.argv[ix + 1:]

## SAVE

PORT=traci.getFreeSocketPort()
sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', 'sumo'))
#~ sumoBinary = os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', 'bin', 'sumo-gui')
traci.start([sumoBinary] + saveParams)
tend = 55.
traci.simulationStep()
#traci.vehicletype.setImperfection("DEFAULT_VEHTYPE", 1.0)
#traci.vehicletype.setImperfection("t1", 1.0)
#traci.vehicletype.setDecel("t1", 2.0)
#traci.vehicletype.setAccel("t1", 3.0)
traci.vehicletype.setLength("t1", 4.0)
#traci.vehicletype.setApparentDecel("t1", 6.0)
#traci.vehicletype.setEmergencyDecel("t1", 7.0)
#traci.vehicletype.setEmissionClass("t1", ??)
traci.vehicletype.setMinGap("t1", 0.8)
traci.vehicletype.setSpeedFactor("t1", 0.9)
traci.vehicletype.setWidth("t1", 3.3)
traci.vehicletype.setActionStepLength("t1", 2.6)
traci.vehicletype.setActionStepLength("t1", 1.5)
#traci.vehicletype.setTau("t1", 1.3)
traci.vehicletype.setMaxSpeed("t1", 99.9)
while traci.simulation.getCurrentTime() < (tend-10.)*1000:
    traci.simulationStep()
    
#traci.vehicle.setImperfection("veh2", 1.0)
#traci.vehicle.setDecel("veh2", 2.0)
#traci.vehicle.setAccel("veh2", 3.0)
traci.vehicle.setLength("veh2", 4.0)
#traci.vehicle.setApparentDecel("veh2", 6.0)
#traci.vehicle.setEmergencyDecel("veh2", 7.0)
#traci.vehicle.setEmissionClass("veh2", ??)
traci.vehicle.setMinGap("veh2", 0.8)
traci.vehicle.setSpeedFactor("veh2", 0.9)
traci.vehicle.setWidth("veh2", 3.3)
#traci.vehicle.setTau("veh2", 1.3)
traci.vehicle.setMaxSpeed("veh2", 99.9)
traci.vehicle.setActionStepLength("veh2", 2.6)
traci.vehicle.setActionStepLength("veh2", 1.5)

print("Get before save....")

print("vtype")
#print(traci.vehicletype.getImperfection("DEFAULT_VEHTYPE") , "== 1.0")
#print(traci.vehicletype.getImperfection("t1") , "== 1.0")
#print(traci.vehicletype.getDecel("t1") ,"== 2.0")
#print(traci.vehicletype.getAccel("t1") ,"== 3.0")
print(traci.vehicletype.getLength("t1") ,"== 4.0")
#print(traci.vehicletype.getApparentDecel("t1") , "== 6.0")
#print(traci.vehicletype.getEmergencyDecel("t1") , "== 7.0")
#print(traci.vehicletype.getEmissionClass("t1") , " == ??")
print(traci.vehicletype.getMinGap("t1") ,"== 0.8")
print(traci.vehicletype.getSpeedFactor("t1") , "== 0.9")
print(traci.vehicletype.getWidth("t1") , "== 3.3")
#print(traci.vehicletype.getTau("t1") , "== 1.3")
print(traci.vehicletype.getMaxSpeed("t1") , "== 99.9")
print(traci.vehicletype.getActionStepLength("t1") , "==1.5")

print("vehicle")
#print(traci.vehicle.getImperfection("veh0") , "== 1.0")
#print(traci.vehicle.getImperfection("veh2") , "== 1.0")
#print(traci.vehicle.getDecel("veh2") ,"== 2.0")
#print(traci.vehicle.getAccel("veh2") ,"== 3.0")
print(traci.vehicle.getLength("veh2") ,"== 4.0")
#print(traci.vehicle.getApparentDecel("veh2") , "== 6.0")
#print(traci.vehicle.getEmergencyDecel("veh2") , "== 7.0")
#print(traci.vehicle.getEmissionClass("veh2") , " == ??")
print(traci.vehicle.getMinGap("veh2") ,"== 0.8")
print(traci.vehicle.getSpeedFactor("veh2") , "== 0.9")
print(traci.vehicle.getWidth("veh2") , "== 3.3")
#print(traci.vehicle.getTau("veh2") , "== 1.3")
print(traci.vehicle.getMaxSpeed("veh2") , "== 99.9")
print(traci.vehicle.getActionStepLength("veh2") , "==1.5")

while traci.simulation.getCurrentTime() < tend*1000:
    traci.simulationStep()
traci.close()


## LOAD

traci.start([sumoBinary] + loadParams)
tend = 300.

print("Get after load....")
print("vtype")
#print(traci.vehicletype.getImperfection("DEFAULT_VEHTYPE") , "== 1.0")
#print(traci.vehicletype.getImperfection("t1") , "== 1.0")
#print(traci.vehicletype.getDecel("t1") ,"== 2.0")
#print(traci.vehicletype.getAccel("t1") ,"== 3.0")
print(traci.vehicletype.getLength("t1") ,"== 4.0")
#print(traci.vehicletype.getApparentDecel("t1") , "== 6.0")
#print(traci.vehicletype.getEmergencyDecel("t1") , "== 7.0")
#print(traci.vehicletype.getEmissionClass("t1") , " == ??")
print(traci.vehicletype.getMinGap("t1") ,"== 0.8")
print(traci.vehicletype.getSpeedFactor("t1") , "== 0.9")
print(traci.vehicletype.getWidth("t1") , "== 3.3")
#print(traci.vehicletype.getTau("t1") , "== 1.3")
print(traci.vehicletype.getMaxSpeed("t1") , "== 99.9")
print(traci.vehicletype.getActionStepLength("t1") , "==1.5")

print("vehicle")
#print(traci.vehicle.getImperfection("veh0") , "== 1.0")
#print(traci.vehicle.getImperfection("veh2") , "== 1.0")
#print(traci.vehicle.getDecel("veh2") ,"== 2.0")
#print(traci.vehicle.getAccel("veh2") ,"== 3.0")
print(traci.vehicle.getLength("veh2") ,"== 4.0")
#print(traci.vehicle.getApparentDecel("veh2") , "== 6.0")
#print(traci.vehicle.getEmergencyDecel("veh2") , "== 7.0")
#print(traci.vehicle.getEmissionClass("veh2") , " == ??")
print(traci.vehicle.getMinGap("veh2") ,"== 0.8")
print(traci.vehicle.getSpeedFactor("veh2") , "== 0.9")
print(traci.vehicle.getWidth("veh2") , "== 3.3")
#print(traci.vehicle.getTau("veh2") , "== 1.3")
print(traci.vehicle.getMaxSpeed("veh2") , "== 99.9")
print(traci.vehicle.getActionStepLength("veh2") , "==1.5")

while traci.simulation.getCurrentTime() < tend*1000:
    traci.simulationStep()
traci.close()








