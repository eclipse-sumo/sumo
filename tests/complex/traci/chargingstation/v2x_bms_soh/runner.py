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


KEYS = ("chargeLevel", "effectiveCapacity", "stateOfHealth", "minimumSoC", "maximumSoC",
        "totalEnergyCharged", "totalEnergyDischarged", "equivalentFullCycles")


def battery(veh, key):
    return float(traci.vehicle.getParameter(veh, "device.battery." + key))


def report(veh):
    print("time=%s %s %s" % (traci.simulation.getTime(), veh,
                             " ".join("%s=%.4f" % (k, battery(veh, k)) for k in KEYS)))


for badKey, badVal in [("minimumSoC", "-0.1"), ("maximumSoC", "1.5"), ("stateOfHealth", "0"),
                       ("capacityLossPerCycle", "-1")]:
    traci.vehicle.setParameter("v0", "device.battery." + badKey, badVal)
    print("after invalid", badKey, battery("v0", badKey))

seen = set()
while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    if t == 100:
        # tighten the BMS window of v1 while it is charging: charging stops immediately at the new ceiling
        traci.vehicle.setParameter("v1", "device.battery.maximumSoC", "0.49")
    traci.simulationStep()
    for veh in traci.vehicle.getIDList():
        stopped = traci.vehicle.getParameter(veh, "device.battery.chargingStationId") != "NULL"
        if stopped or veh not in seen:
            report(veh)
        if stopped:
            seen.add(veh)

traci.close()
