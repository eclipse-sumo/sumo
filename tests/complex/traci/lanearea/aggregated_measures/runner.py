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

ATTRS = [
    "sampledSeconds",
    "nVehEntered",
    "nVehLeft",
    "nVehSeen",
    "meanSpeed",
    "meanTimeLoss",
    "meanOccupancy",
    "maxOccupancy",
    "meanMaxJamLengthInVehicles",
    "meanMaxJamLengthInMeters",
    "maxJamLengthInVehicles",
    "maxJamLengthInMeters",
    "jamLengthInVehiclesSum",
    "jamLengthInMetersSum",
    "meanHaltingDuration",
    "maxHaltingDuration",
    "haltingDurationSum",
    "meanIntervalHaltingDuration",
    "maxIntervalHaltingDuration",
    "intervalHaltingDurationSum",
    "startedHalts",
    "meanVehicleNumber",
    "maxVehicleNumber",
]

extra_args = [a for a in sys.argv[1:] if not a.startswith("--meso")]
traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net2.net.xml",
    "-a", "input_additional.add.xml",
    "-r", "input_routes.rou.xml",
    "--no-step-log",
] + extra_args)

for _ in range(30):
    traci.simulationStep()

retrieved = {}
for attr in ATTRS:
    val = traci.lanearea.getParameter("e2_0", attr)
    retrieved[attr] = val
    print("%s=%s" % (attr, val))

traci.close()

iv = list(sumolib.xml.parse("e2.xml", "interval"))[0]
mismatches = [a for a in ATTRS if retrieved[a] != getattr(iv, a)]
print("all 23 XML measures match:", len(mismatches) == 0, mismatches)
