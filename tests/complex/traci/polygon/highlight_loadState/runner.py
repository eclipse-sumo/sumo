#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-21

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg", "--no-warnings"] + sys.argv[1:])
for _ in range(3):
    traci.simulationStep()

traci.polygon.add("static_poly", ((0, 0), (0, 10), (10, 10), (10, 0)), (255, 0, 0, 255), True, "test")
traci.simulation.saveState("saved_snapshot.xml")

traci.vehicle.highlight("horiz", (0, 255, 0, 255))
print("before_load:", sorted(traci.polygon.getIDList()))

traci.simulation.loadState("saved_snapshot.xml")
print("after_load:", sorted(traci.polygon.getIDList()))

for _ in range(3):
    traci.simulationStep()
print("after_steps:", sorted(traci.polygon.getIDList()))
traci.close()
