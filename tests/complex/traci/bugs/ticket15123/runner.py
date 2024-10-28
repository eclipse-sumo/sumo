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
# @date    2023-11-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-n", "input_net2.net.xml",
             "--no-step-log",
             "--vehroute-output", "vehroutes.xml"])

vehID = "ego"
traci.vehicle.add(vehID, "")
traci.vehicle.moveToXY(vehID, edgeID="", laneIndex=-1, x=98.17, y=166.49, keepRoute=2)

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    if traci.simulation.getTime() == 1:
        traci.vehicle.changeTarget(vehID, "CS")
traci.close()
