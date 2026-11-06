#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
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

"""Departure rerouting must preserve the occupied edge and destination TAZ."""

import os
import sys

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci


def check_case(name, change_time, release_time, pre_period, expected_start, expected_end):
    traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml",
                 "-r", "input_routes.rou.xml", "-a", "input_additional.add.xml",
                 "--device.rerouting.probability", "1", "--device.rerouting.period", "0",
                 "--device.rerouting.pre-period", str(pre_period), "--device.rerouting.with-taz", "true",
                 "--device.rerouting.adaptation-interval", "1", "--time-to-teleport", "-1",
                 "--no-step-log", "--duration-log.disable", "true"])
    try:
        traci.simulationStep()
        traci.vehicle.setSpeed("blocker", 0)
        departed = False
        arrived = False
        for step in range(2, 100):
            if step == change_time:
                assert traci.vehicle.getRoute("probe") == ("a", "d1")
            if change_time <= step < release_time:
                # These are the shared weights used by the rerouting device.
                for edge, cost in (("a", 100), ("b", 1), ("d1", 100), ("d2", 1)):
                    traci.vehicle.setParameter("blocker", "device.rerouting.edge:" + edge, str(cost))
            if step == release_time:
                traci.vehicle.remove("blocker")
            traci.simulationStep()
            assert traci.simulation.getStartingTeleportNumber() == 0
            assert traci.simulation.getCollidingVehiclesNumber() == 0
            if "probe" in traci.simulation.getArrivedIDList():
                arrived = True
                break
            if "probe" in traci.vehicle.getIDList():
                road = traci.vehicle.getRoadID("probe")
                route = traci.vehicle.getRoute("probe")
                index = traci.vehicle.getRouteIndex("probe")
                assert route[0] == expected_start, (name, road, route)
                assert route[-1] == expected_end, (name, road, route)
                assert road == route[index], (name, road, route, index)
                departed = True
        assert departed and arrived, name
        print(name, "passed")
    finally:
        traci.close()


check_case("delayed departure", 20, 30, 10, "a", "d2")
check_case("pre-insertion origin selection", 5, 30, 10, "b", "d2")
check_case("disabled pre-insertion rerouting", 20, 30, 0, "a", "d1")
