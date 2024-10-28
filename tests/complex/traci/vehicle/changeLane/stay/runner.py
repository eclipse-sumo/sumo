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
# @author  Jakob Erdmann
# @date    2018-09-27

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--lanechange-output", "lanechanges.xml",
             "--no-step-log",
             ])


def reportState(vehID, direction):
    print("t=%s laneIndex=%s state(%s)=%s" % (
        traci.simulation.getTime(),
        traci.vehicle.getLaneIndex(vehID),
        direction,
        traci.vehicle.getLaneChangeStatePretty(vehID, direction)))


vehID = "v0"
traci.vehicle.add(vehID, "r0", arrivalLane="2")
traci.simulationStep()
traci.vehicle.changeLaneRelative(vehID, 0, 8)
for i in range(10):
    reportState(vehID, 1)
    traci.simulationStep()

traci.close()
