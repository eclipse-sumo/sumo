#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

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
for step in range(4):
    print("step", step)
    traci.simulationStep()
print("areals", traci.areal.getIDList())
print("areal count", traci.areal.getIDCount())
detID = "det0"
print("examining", detID)
print("pos", traci.areal.getPosition(detID))
print("length", traci.areal.getLength(detID))
print("lane", traci.areal.getLaneID(detID))
print("vehNum", traci.areal.getLastStepVehicleNumber(detID))
print("meanSpeed", traci.areal.getLastStepMeanSpeed(detID))
print("vehIDs", traci.areal.getLastStepVehicleIDs(detID))
print("occupancy", traci.areal.getLastStepOccupancy(detID))

traci.areal.subscribe(detID)
print(traci.areal.getSubscriptionResults(detID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.areal.getSubscriptionResults(detID))
traci.close()
sumoProcess.wait()
