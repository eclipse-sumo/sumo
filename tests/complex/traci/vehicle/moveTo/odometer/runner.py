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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

WATCH = False

sumoBinary = sumolib.checkBinary("sumo")
cmd = [
    sumoBinary,
    '-n', 'input_net.net.xml',
    '-r', 'input_routes.rou.xml',
    '--no-step-log', ]
if not WATCH:
    cmd += ['-S', '-Q']

traci.start(cmd)

vehID = "ego"

traci.simulationStep()
x, y = traci.vehicle.getPosition(vehID)

for i in range(20):
    print(traci.simulation.getTime(), "distance",
          traci.vehicle.getDistance(vehID))
    x += 10
    edgeID, pos, laneIndex = traci.simulation.convertRoad(x, y)
    laneID = "%s_%s" % (edgeID, laneIndex)
    traci.vehicle.moveTo(vehID, laneID, pos)
    traci.simulationStep()

traci.close()
