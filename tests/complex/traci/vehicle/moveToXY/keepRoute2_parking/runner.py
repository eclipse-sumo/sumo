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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"),
             '-n', 'input_net.net.xml',
             '-a', 'input_additional.add.xml',
             '-r', 'input_routes.rou.xml',
             '--keep-after-arrival', '3',
             '--fcd-output', 'fcd.xml',
             '--no-step-log',
             # '-C', 'debug.sumocfg'
             ])
traci.simulationStep()
vehID = traci.vehicle.getIDList()[0]

maneuver = [
    (-3, 0, -30),
    (-1, -3, -30),
    (0, -3, -30),
]
maneuverStart = None
maneuverDuration = len(maneuver)

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    t = traci.simulation.getTime()
    if traci.vehicle.isStopped(vehID):
        if maneuverStart is None:
            maneuverStart = t
        if t < maneuverStart + maneuverDuration:
            pos = traci.vehicle.getPosition(vehID)
            a = traci.vehicle.getAngle(vehID)
            dx, dy, da = maneuver[int(t - maneuverStart)]
            traci.vehicle.moveToXY(vehID, '', 0, pos[0] + dx, pos[1] + dy, a + da, keepRoute=2)
traci.simulationStep()
traci.simulationStep()
traci.simulationStep()
traci.simulationStep()
traci.close()
