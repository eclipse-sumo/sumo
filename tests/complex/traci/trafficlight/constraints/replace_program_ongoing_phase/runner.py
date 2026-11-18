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

traci.start([sumolib.checkBinary("sumo"), "-n", "input_net2.net.xml", "--no-step-log"] + sys.argv[1:])

for _ in range(3):
    traci.simulationStep()

print("before replace: t=%.1f phase=%d nextSwitch=%.1f spent=%.1f" % (
    traci.simulation.getTime(),
    traci.trafficlight.getPhase("C"),
    traci.trafficlight.getNextSwitch("C"),
    traci.trafficlight.getSpentDuration("C"),
))

logic = traci.trafficlight.getAllProgramLogics("C")[0]
logic.phases[0].duration = 10.0
logic.phases[0].minDur = 10.0
logic.phases[0].maxDur = 10.0
traci.trafficlight.setCompleteRedYellowGreenDefinition("C", logic)

print("after replace: t=%.1f phase=%d nextSwitch=%.1f spent=%.1f" % (
    traci.simulation.getTime(),
    traci.trafficlight.getPhase("C"),
    traci.trafficlight.getNextSwitch("C"),
    traci.trafficlight.getSpentDuration("C"),
))

for _ in range(9):
    traci.simulationStep()
    print("t=%.1f phase=%d nextSwitch=%.1f spent=%.1f" % (
        traci.simulation.getTime(),
        traci.trafficlight.getPhase("C"),
        traci.trafficlight.getNextSwitch("C"),
        traci.trafficlight.getSpentDuration("C"),
    ))

traci.close()
