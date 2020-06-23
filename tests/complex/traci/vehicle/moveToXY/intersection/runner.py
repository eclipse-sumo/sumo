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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
cmd = [sumoBinary,
       '-n', 'input_net2.net.xml',
       '-r', 'input_routes.rou.xml',
       '--no-step-log',
       '--duration-log.statistics',
       '--tripinfo-output.write-unfinished',
       '--collision.check-junctions',
       '--collision.action', 'warn',
       # '-S', '-Q',
       ]

ANGLE_UNDEF = traci.constants.INVALID_DOUBLE_VALUE
INVALID = traci.constants.INVALID_DOUBLE_VALUE

vehID = "ego"

traci.start(cmd)
traci.simulationStep()
x, y = traci.vehicle.getPosition(vehID)
for i in range(25):
    traci.vehicle.moveToXY(vehID, "", 0, x, y)
    traci.simulationStep()
    y += 5
traci.close()
