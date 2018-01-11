#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
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
import subprocess
import sys
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -S -Q -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
polygonID = "0"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((1, 1), (1, 10), (10, 10)), (1, 2, 3, 4), True, "test")

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())
print("examining", polygonID)
print("shape", traci.polygon.getShape(polygonID))
print("type", traci.polygon.getType(polygonID))
print("color", traci.polygon.getColor(polygonID))
print("filled", traci.polygon.getFilled(polygonID))
traci.polygon.setShape(polygonID, ((11, 11), (11, 101), (101, 101)))
print("shape modified", traci.polygon.getShape(polygonID))
traci.polygon.setType(polygonID, "blub")
print("type modified", traci.polygon.getType(polygonID))
traci.polygon.setColor(polygonID, (5,6,7,8))
print("color modified", traci.polygon.getColor(polygonID))
traci.polygon.setColor(polygonID, (5,6,7))
print("color modified2", traci.polygon.getColor(polygonID))
traci.polygon.setFilled(polygonID, False)
print("filled modified", traci.polygon.getFilled(polygonID))

traci.polygon.subscribe(polygonID)
print(traci.polygon.getSubscriptionResults(polygonID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.polygon.getSubscriptionResults(polygonID))
traci.close()
sumoProcess.wait()
