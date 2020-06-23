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

ANGLE_UNDEF = traci.constants.INVALID_DOUBLE_VALUE
INVALID = traci.constants.INVALID_DOUBLE_VALUE

vehID = "v0"


def check(x, y, angle, exLane, exPos, exPosLat, comment):
    traci.vehicle.moveToXY(vehID, "", 0, x, y, angle, keepRoute=2)
    traci.simulationStep()
    x2, y2 = traci.vehicle.getPosition(vehID)
    lane2 = traci.vehicle.getLaneID(vehID)
    pos2 = traci.vehicle.getLanePosition(vehID)
    posLat2 = traci.vehicle.getLateralLanePosition(vehID)
    if (abs(x - x2) > 0.1 or
            abs(y - y2) > 0.1 or
            exLane != lane2 or
            (exPos is not None and abs(exPos - pos2) > 0.1) or
            (exPosLat is not None and abs(exPosLat - posLat2) > 0.1)):
        print(comment, ("failed: x=%s, x2=%s,   y=%s, y2=%s,   lane=%s, lane2=%s, pos=%s, pos2=%s   " +
                        "posLat=%s posLat2=%s") % (x, x2, y, y2, exLane, lane2, exPos, pos2, exPosLat, posLat2))
    else:
        # (comment, "success")
        pass


traci.start([sumolib.checkBinary("sumo"), '-n', 'input_net.net.xml', '--no-step-log'])
traci.simulationStep()
traci.route.add("beg", ["beg"])
traci.vehicle.add(vehID, "beg")
check(-1,  0,   ANGLE_UNDEF, "",         None, None,       "1m before the start of the edge")
check(-5,  0,   ANGLE_UNDEF, "",         None, None,       "5m before the start of the edge")
check(198, 1.9, ANGLE_UNDEF, "middle_0", 98.0, 1.9,        "internal corner (inside, segment1)")
check(198, 1.9, 0,           "middle_0", 102,  2.0,        "internal corner (inside, segment2)")
check(201, -1, 0,            "middle_0", 100, -1.41,       "internal corner (outside, near)")
check(203, -4, 0,            "", INVALID, INVALID,         "internal corner (outside, far)")
print("vehicleList", traci.vehicle.getIDList())
traci.close()
