#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"),
             '-n', 'input_net2.net.xml',
             '-r', 'input_routes2.rou.xml',
             '--no-step-log',
             # '-C', 'debug.sumocfg',
             '-S', '-Q'
             ])

vehID = "ego"
locations = []
traci.simulationStep()
for i in range(10):
    locations.append((traci.vehicle.getLaneID(vehID),
                      traci.vehicle.getLanePosition(vehID)))
    traci.simulationStep()

print(locations)
for lane, pos in locations:
    traci.vehicle.moveTo(vehID, lane, pos)
    traci.simulationStep()

traci.close()
