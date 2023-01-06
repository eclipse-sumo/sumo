#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net2.net.xml",
             "--no-step-log",
             ] + sys.argv[1:])
tlsID = "C"
for step in range(100):
    print(traci.simulation.getTime(), " phase=%s nextSwitch=%s" % (
        traci.trafficlight.getPhase(tlsID),
        traci.trafficlight.getNextSwitch(tlsID)
    ))
    traci.simulationStep()


print("replace logic")
logic = traci.trafficlight.getAllProgramLogics(tlsID)[0]
logic.programID = "custom"  # replacing existing program id works fine but setting a custom id triggers #2238

traci.trafficlight.setProgramLogic(tlsID, logic)
# traci.trafficlight.setPhaseDuration(tlsID, 5) # will work fine

for step in range(100):
    print(traci.simulation.getTime(), " phase=%s nextSwitch=%s" % (
        traci.trafficlight.getPhase(tlsID),
        traci.trafficlight.getNextSwitch(tlsID)
    ))
    traci.simulationStep()


traci.close()
