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
# @author  Matthias Schwamborn
# @date    2022-03-01

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             ] + sys.argv[1:])

vehID = "v0"


def accelerate(currentSpeed, targetSpeed, duration):
    accel = (targetSpeed - currentSpeed) / duration
    print("  accelerate from %s m/s to %s m/s over %s seconds" % (currentSpeed, targetSpeed, duration))
    traci.vehicle.setAcceleration(vehID, accel, duration)


traci.vehicle.setSpeed(vehID, 15)
while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    accel = traci.vehicle.getAcceleration(vehID)
    speed = traci.vehicle.getSpeed(vehID)
    print("%s acceleration=%s" % (t, accel))
    print("%s speed=%s" % (t, speed))
    if t == 4:
        accelerate(speed, speed + 2, 2)
    if t == 8:
        accelerate(speed, speed + 4, 3)
    if t == 15:
        accelerate(speed, -1, 5)
    traci.simulationStep()

traci.close()
