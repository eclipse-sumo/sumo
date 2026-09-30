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
# @author  Angelo Banse
# @date    2026-09-29

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             "--begin", "0",
             "--end", "100",
             "--step-length", "1",
             "--lanechange.duration", "3",
             ] + sys.argv[1:])

veh_id = "veh0"

# Step 1: Advance simulation to depart vehicle
traci.simulationStep()
lane_id = traci.vehicle.getLaneID(veh_id)
shadow_lane = traci.vehicle.getShadowLaneID(veh_id)
print(f"\n[Step {int(traci.simulation.getTime())}] Before lane change:")
print(f"  Primary Lane: '{lane_id}', Shadow Lane: '{shadow_lane}'")
assert shadow_lane == "", f"Expected empty shadow lane before LC, got '{shadow_lane}'"

# Subscribe to VAR_SHADOW_LANE_ID to test subscription support as well
traci.vehicle.subscribe(veh_id, [traci.constants.VAR_LANE_ID, traci.constants.VAR_SHADOW_LANE_ID])

# Initiate lane change from lane 0 to lane 1 with duration 3 seconds
print("\nCommanding lane change from lane 0 to lane 1 (duration: 3s)...")
traci.vehicle.changeLane(veh_id, 1, 3)

# Observe the vehicle across lane changing steps
found_shadow_lane = False
for step in range(3):
    traci.simulationStep()
    t = int(traci.simulation.getTime())
    lane_id = traci.vehicle.getLaneID(veh_id)
    shadow_lane = traci.vehicle.getShadowLaneID(veh_id)

    # Check subscription result
    sub_results = traci.vehicle.getSubscriptionResults(veh_id)
    sub_shadow = sub_results.get(traci.constants.VAR_SHADOW_LANE_ID, "")

    print(f"[Step {t}] Primary Lane: '{lane_id}', Shadow Lane: '{shadow_lane}', Subscribed: '{sub_shadow}'")

    assert shadow_lane == sub_shadow, f"Direct getter ('{shadow_lane}') and subscription ('{sub_shadow}') must match"

    if shadow_lane != "":
        found_shadow_lane = True

assert found_shadow_lane, "Expected a non-empty shadow lane during active lane changing"

# After lane change is completed, shadow lane should be empty again
lane_id = traci.vehicle.getLaneID(veh_id)
shadow_lane = traci.vehicle.getShadowLaneID(veh_id)
print("\nAfter lane change completion:")
print(f"  Primary Lane: '{lane_id}', Shadow Lane: '{shadow_lane}'")
assert shadow_lane == "", f"Expected empty shadow lane after LC completion, got '{shadow_lane}'"

traci.close()
