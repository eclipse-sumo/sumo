#!/usr/bin/env python
# -*- coding: utf-8 -*-
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


import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa
import traci  # noqa


def run(change_lane):
    traci.start([checkBinary("sumo"), "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
                 "--step-length", "0.1", "--default.action-step-length", "1", "--step-method.ballistic",
                 "--lateral-resolution", "0.2", "--no-step-log"] + sys.argv[1:])
    traci.simulationStep()
    if change_lane:
        traci.vehicle.changeLane("changing", 1, 5)
    departed = None
    collisions = 0
    while traci.simulation.getTime() < 6:
        traci.simulationStep()
        if "inserting" in traci.simulation.getDepartedIDList():
            departed = traci.simulation.getTime()
        collisions += traci.simulation.getCollidingVehiclesNumber()
    traci.close()
    print("Reservation:", change_lane, "insertion time:", departed, "colliding vehicles:", collisions)


run(False)
run(True)
