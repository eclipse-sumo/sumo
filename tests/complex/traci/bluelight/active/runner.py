#!/usr/bin/env python
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
# @author  Mark Colley
# @date    2026-11-18

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

extra_args = [a for a in sys.argv[1:] if not a.startswith("--meso")]
traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net2.net.xml",
    "-r", "input_routes.rou.xml",
    "--lateral-resolution", "0.8",
    "--no-step-log",
] + extra_args)

state_len = len(traci.trafficlight.getRedYellowGreenState("C"))
traci.trafficlight.setRedYellowGreenState("C", "r" * state_len)

traci.simulationStep()
print("initial active:", traci.vehicle.getParameter("ego", "device.bluelight.active"))
print("initial speedMode:", traci.vehicle.getSpeedMode("ego"))

for _ in range(10):
    traci.simulationStep()

print("stopped at red: road=%s speed=%.2f blocker_rescue=%r" % (
    traci.vehicle.getRoadID("ego"),
    traci.vehicle.getSpeed("ego"),
    traci.vehicle.getParameter("blocker", "rescueLane"),
))

traci.vehicle.setParameter("ego", "device.bluelight.active", "true")
print("activated:", traci.vehicle.getParameter("ego", "device.bluelight.active"),
      "speedMode:", traci.vehicle.getSpeedMode("ego"))

traci.simulationStep()
print("step 1 after activation: road=%s blocker_rescue=%r" % (
    traci.vehicle.getRoadID("ego"),
    traci.vehicle.getParameter("blocker", "rescueLane"),
))

traci.vehicle.setParameter("ego", "device.bluelight.active", "false")
print("deactivated on SC: active=%s speedMode=%s blocker_rescue=%r" % (
    traci.vehicle.getParameter("ego", "device.bluelight.active"),
    traci.vehicle.getSpeedMode("ego"),
    traci.vehicle.getParameter("blocker", "rescueLane"),
))

traci.vehicle.setParameter("ego", "device.bluelight.active", "true")
for _ in range(8):
    traci.simulationStep()

print("drove past red after re-activation: road=%s" % traci.vehicle.getRoadID("ego"))
traci.close()
