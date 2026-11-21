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
import time

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net2.net.xml",
    "-r", "input_routes.rou.xml",
    "--statistic-output", "stats.xml",
    "--no-step-log",
] + sys.argv[1:] + ["--no-duration-log", "false"])

veh_steps = 0
pers_steps = 0
for _ in range(10):
    time.sleep(0.03)
    traci.simulationStep()
    veh_steps += traci.vehicle.getIDCount()
    pers_steps += traci.person.getIDCount()

traci.close()

perf = list(sumolib.xml.parse("stats.xml", "performance"))[0]
clock_dur = float(perf.clockDuration)
traci_dur = float(perf.traciDuration)
veh_ups = float(perf.vehicleUpdatesPerSecond)
pers_ups = float(perf.personUpdatesPerSecond)

naive_veh_ups = veh_steps / clock_dur
naive_pers_ups = pers_steps / clock_dur

print("veh_steps > 0 and pers_steps > 0:", veh_steps > 0 and pers_steps > 0)
print("clockDuration >= 0.25:", clock_dur >= 0.25)
if not traci.isLibsumo():
    print("traciDuration >= 0.20:", traci_dur >= 0.20)
else:
    print("traciDuration >= 0.20:", True)
print("vehUPS > 5x naive clock UPS:", veh_ups > naive_veh_ups * 5.0)
print("persUPS > 5x naive clock UPS:", pers_ups > naive_pers_ups * 5.0)
