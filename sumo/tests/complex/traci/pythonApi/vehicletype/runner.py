#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
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
print("imperfection", traci.vehicletype.getImperfection(typeID))
print("tau", traci.vehicletype.getTau(typeID))
print("vClass", traci.vehicletype.getVehicleClass(typeID))
print("emissionclass", traci.vehicletype.getEmissionClass(typeID))
print("shape", traci.vehicletype.getShapeClass(typeID))
print("MinGap", traci.vehicletype.getMinGap(typeID))
print("width", traci.vehicletype.getWidth(typeID))
print("color", traci.vehicletype.getColor(typeID))
traci.vehicletype.subscribe(typeID)
print(traci.vehicletype.getSubscriptionResults(typeID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.vehicletype.getSubscriptionResults(typeID))
traci.vehicletype.setLength(typeID, 1.0)
print("length", traci.vehicletype.getLength(typeID))
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
traci.vehicletype.setWidth(typeID, 1.1)
print("width", traci.vehicletype.getWidth(typeID))
traci.vehicletype.setHeight(typeID, 1.9)
print("height", traci.vehicletype.getHeight(typeID))
traci.vehicletype.setColor(typeID, (1, 0, 0, 1))
print("color", traci.vehicletype.getColor(typeID))
traci.close()
sumoProcess.wait()
