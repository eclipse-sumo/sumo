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

# @file    runner.py
# @author  Mirko Barthauer
# @date    2024-12-05


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net.net.xml', '-r', 'input_routes.rou.xml',
             '--ignore-route-errors',
             '--additional-files',
             'input_additional.add.xml',
             '--no-step-log', 'true',
             "--default.speeddev", "0"] + sys.argv[1:])
vehID = "el"
s = step()
for i in range(20):
    if "el" in traci.vehicle.getIDList():
        print(('%.2f batteryNeed="%s" chargingStation="%s" chargePower="%s" usedAverage="%s"') % (
            s, traci.vehicle.getParameter(vehID, "device.stationfinder.batteryNeed"),
            traci.vehicle.getParameter(vehID, "device.stationfinder.chargingStation"),
            traci.vehicle.getParameter(vehID, "device.battery.chargePower"),
            traci.vehicle.getParameter(vehID, "device.battery.usedAverage"),
        ))
    s = step()

# done
traci.close()
