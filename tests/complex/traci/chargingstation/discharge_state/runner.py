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
# @date    2026-11-24


import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

traci.start([sumolib.checkBinary("sumo"), "-n", "input_net2.net.xml", "-a", "input_additional.add.xml",
             "-r", "input_routes.rou.xml", "--no-step-log", "--duration-log.disable"] + sys.argv[1:])

vehicle = "v0"
limits = ("device.battery.maximumDischargeRate", "device.battery.minimumChargeLevel")


def read_limits():
    return tuple(float(traci.vehicle.getParameter(vehicle, key)) for key in limits)


try:
    traci.simulationStep()
    assert read_limits() == (3600, 10000), read_limits()
    for key in limits:
        traci.vehicle.setParameter(vehicle, key, "0")
    for attempt in range(2):
        filename = "discharge_state_%s.xml" % attempt
        traci.simulation.saveState(filename)
        traci.simulation.loadState(filename)
        assert read_limits() == (0, 0), read_limits()
    traci.simulationStep(45)
    assert float(traci.vehicle.getParameter(vehicle, "device.battery.totalEnergyDischarged")) == 0
    print("Disabled V2G limits preserved across consecutive state reloads.")
finally:
    traci.close()
