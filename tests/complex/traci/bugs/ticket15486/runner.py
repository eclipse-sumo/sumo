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
# @date    2024-09-16

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-a", "input_additional.add.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             "--tripinfo-output", "tripinfo.xml"])

vehicle_id = "v_0"
route_id = "r_0"
traci.vehicle.add(
    vehicle_id,
    route_id,
    typeID="DEFAULT_VEHTYPE",
    departSpeed="desired",
    departLane="random",
)
parking_stop_flags = (
    traci.constants.STOP_PARKING
    | traci.constants.STOP_PARKING_AREA
    | traci.constants.STOP_TRIGGERED
)

parking_area = "pa_0"
traci.vehicle.setParkingAreaStop(
    vehicle_id, parking_area, duration=0, flags=parking_stop_flags
)
traci.vehicle.setStopParameter(vehicle_id, 0, "expected", "p0 p1")

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
traci.close()
