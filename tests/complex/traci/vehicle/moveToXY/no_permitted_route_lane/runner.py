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
# @author  Mark Colley
# @date    2026-11-05


"""Route mapping must reject incompatible lanes without terminating SUMO."""

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402

traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"])
try:
    traci.route.add("route", ["edge"])
    traci.vehicle.add("ego", "route")
    traci.simulationStep()
    x, y = traci.vehicle.getPosition("ego")
    traci.lane.setAllowed("edge_0", ["bicycle"])
    try:
        traci.vehicle.moveToXY("ego", "edge", 0, x, y, keepRoute=1)
    except traci.TraCIException:
        print("Mapping without a permitted route lane rejected")
    else:
        raise AssertionError("Mapping succeeded without a permitted lane")
    assert traci.vehicle.getLaneID("ego") == "edge_0"
    traci.vehicle.moveToXY("ego", "edge", 0, x, y, keepRoute=5)
    assert traci.vehicle.getLaneID("ego") == "edge_0"
    print("Connection remains usable and permission override succeeds")
finally:
    traci.close()
