#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2017-01-23
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa
import traci  # noqa

sumoBinary = sumolib.checkBinary("sumo")
traci.start([sumoBinary,
             '-n', 'input_net.net.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log',
             # '-S', '-Q'
             ])


vehID = 'v0'
traci.simulationStep()
while traci.simulation.getTime() < 75:
    pos = traci.vehicle.getPosition3D(vehID)
    pos2 = (pos[0] + 20, pos[1])
    traci.vehicle.moveToXY('v0', '', 0, pos2[0], pos2[1])
    traci.simulationStep()
    pos3 = traci.vehicle.getPosition3D(vehID)
    print("step=%s old=%s target=%s mapped=%s" % (
        traci.simulation.getTime(), pos, pos2, pos3))
traci.close()
