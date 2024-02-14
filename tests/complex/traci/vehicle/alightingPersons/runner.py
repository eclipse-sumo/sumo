#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer
# @date    2024-02-14

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net2.net.xml",
             "-a", "input_additional.add.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             ] + sys.argv[1:])

vehID = "bus"

while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    if t % 10 == 0:
        passengerIDs = traci.vehicle.getPersonIDList(vehID)
        nextStops = traci.vehicle.getStops(vehID, limit=1)
        if len(nextStops) > 0:
            stopID = nextStops[0].stoppingPlaceID
            stopEdge = nextStops[0].lane.rsplit("_",1)[1]
            alightingPassengers = []
            for passengerID in passengerIDs:
                nextStage = traci.person.getStage(passengerID)
                if nextStage.destStop == stopID:
                    alightingPassengers.append(passengerID)
            print("time", t)
            print("  next stop", stopID)
            print("  alighting passengers", str(alightingPassengers))
    traci.simulationStep()

traci.close()
