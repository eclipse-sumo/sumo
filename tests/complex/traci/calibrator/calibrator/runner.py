#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @date    2020-03-16


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
print("calibrators", traci.calibrator.getIDList())
print("calibrator count", traci.calibrator.getIDCount())

for step in range(50):
    traci.simulationStep()

for cali in traci.calibrator.getIDList():
    print("examining", cali)
    print("  edge", traci.calibrator.getEdgeID(cali))
    print("  lane", traci.calibrator.getLaneID(cali))
    print("  vehsPerHour", traci.calibrator.getVehsPerHour(cali))
    print("  speed", traci.calibrator.getSpeed(cali))
    print("  begin", traci.calibrator.getBegin(cali))
    print("  end", traci.calibrator.getEnd(cali))
    print("  getRouteID", traci.calibrator.getRouteID(cali))
    print("  getRouteProbeID", traci.calibrator.getRouteProbeID(cali))
    print("  getVTypes", traci.calibrator.getVTypes(cali))
    print("  getPassed", traci.calibrator.getPassed(cali))
    print("  getInserted", traci.calibrator.getInserted(cali))
    print("  getRemoved", traci.calibrator.getRemoved(cali))
    traci.calibrator.setParameter(cali, "test", "42")
    print("  getParameter", traci.calibrator.getParameter(cali, "test"))

traci.calibrator.setFlow("cali_WE", 100, 200, 1800, 15, "DEFAULT_VEHTYPE", "WS")
traci.calibrator.setFlow("cali_SN", 0, 200, 300, -1, "DEFAULT_VEHTYPE", "SN")

for step in range(250):
    traci.simulationStep()

traci.close()
