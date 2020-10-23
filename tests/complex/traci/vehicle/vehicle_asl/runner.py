#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
# ~ sumoBinary = sumolib.checkBinary("sumo-gui")
cmd = [
    sumoBinary,
    '-n', 'input_net.net.xml',
    '-r', 'input_routes.rou.xml',
    '--lanechange-output', 'lanechanges.xml',
    '--lanechange-output.started',
    '--lanechange-output.ended',
    '--fcd-output', 'fcd.xml',
    '--no-step-log',
    '--begin', '0',
    # ~ '--lateral-resolution', '3.2',
    # '-S', '-Q',
    '--step-length', '0.1',
    '--default.action-step-length', '1.0',
    "--default.speeddev", "0"]

traci.start(cmd)
traci.simulationStep()
vehID = "v0"
traci.vehicle.setLaneChangeMode(vehID, 0b0100000000)
traci.vehicle.changeLane(vehID, 1, 0)
for i in range(10):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)
traci.vehicle.setSpeed(vehID, 0)
for i in range(10):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)
traci.vehicle.setSpeed(vehID, -1)
for i in range(10):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)
# step to in between action steps
step = traci.simulation.getTime()
traci.simulationStep(step + .5)
traci.vehicle.changeLane(vehID, 0, 0)
for i in range(10):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)
traci.vehicle.setSpeed(vehID, 0)
for i in range(10):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)
traci.vehicle.setSpeed(vehID, -1)
for i in range(11):
    step = traci.simulation.getTime()
    traci.simulationStep(step + 1.)

traci.close()
