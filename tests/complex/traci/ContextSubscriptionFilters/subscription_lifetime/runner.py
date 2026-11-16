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


"""Filter ownership survives unrelated removals and ends with its subscription."""

import os
import sys
import tempfile

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402
from traci import constants as tc  # noqa: E402


def expect_no_context():
    try:
        traci.vehicle.addSubscriptionFilterVType(["DEFAULT_VEHTYPE"])
    except traci.TraCIException:
        return
    raise AssertionError("Filter update accepted without a context subscription")


def check_live_context():
    traci.vehicle.addSubscriptionFilterVType(["missing_type"])
    traci.simulationStep()
    assert traci.vehicle.getContextSubscriptionResults("ego") == {}
    traci.vehicle.addSubscriptionFilterVType(["DEFAULT_VEHTYPE"])
    traci.simulationStep()
    assert set(traci.vehicle.getContextSubscriptionResults("ego")) == {"ego"}


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"])
try:
    traci.route.add("route", ["edge"])
    traci.vehicle.add("ego", "route", departPos="20")
    traci.simulationStep()
    traci.vehicle.setSpeed("ego", 0)

    traci.vehicle.subscribe("ego", [tc.VAR_SPEED])
    traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])
    traci.vehicle.unsubscribe("ego")
    if traci.isLibsumo():
        # libsumo clears the filter target on every explicit unsubscribe.
        traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])
    check_live_context()
    print("Removing an earlier subscription preserves filter ownership")

    traci.vehicle.unsubscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100)
    expect_no_context()
    print("Removing the final context subscription rejects further filters")

    traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED],
                                   end=traci.simulation.getTime())
    traci.simulationStep()
    expect_no_context()
    print("Expired context subscriptions reject further filters")

    traci.vehicle.subscribe("ego", [tc.VAR_SPEED], end=traci.simulation.getTime())
    traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])
    traci.simulationStep()
    check_live_context()
    print("Expiring an earlier subscription preserves filter ownership")

    with tempfile.TemporaryDirectory() as state_dir:
        state_path = os.path.join(state_dir, "state.xml")
        traci.simulation.saveState(state_path)
        traci.simulation.loadState(state_path)
    expect_no_context()
    print("Loading state removes context filter ownership")

    traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])
    traci.vehicle.setSpeed("ego", 20)
    for _ in range(30):
        traci.simulationStep()
        if "ego" not in traci.vehicle.getIDList():
            break
    else:
        raise AssertionError("The subscribed vehicle did not arrive")
    expect_no_context()
    print("Arrival removes context filter ownership")

    traci.vehicle.add("ego", "route", departPos="20")
    traci.simulationStep()
    traci.vehicle.subscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 100, [tc.VAR_SPEED])
    traci.load(["-n", "input_net.net.xml", "--no-step-log"])
    expect_no_context()
    print("Reloading the simulation removes context filter ownership")
finally:
    traci.close()
