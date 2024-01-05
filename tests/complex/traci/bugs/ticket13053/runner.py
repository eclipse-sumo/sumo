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
# @date    2022-10-26


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log"
             ] + sys.argv[1:])

vehID = "ego"

traci.vehicle.moveToXY(vehID, '-1', 0, 90.90080210811013, 54.132537576628486, 170.1306746294961, keepRoute=3)
traci.vehicle.setSpeed(vehID, 5.954216768368333)
print("lane=%s routePos=%s" % (traci.vehicle.getLaneID(vehID), traci.vehicle.getRouteIndex(vehID)))
traci.simulation.step()
traci.vehicle.moveToXY(vehID, '-1', 0, 90.83989420896337, 53.51959570314981, 172.41709555246553, keepRoute=3)
traci.vehicle.setSpeed(vehID, 5.982724203690141)
print("lane=%s routePos=%s" % (traci.vehicle.getLaneID(vehID), traci.vehicle.getRouteIndex(vehID)))
traci.simulation.step()
traci.vehicle.moveToXY(vehID, '-1', 0, 90.76993198064643, 52.908392067778, 174.4963732039921, keepRoute=3)
traci.vehicle.setSpeed(vehID, 5.996801175697893)
print("lane=%s routePos=%s" % (traci.vehicle.getLaneID(vehID), traci.vehicle.getRouteIndex(vehID)))
traci.simulation.step()
print("lane=%s routePos=%s" % (traci.vehicle.getLaneID(vehID), traci.vehicle.getRouteIndex(vehID)))

traci.close()
