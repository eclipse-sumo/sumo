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
# @date    2026-11-18


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


def setBattery(key, value):
    traci.vehicle.setParameter(VEH, "device.battery." + key, str(value))


print("dischargePower", traci.chargingstation.getDischargePower(CS))
try:
    traci.chargingstation.setDischargePower(CS, -1)
except traci.TraCIException as e:
    print("caught", e)
traci.chargingstation.setDischargePower(CS, 7200)
print("dischargePower", traci.chargingstation.getDischargePower(CS))

traci.simulationStep()
print("maximumDischargeRate", getBattery("maximumDischargeRate"))
print("minimumChargeLevel", getBattery("minimumChargeLevel"))
try:
    setBattery("maximumDischargeRate", "fast")
except traci.TraCIException as e:
    print("caught", e)

# vehicle-limited discharging (3600 W, i.e. 1 Wh per step) down to 12980 Wh
setBattery("maximumDischargeRate", 3600)
setBattery("minimumChargeLevel", 12980)
print("maximumDischargeRate", getBattery("maximumDischargeRate"))
print("minimumChargeLevel", getBattery("minimumChargeLevel"))

for step in range(1, 50):
    t = traci.simulation.getTime()
    if t == 15:
        # station-limited discharging (7200 W / 0.9 taken from the battery)
        setBattery("maximumDischargeRate", 36000)
    elif t == 30:
        # stop discharging, the vehicle is charged again
        setBattery("maximumDischargeRate", 0)
    elif t == 35:
        # the station no longer accepts discharging
        setBattery("maximumDischargeRate", 36000)
        traci.chargingstation.setDischargePower(CS, 0)
    traci.simulationStep()
    if VEH in traci.vehicle.getIDList():
        print("time=%s cs=%s chargeLevel=%.4f energyCharged=%.4f energyDischarged=%.4f totalEnergyDischarged=%.4f" % (
            traci.simulation.getTime(),
            traci.vehicle.getParameter(VEH, "device.battery.chargingStationId"),
            getBattery("chargeLevel"),
            getBattery("energyCharged"),
            getBattery("energyDischarged"),
            getBattery("totalEnergyDischarged")))

traci.close()
