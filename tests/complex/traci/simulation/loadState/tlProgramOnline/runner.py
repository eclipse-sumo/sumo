#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa


def check():
    print("%s subcription=%s contextSubscription=%s" % (
        traci.simulation.getTime(),
        traci.vehicle.getAllSubscriptionResults(),
        traci.vehicle.getAllContextSubscriptionResults()))


cmd = [sumolib.checkBinary('sumo'),
       "-n", "input_net2.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
       ]

traci.start(cmd)

veh = "v0"
tlID = "C"
tlState = traci.trafficlight.getRedYellowGreenState(tlID)
tlState2 = 'G' * len(tlState)
# switch to program "online"
traci.trafficlight.setRedYellowGreenState(tlID, tlState2)
traci.simulation.saveState("state.xml")
traci.close()
traci.start(cmd)
for i in range(2):
    traci.simulation.loadState("state.xml")
    traci.simulationStep()
    traci.vehicle.add(veh, "r0")
    traci.vehicle.subscribe(veh, [traci.constants.VAR_SPEED])
    traci.simulationStep()
    check()

traci.close()
