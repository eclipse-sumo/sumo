#!/usr/bin/env python
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
# @author  Josh
# @date    2026-11-26

import math
import os
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
from traci import constants as tc  # noqa


def check_gap(expected):
    actual = traci.inductionloop.getLastNetTimeGap("point")
    assert math.isclose(expected, actual, abs_tol=1e-9), (expected, actual)


args = ["-n", "input_net.net.xml", "-r", "input_routes.rou.xml", "-a", "input_additional.add.xml",
        "--save-state.precision", "12", "--no-step-log", "--no-duration-log"] + sys.argv[1:]
traci.start([sumolib.checkBinary("sumo")] + args)
try:
    check_gap(-1)
    traci.inductionloop.subscribe("point", (tc.LAST_NET_TIME_GAP,))

    traci.simulationStep(5)
    leader, = traci.inductionloop.getVehicleData("point")
    assert leader[0] == "leader", leader
    check_gap(-1)
    traci.simulation.saveState("empty.xml")

    traci.simulationStep(9)
    follower, = traci.inductionloop.getVehicleData("point")
    assert follower[0] == "follower", follower
    gap = follower[2] - leader[3]
    assert 4 < gap < 5, gap
    check_gap(gap)
    assert traci.inductionloop.getSubscriptionResults("point")[tc.LAST_NET_TIME_GAP] == gap
    assert traci.inductionloop.getLastNetTimeGap("overlap") == 0
    traci.inductionloop.overrideTimeSinceDetection("point", 0)
    assert traci.inductionloop.getTimeSinceDetection("point") == 0
    check_gap(gap)

    traci.simulation.saveState("occupied.xml")
    traci.simulation.loadState("occupied.xml")
    check_gap(gap)

    traci.load(args + ["--load-state", "empty.xml"])
    traci.simulationStep(9)
    check_gap(gap)

    legacy = ET.parse("empty.xml")
    for loop in legacy.findall("inductionLoop"):
        loop.attrib.pop("lastLeaveComplete", None)
        loop.attrib.pop("netTimeGap", None)
    legacy.write("legacy.xml")
    traci.load(args + ["--load-state", "legacy.xml"])
    traci.simulationStep(9)
    check_gap(-1)

    traci.close()
    traci.start([sumolib.checkBinary("sumo")] + args)
    traci.simulationStep(4)
    leader, = traci.inductionloop.getVehicleData("point")
    assert leader[0] == "leader" and leader[3] == -1, leader
    traci.vehicle.remove("leader")
    traci.simulationStep(9)
    check_gap(-1)
    print("Net time gap: %.2f s" % gap)
finally:
    traci.close()
