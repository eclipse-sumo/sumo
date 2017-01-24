#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@date    2015-02-06
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


def step():
    s = traci.simulation.getCurrentTime() / 1000
    traci.simulationStep()
    return s

for i in range(3):
    print("step", step())


def check(personID):
    print("persons", traci.person.getIDList())
    print("person count", traci.person.getIDCount())
    print("examining", personID)
    print("speed", traci.person.getSpeed(personID))
    print("pos", traci.person.getPosition(personID))
    print("angle", traci.person.getAngle(personID))
    print("road", traci.person.getRoadID(personID))
    print("type", traci.person.getTypeID(personID))
    print("lanePos", traci.person.getLanePosition(personID))
    print("color", traci.person.getColor(personID))
    print("length", traci.person.getLength(personID))
    print("MinGap", traci.person.getMinGap(personID))
    print("width", traci.person.getWidth(personID))
    print("waiting time", traci.person.getWaitingTime(personID))
    print("stage", traci.person.getStage(personID))
    print("remainingStages", traci.person.getRemainingStages(personID))
    print("edges", traci.person.getEdges(personID))
    print("vehicle", traci.person.getVehicle(personID))

personID = "horiz"
check(personID)
traci.person.subscribe(personID)
print(traci.person.getSubscriptionResults(personID))
for i in range(3):
    print("step", step())
    print(traci.person.getSubscriptionResults(personID))
check(personID)
try:
    check("bla")
except traci.TraCIException:
    print("recovering from exception after asking for unknown person")
print("step", step())
print("getIDList", traci.person.getIDList())
for i in range(6):
    print("step", step())
    print(traci.person.getSubscriptionResults(personID))

for i in range(200):
    step()
# person should be on a walkingArea in front of a crossing now
print(traci.person.getRoadID(personID))
print(traci.person.getLanePosition(personID))
print(traci.person.getNextEdge(personID))
# retrieve invalid stages
try:
    print(traci.person.getStage(personID, 3))
except traci.TraCIException:
    print("recovering from exception after asking for invalid stage index")
try:
    print(traci.person.getStage(personID, -2))
except traci.TraCIException:
    print("recovering from exception after asking for invalid stage index")
traci.close()
sumoProcess.wait()
