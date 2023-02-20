#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
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
# @date    2020-10-15

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"), "-c",
             "data/OnlyNetQuickstart.sumocfg", "--no-step-log"] + sys.argv[1:])

traci.vehicle.add(vehID="bus_1", typeID="BUS", routeID="", depart=0, departPos=0, departSpeed=0, departLane=1)
traci.vehicle.setRoute("bus_1", ["L12", "L15", "L5"])
traci.vehicle.setStop(vehID="bus_1", edgeID="bus_stop_1", pos=0, laneIndex=1, duration=50, flags=tc.STOP_BUS_STOP)
traci.vehicle.setStop(vehID="bus_1", edgeID="bus_stop_2", pos=0, laneIndex=1, duration=50, flags=tc.STOP_BUS_STOP)

traci.person.add(personID="P4", edgeID="L12", pos=15, depart=0, typeID='DEFAULT_PEDTYPE')
stage = traci.simulation.Stage(type=tc.STAGE_DRIVING, line="ANY", edges=["L5"],
                               departPos=0, arrivalPos=79.34, destStop='bus_stop_2', description="foo")
traci.person.appendStage("P4", stage)
traci.person.appendWaitingStage(personID="P4", duration=1000, description='thinking', stopID='bus_stop_2')

traci.person.add(personID="P5", edgeID="L12", pos=15, depart=20, typeID='DEFAULT_PEDTYPE')
stage = traci.simulation.Stage(type=tc.STAGE_DRIVING, line="ANY", edges=["L5"], departPos=0, destStop='bus_stop_2')
traci.person.appendStage("P5", stage)
traci.person.appendWaitingStage(personID="P5", duration=1000, description='thinking', stopID='bus_stop_2')

step = 0
while step < 1500:
    traci.simulationStep()
    step += 1

traci.close()
