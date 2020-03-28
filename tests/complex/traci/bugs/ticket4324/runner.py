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
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa


sumoBinary = os.environ["SUMO_BINARY"]

cmd = [
    sumoBinary,
    '-n', 'input_net2.net.xml',
    '--no-step-log', ]

tlsID = "C"

traci.start(cmd)
program = traci.trafficlight.getCompleteRedYellowGreenDefinition(tlsID)[0]
# make actuated
green_index = 1
for phase in program.getPhases():
    if 'G' in phase.state:
        phase.minDur = 5
        phase.maxDur = 35
        phase.name = "Major %s" % green_index
        green_index += 1
    else:
        phase.name = "Transition"
program.type = 3  # actuated
program.programID = "custom_actuated"
traci.trafficlight.setCompleteRedYellowGreenDefinition(tlsID, program)

for program in traci.trafficlight.getCompleteRedYellowGreenDefinition(tlsID):
    print("program", program)

for i in range(120):
    traci.simulationStep()
    print("step=%s phase=%s name=%s" % (i, traci.trafficlight.getPhase(tlsID),
                                        traci.trafficlight.getPhaseName(tlsID)))

traci.close()
