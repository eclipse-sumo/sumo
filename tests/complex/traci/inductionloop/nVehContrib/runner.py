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
    "-a", "input_additional.add.xml",
    "-r", "input_routes.rou.xml",
    "--no-step-log",
] + extra_args)

traci_contrib = []
for step in range(1, 16):
    if step == 12 and "v_early_lc" in traci.vehicle.getIDList():
        traci.vehicle.changeLane("v_early_lc", 1, 10.0)
    traci.simulationStep()
    last_num = traci.inductionloop.getLastStepVehicleNumber("e1_0")
    contrib = int(traci.inductionloop.getParameter("e1_0", "nVehContrib"))
    traci_contrib.append(contrib)
    if last_num > 0 or contrib > 0:
        print("t=%d lastStepVehNum=%d nVehContrib=%d" % (step, last_num, contrib))

traci.close()

xml_contrib = [int(iv.nVehContrib) for iv in sumolib.xml.parse("e1.xml", "interval")][:15]
print("matches XML nVehContrib:", traci_contrib == xml_contrib)
