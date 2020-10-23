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
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import traci.constants as tc  # noqa
import sumolib  # noqa


traci.start([
    sumolib.checkBinary('sumo'),
    "-c", "sumo.sumocfg",
    '--device.ssm.probability', '1',
])


def checkSSM(vehID):
    print("  veh=%s minTTC=%s maxDRAC=%s minPET=%s" % (
        vehID,
        traci.vehicle.getParameter(vehID, "device.ssm.minTTC"),
        traci.vehicle.getParameter(vehID, "device.ssm.maxDRAC"),
        traci.vehicle.getParameter(vehID, "device.ssm.minPET"),
    ))


for i in range(15):
    traci.simulationStep()
    print("step %s" % i)
    checkSSM("ego")
    checkSSM("leader")
traci.close()
