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
# @author  Mark Colley
# @date    2026-11-20


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net2.net.xml',
             '-a', 'input_additional.add.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log',
             ] + sys.argv[1:])

CS = "cs1"
VEH = "v0"


def getBattery(key):
    return float(traci.vehicle.getParameter(VEH, "device.battery." + key))


print("chargeRampUp", traci.chargingstation.getParameter(CS, "chargeRampUp"))
print("chargeRampDown", traci.chargingstation.getParameter(CS, "chargeRampDown"))
print("chargeStopDelay", traci.chargingstation.getParameter(CS, "chargeStopDelay"))
print("actualPower", traci.chargingstation.getParameter(CS, "actualPower"))

for badKey, badVal in [("chargeRampUp", "-1"), ("chargeStopDelay", "-2"), ("actualPower", "100")]:
    try:
        traci.chargingstation.setParameter(CS, badKey, badVal)
    except traci.TraCIException as e:
        print("caught", e)

for step in range(36):
    t = traci.simulation.getTime()
    if t == 18:
        # switch from charging (+10000 W) to V2G discharging (-8000 W) with faster ramp-down
        traci.chargingstation.setParameter(CS, "chargeRampDown", "5000")
        traci.chargingstation.setParameter(CS, "chargeRampUp", "4000")
        traci.chargingstation.setParameter(CS, "chargeStopDelay", "4")
        traci.vehicle.setParameter(VEH, "device.battery.maximumDischargeRate", "20000")
    traci.simulationStep()
    if VEH in traci.vehicle.getIDList():
        print("time=%s cs=%s actualPower=%s energyCharged=%.4f energyDischarged=%.4f chargeLevel=%.4f" % (
            traci.simulation.getTime(),
            traci.vehicle.getParameter(VEH, "device.battery.chargingStationId"),
            traci.chargingstation.getParameter(CS, "actualPower"),
            getBattery("energyCharged"),
            getBattery("energyDischarged"),
            getBattery("chargeLevel")))

print("afterActualPower", traci.chargingstation.getParameter(CS, "actualPower"))
traci.close()
