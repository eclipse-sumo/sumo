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

import sumolib  # noqa
import traci  # noqa

sumoBinary = sumolib.checkBinary("sumo")
traci.start([sumoBinary,
             '-n', 'input_net.net.xml',
             '--no-step-log',
             # '-S', '-Q'
             ])

ANGLE_UNDEF = traci.constants.INVALID_DOUBLE_VALUE
INVALID = traci.constants.INVALID_DOUBLE_VALUE
vehID = "v0"


def check(x, y, angle, exZ, comment):
    traci.vehicle.moveToXY(vehID, "", 0, x, y, angle, keepRoute=2)
    traci.simulationStep()
    x2, y2 = traci.vehicle.getPosition(vehID)
    z2 = traci.vehicle.getPosition3D(vehID)[2]
    if (abs(x - x2) > 0.1 or
            abs(y - y2) > 0.1 or
            abs(exZ - z2) > 0.1):
        print(comment, ("failed: exZ=%s, z2=%s" % (exZ, z2)))
    else:
        # (comment, "success")
        pass


traci.simulationStep()
traci.route.add("beg", ["beg"])
traci.vehicle.add(vehID, "beg")
check(50, 0, ANGLE_UNDEF, 5,        "ascending beg")
check(150, 50, ANGLE_UNDEF, 10,        "middle")
check(250, 100, ANGLE_UNDEF, 5,        "descending end")
check(250, 110, ANGLE_UNDEF, 5,        "outside network, use last position")
check(1250, 1100, ANGLE_UNDEF, 5,        "far outside network, use last position")
traci.close()
