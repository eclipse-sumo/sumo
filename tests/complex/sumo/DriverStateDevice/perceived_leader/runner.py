#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
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

from __future__ import absolute_import
from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

extra_args = [a for a in sys.argv[1:] if not a.startswith("--meso")]
traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net.net.xml",
    "-r", "input_routes.rou.xml",
    "--no-step-log",
    "--default.speeddev", "0",
] + extra_args)

traci.simulationStep()
traci.vehicle.setLaneChangeMode("ego", 0)
traci.simulationStep()
traci.simulationStep()

print("unknown leader headway:", repr(traci.vehicle.getParameter("ego", "device.driverstate.perceivedHeadway.other")))
print("model headway:", repr(traci.vehicle.getParameter("ego", "device.driverstate.perceivedHeadway.leader")))
print("model speedDiff:", repr(traci.vehicle.getParameter("ego", "device.driverstate.perceivedSpeedDifference.leader")))
speed_before = traci.vehicle.getSpeed("ego")
print("ego speed before override: %.2f" % speed_before)

traci.vehicle.setParameter("ego", "device.driverstate.perceivedHeadway.leader", "2.0")
traci.vehicle.setParameter("ego", "device.driverstate.perceivedSpeedDifference.leader", "-10.0")
traci.simulationStep()

print("override headway:", traci.vehicle.getParameter("ego", "device.driverstate.perceivedHeadway.leader"))
print("override speedDiff:", traci.vehicle.getParameter("ego", "device.driverstate.perceivedSpeedDifference.leader"))
speed_override = traci.vehicle.getSpeed("ego")
print("ego speed with close perceived leader: %.2f (braked=%s)" % (speed_override, speed_override < speed_before))

traci.vehicle.setParameter("ego", "device.driverstate.perceivedHeadway.leader", "")
traci.vehicle.setParameter("ego", "device.driverstate.perceivedSpeedDifference.leader", "")
traci.simulationStep()

print("restored headway:", traci.vehicle.getParameter("ego", "device.driverstate.perceivedHeadway.leader"))
print("ego speed after release: %.2f" % traci.vehicle.getSpeed("ego"))

traci.close()
