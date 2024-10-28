#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("vehicletypes", traci.vehicletype.getIDList())
print("vehicletype count", traci.vehicletype.getIDCount())
typeID = "DEFAULT_VEHTYPE"
print("examining", typeID)
print("length", traci.vehicletype.getLength(typeID))
print("maxSpeed", traci.vehicletype.getMaxSpeed(typeID))
print("speedFactor", traci.vehicletype.getSpeedFactor(typeID))
print("speedDev", traci.vehicletype.getSpeedDeviation(typeID))
print("accel", traci.vehicletype.getAccel(typeID))
print("decel", traci.vehicletype.getDecel(typeID))
print("emergencyDecel", traci.vehicletype.getEmergencyDecel(typeID))
print("apparentDecel", traci.vehicletype.getApparentDecel(typeID))
print("actionStepLength", traci.vehicletype.getActionStepLength(typeID))
print("imperfection", traci.vehicletype.getImperfection(typeID))
print("tau", traci.vehicletype.getTau(typeID))
print("vClass", traci.vehicletype.getVehicleClass(typeID))
print("emissionclass", traci.vehicletype.getEmissionClass(typeID))
print("shape", traci.vehicletype.getShapeClass(typeID))
print("MinGap", traci.vehicletype.getMinGap(typeID))
print("width", traci.vehicletype.getWidth(typeID))
print("color", traci.vehicletype.getColor(typeID))
print("maxSpeedLat", traci.vehicletype.getMaxSpeedLat(typeID))
print("latAlign", traci.vehicletype.getLateralAlignment(typeID))
print("minGapLat", traci.vehicletype.getMinGapLat(typeID))
print("personCapacity", traci.vehicletype.getPersonCapacity(typeID))
print("boardingDuration", traci.vehicletype.getBoardingDuration(typeID))
print("impatience", traci.vehicletype.getImpatience(typeID))
traci.vehicletype.subscribe(typeID)
print(traci.vehicletype.getSubscriptionResults(typeID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.vehicletype.getSubscriptionResults(typeID))
traci.vehicletype.setLength(typeID, 1.0)
print("length", traci.vehicletype.getLength(typeID))
traci.vehicletype.setScale(typeID, 1.4)
print("scale", traci.vehicletype.getScale(typeID))
traci.vehicletype.setMaxSpeed(typeID, 1.0)
print("maxSpeed", traci.vehicletype.getMaxSpeed(typeID))
traci.vehicletype.setSpeedFactor(typeID, 1.1)
print("speedFactor", traci.vehicletype.getSpeedFactor(typeID))
traci.vehicletype.setSpeedDeviation(typeID, 1.1)
print("speedDev", traci.vehicletype.getSpeedDeviation(typeID))
traci.vehicletype.setAccel(typeID, 1.1)
print("accel", traci.vehicletype.getAccel(typeID))
traci.vehicletype.setDecel(typeID, 1.1)
print("decel", traci.vehicletype.getDecel(typeID))
traci.vehicletype.setEmergencyDecel(typeID, 2.2)
print("emergencyDecel", traci.vehicletype.getEmergencyDecel(typeID))
traci.vehicletype.setApparentDecel(typeID, 3.3)
print("apparentDecel", traci.vehicletype.getApparentDecel(typeID))
traci.vehicletype.setActionStepLength(typeID, 2.2)
print("actionStepLength", traci.vehicletype.getActionStepLength(typeID))
traci.vehicletype.setImperfection(typeID, 0.1)
print("imperfection", traci.vehicletype.getImperfection(typeID))
traci.vehicletype.setTau(typeID, 1.1)
print("tau", traci.vehicletype.getTau(typeID))
traci.vehicletype.setVehicleClass(typeID, "bicycle")
print("vClass", traci.vehicletype.getVehicleClass(typeID))
traci.vehicletype.setEmissionClass(typeID, "zero")
print("emissionclass", traci.vehicletype.getEmissionClass(typeID))
traci.vehicletype.setShapeClass(typeID, "bicycle")
print("shape", traci.vehicletype.getShapeClass(typeID))
traci.vehicletype.setMinGap(typeID, 1.1)
print("MinGap", traci.vehicletype.getMinGap(typeID))
traci.vehicletype.setMinGapLat(typeID, 0.6)
print("MinGapLat", traci.vehicletype.getMinGapLat(typeID))
traci.vehicletype.setMaxSpeedLat(typeID, 1.0)
print("MaxSpeedLat", traci.vehicletype.getMaxSpeedLat(typeID))
traci.vehicletype.setLateralAlignment(typeID, "center")
print("LateralAlignment", traci.vehicletype.getLateralAlignment(typeID))
traci.vehicletype.setWidth(typeID, 1.1)
print("width", traci.vehicletype.getWidth(typeID))
traci.vehicletype.setHeight(typeID, 1.9)
print("height", traci.vehicletype.getHeight(typeID))
traci.vehicletype.setMass(typeID, 1600)
print("mass", traci.vehicletype.getMass(typeID))
traci.vehicletype.setBoardingDuration(typeID, 42)
print("boardingDuration", traci.vehicletype.getBoardingDuration(typeID))
traci.vehicletype.setImpatience(typeID, 0.5)
print("impatience", traci.vehicletype.getImpatience(typeID))
traci.vehicletype.setColor(typeID, (255, 0, 0, 255))
print("color", traci.vehicletype.getColor(typeID))
copyID = typeID + "_copy"
print("Copying vType '%s' to '%s'" % (typeID, copyID))
traci.vehicletype.copy(typeID, copyID)
print("vehicletypes", traci.vehicletype.getIDList())
print("vehicletype count", traci.vehicletype.getIDCount())
traci.vehicletype.setAccel(copyID, 100.)
print("accel (original)", traci.vehicletype.getAccel(typeID))
print("accel (copied)", traci.vehicletype.getAccel(copyID))


print("param", "'%s'" % traci.vehicletype.getParameter(typeID, "foo"))
traci.vehicletype.setParameter(typeID, "foo", "42")
print("param2", traci.vehicletype.getParameter(typeID, "foo"))

print("jmParam", "'%s'" % traci.vehicletype.getParameter(typeID, "junctionModel.jmTimegapMinor"))
traci.vehicletype.setParameter(typeID, "junctionModel.jmTimegapMinor", "2")
print("jmParam", traci.vehicletype.getParameter(typeID, "junctionModel.jmTimegapMinor"))

try:
    traci.vehicletype.setParameter(typeID, "junctionModel.dummy", "2")
except traci.TraCIException:
    pass
try:
    traci.vehicletype.setParameter(typeID, "junctionModel.speed", "2")
except traci.TraCIException:
    pass
try:
    traci.vehicletype.setParameter(typeID, "junctionModel.jmTimegapMinor", "notANumber")
except traci.TraCIException:
    pass


traci.simulationStep()
traci.close()
