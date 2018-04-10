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
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-c', 'sumo.sumocfg',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)


def check(vehID, steps=1):
    for i in range(steps):
        if i > 0:
            traci.simulationStep()
        try:
            print("%s vehicle %s on lane=%s pos=%s speed=%s" % (
                traci.simulation.getCurrentTime() / 1000.0,
                vehID,
                traci.vehicle.getLaneID(vehID),
                traci.vehicle.getLanePosition(vehID),
                traci.vehicle.getSpeed(vehID)))
        except traci.TraCIException:
            pass
    if steps > 1:
        print()


vehID = "v0"
traci.init(PORT)
traci.simulationStep()
check(vehID)
try:
    print("%s setStop for %s" %
          (traci.simulation.getCurrentTime() / 1000.0, vehID))
    traci.vehicle.setStop(vehID, "beg", pos=1.0, laneIndex=0, duration=5000)
except traci.TraCIException:
    pass
check(vehID, 10)

traci.simulationStep(21 * 1000)
vehID = "v1"
check(vehID)
try:
    print("%s setStop for %s" %
          (traci.simulation.getCurrentTime() / 1000.0, vehID))
    traci.vehicle.setStop(vehID, "end", pos=1.0, laneIndex=0, duration=5000)
except traci.TraCIException:
    pass
check(vehID, 10)

traci.simulationStep(41 * 1000)
vehID = "v2"
check(vehID)
try:
    print("%s setStop for %s" %
          (traci.simulation.getCurrentTime() / 1000.0, vehID))
    traci.vehicle.setStop(vehID, "middle", pos=1.0, laneIndex=0, duration=5000)
except traci.TraCIException:
    pass
check(vehID, 10)

traci.simulationStep(61 * 1000)
vehID = "v3"
check(vehID)
try:
    print("%s setStop for %s" %
          (traci.simulation.getCurrentTime() / 1000.0, vehID))
    traci.vehicle.setStop(vehID, "middle", pos=1.0, laneIndex=0, duration=5000)
except traci.TraCIException:
    pass
check(vehID, 10)

traci.close()
sumoProcess.wait()
