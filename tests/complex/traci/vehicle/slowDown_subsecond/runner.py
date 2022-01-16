#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer (Technische Universitaet Braunschweig)
# @date    2018-09-27

from __future__ import print_function
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--step-length", "0.5",
             "--no-step-log",
             ] + sys.argv[1:])

vehID = "v0"


def slowDown(currentSpeed, delta, duration):
    newSpeed = currentSpeed + delta
    print("  slowDown from %s to %s over %s seconds" % (currentSpeed,
                                                        newSpeed, duration))
    traci.vehicle.slowDown(vehID, newSpeed, duration)


traci.vehicle.setAccel(vehID, 0)
# traci.vehicle.setSpeed(vehID, 25)
while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    speed = traci.vehicle.getSpeed(vehID)
    print("%s speed=%s" % (t, speed))
    if t == 4:
        slowDown(speed, -4, 2)
    if t == 8:
        slowDown(speed, -6, 5)
    if t == 15:
        slowDown(speed, -2, 1)
    traci.simulationStep()

traci.close()
