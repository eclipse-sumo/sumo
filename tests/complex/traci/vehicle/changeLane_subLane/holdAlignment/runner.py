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
# @author  Jakob Erdmann
# @date    2018-09-27

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import sumolib  # noqa
import traci  # noqa


traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net.net.xml",
             "--lateral-resolution", "0.2", "--no-step-log"] + sys.argv[1:])
try:
    traci.route.add("r", ["gneE0"])
    traci.vehicletype.copy("DEFAULT_VEHTYPE", "narrow")
    traci.vehicletype.setWidth("narrow", 1)
    traci.vehicletype.setLateralAlignment("narrow", "center")
    traci.vehicle.add("aligned", "r", typeID="narrow")
    traci.vehicle.add("unaligned", "r", typeID="narrow", departPos="100")
    traci.vehicle.setLaneChangeMode("unaligned", 512)
    traci.simulationStep()
    for vehicleID in ("aligned", "unaligned"):
        traci.vehicle.changeLane(vehicleID, 1, 30)

    for _ in range(6):
        traci.simulationStep()
    for _ in range(8):
        assert traci.vehicle.getLaneIndex("aligned") == 1
        assert traci.vehicle.getLaneIndex("unaligned") == 1
        assert abs(traci.vehicle.getLateralLanePosition("aligned")) < 0.01
        assert abs(traci.vehicle.getLateralLanePosition("unaligned")) > 0.5
        traci.simulationStep()
finally:
    traci.close()
