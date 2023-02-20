#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])

loopID = "0"


def check():
    print("%s timeSinceDet=%s occupancy=%s vehNum=%s vehIDs=%s meanLength=%s meanSpeed=%s vehData=%s" % (
        traci.simulation.getTime(),
        traci.inductionloop.getTimeSinceDetection(loopID),
        traci.inductionloop.getLastStepOccupancy(loopID),
        traci.inductionloop.getLastStepVehicleNumber(loopID),
        traci.inductionloop.getLastStepVehicleIDs(loopID),
        traci.inductionloop.getLastStepMeanLength(loopID),
        traci.inductionloop.getLastStepMeanSpeed(loopID),
        traci.inductionloop.getVehicleData(loopID)))


def ovr(time):
    print("override %s" % time)
    traci.inductionloop.overrideTimeSinceDetection(loopID, time)


for step in range(3):
    check()
    traci.simulationStep()
ovr(0)

for step in range(3):
    check()
    traci.simulationStep()
ovr(0.5)

for step in range(3):
    check()
    traci.simulationStep()
ovr(1)

for step in range(3):
    check()
    traci.simulationStep()
ovr(2)

for step in range(3):
    check()
    traci.simulationStep()
ovr(-1)

for step in range(3):
    check()
    traci.simulationStep()

traci.close()
