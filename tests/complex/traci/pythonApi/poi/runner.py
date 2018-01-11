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


def check(poiID):
    print("pois", traci.poi.getIDList())
    print("poi count", traci.poi.getIDCount())
    print("examining", poiID)
    print("pos", traci.poi.getPosition(poiID))
    print("type", traci.poi.getType(poiID))
    print("color", traci.poi.getColor(poiID))

sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -S -Q -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
poiID = "0"
print("adding", poiID)
traci.poi.add(poiID, 1, 1, (1, 2, 3, 4), "test")
check(poiID)
traci.poi.subscribe(poiID)
print(traci.poi.getSubscriptionResults(poiID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep(step)
    print(traci.poi.getSubscriptionResults(poiID))

traci.poi.setPosition(poiID, 5, 5)
traci.poi.setType(poiID, "blub")
traci.poi.setColor(poiID, (222, 111, 221, 0))
check(poiID)
traci.poi.add("p2", 2, 2, (11, 21, 31, 41), "ptest")
check("p2")
traci.poi.remove(poiID)
check("p2")
print("poi1 key='foo'", traci.poi.getParameter("poi1", "foo"))
print("poi2 pos", traci.poi.getPosition("poi2"))
print("poi2 key='lane'", traci.poi.getParameter("poi2", "lane"))
print("poi2 key='pos'", traci.poi.getParameter("poi2", "pos"))
print("poi2 key='posLat'", traci.poi.getParameter("poi2", "posLat"))
traci.close()
sumoProcess.wait()
