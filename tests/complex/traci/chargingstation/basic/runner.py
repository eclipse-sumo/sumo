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
# @author   Mirko Barthauer
# @date    2020-03-16


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
             '-a', 'input_additional2.add.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log',
             ])

print("chargingstations", traci.chargingstation.getIDList())
print("chargingstation count", traci.chargingstation.getIDCount())
print("stop attributes before setting values:")
for stop in traci.chargingstation.getIDList():
    print("  stop=%s lane=%s startPos=%s endPos=%s name=%s power=%.0f efficiency=%.2f chargeDelay=%.2f chargeInTransit=%d" % (  # noqa
        stop,
        traci.chargingstation.getLaneID(stop),
        traci.chargingstation.getStartPos(stop),
        traci.chargingstation.getEndPos(stop),
        traci.chargingstation.getName(stop),
        traci.chargingstation.getChargingPower(stop),
        traci.chargingstation.getEfficiency(stop),
        traci.chargingstation.getChargeDelay(stop),
        traci.chargingstation.getChargeInTransit(stop))
        )
    # setter functions
print("stop attributes after setting values:")
for stop in traci.chargingstation.getIDList():
    traci.chargingstation.setChargingPower(stop, 50000.)
    traci.chargingstation.setEfficiency(stop, 1.)
    traci.chargingstation.setChargeDelay(stop, 0.)
    traci.chargingstation.setChargeInTransit(stop, True)
    print("  stop=%s power=%.0f efficiency=%.2f chargeDelay=%.2f chargeInTransit=%d" % (
        stop,
        traci.chargingstation.getChargingPower(stop),
        traci.chargingstation.getEfficiency(stop),
        traci.chargingstation.getChargeDelay(stop),
        traci.chargingstation.getChargeInTransit(stop))
        )

for step in range(50):
    if step % 5 == 0:
        print("time:", traci.simulation.getTime())
        for stop in traci.chargingstation.getIDList():
            print("  stop=%s vC=%s vIDs=%s" % (
                stop,
                traci.chargingstation.getVehicleCount(stop),
                traci.chargingstation.getVehicleIDs(stop)))
    traci.simulationStep()

traci.close()
