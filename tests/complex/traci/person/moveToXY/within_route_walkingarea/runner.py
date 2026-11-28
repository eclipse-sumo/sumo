#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2015-02-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
cmd = [
    sumoBinary,
    "-n", "input_net.net.xml",
    "-r", "input_routes.rou.xml",
    "--fcd-output", "fcd.xml",
    "--no-step-log"]
traci.start(cmd + sys.argv[1:])


p = "p0"
speed = 1.4
traci.simulationStep()
for i in range(20):
    angle = traci.person.getAngle(p)
    x, y = traci.person.getPosition(p)
    y += speed
    traci.person.moveToXY(p, "", x, y, angle, keepRoute=4)
    print(traci.simulation.getTime(), p, x, y, traci.person.getPosition(p))
    traci.simulationStep()

# A manually controlled person without a walking route can enter the walking area.
traci.load(["-n", "input_net.net.xml", "--no-step-log"] + sys.argv[1:])
p = "waiting"
traci.person.add(p, "-gneE1", 0)
traci.person.appendWaitingStage(p, 100)
traci.simulationStep()
for x, y, edge in [(1.6, -12, "-gneE1"), (1.6, -9, "-gneE1"), (1.6, -6, "-gneE1"),
                   (2.75, -4.5, ":gneJ0_w1"), (2.55, -4.25, ":gneJ0_w1")]:
    traci.person.moveToXY(p, "", x, y, angle=0, keepRoute=4, matchThreshold=500)
    traci.person.setSpeed(p, 0)
    traci.simulationStep()
    assert traci.person.getRoadID(p) == edge, traci.person.getRoadID(p)
    actual_x, actual_y = traci.person.getPosition(p)
    assert abs(actual_x - x) < 1e-6 and abs(actual_y - y) < 1e-6, (actual_x, actual_y)
print("Manually moved a waiting person onto the walking area.")

traci.close()
