#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
if len(sys.argv) > 1:
    import libsumo as traci  # noqa
else:
    import traci  # noqa
import sumolib  # noqa

def examine(polygonID):
    print("examining", polygonID)
    print("shape", traci.polygon.getShape(polygonID))
    print("type", traci.polygon.getType(polygonID))
    print("color", traci.polygon.getColor(polygonID))
    print("filled", traci.polygon.getFilled(polygonID))

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
for step in range(3):
    print("step", step)
    traci.simulationStep()
polygonID = "0"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((0, 50), (0, 50), (50, 50)), (100, 200, 0, 255), True, "test")

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

examine(polygonID)

for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    
# Add empty polygon dynamics
print ("Adding underspecified dynamics...")
try:
    traci.polygon.addDynamics(polygonID)
except traci.exceptions.TraCIException as e:
    print("Caught TraCIException")
    pass
    
print ("Adding malformed dynamics 1 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [0,1,2,4,3])
except traci.exceptions.TraCIException as e:
    print("Caught TraCIException")
    pass
    
print ("Adding malformed dynamics 2 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [1,2,3,4], [200,20,2,1])
except traci.exceptions.TraCIException as e:
    print("Caught TraCIException")
    pass
    
print ("Adding malformed dynamics 3 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [0,1,2,3], [200,20,2])
except traci.exceptions.TraCIException as e:
    print("Caught TraCIException")
    pass


for step in range(6, 9):
    print("step", step)
    traci.simulationStep()
    
traci.close()
