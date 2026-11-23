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

import math
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def check_position(person, x, y):
    actual_x, actual_y = traci.person.getPosition(person)
    assert math.isclose(actual_x, x, abs_tol=1e-6) and math.isclose(actual_y, y, abs_tol=1e-6), (
        (actual_x, actual_y), (x, y))


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net2.net.xml", "--no-step-log"] + sys.argv[1:])
try:
    traci.person.add("p", "SC", 40)
    traci.person.appendWalkingStage("p", ["SC", "CN"], 50)
    traci.simulationStep()
    lane = traci.person.getLaneID("p")
    (x, y), _ = traci.lane.getShape(lane)

    traci.person.moveToXY("p", "SC", x, y - 2, keepRoute=1)
    traci.simulationStep()
    assert traci.person.getLaneID("p") == lane
    check_position("p", x, y)

    traci.person.moveToXY("p", "SC", x + 0.5, y + 10, keepRoute=1)
    traci.simulationStep()
    check_position("p", x + 0.5, y + 10)

    print("The lane endpoint adds no lateral offset.")
finally:
    traci.close()
