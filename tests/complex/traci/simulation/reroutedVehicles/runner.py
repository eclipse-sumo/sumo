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

import os
import subprocess
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def check_reroutes(expected, subscribed=False):
    simulation = traci.simulation
    variables = traci.constants
    assert tuple(simulation.getReroutedVehicleIDList()) == expected
    assert simulation.getReroutedVehicleNumber() == len(expected)
    if subscribed:
        results = simulation.getSubscriptionResults()
        assert tuple(results[variables.VAR_REROUTED_VEHICLES_IDS]) == expected
        assert results[variables.VAR_REROUTED_VEHICLES_NUMBER] == len(expected)


subprocess.check_call([
    sumolib.checkBinary("netconvert"),
    "-n", "input_nodes.nod.xml", "-e", "input_edges.edg.xml",
    "-o", "input_net.net.xml",
], stdout=subprocess.DEVNULL)
traci.start([
    sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
    "--no-step-log", "--no-duration-log",
] + sys.argv[1:])
try:
    check_reroutes(())
    traci.simulation.subscribe((
        traci.constants.VAR_REROUTED_VEHICLES_IDS,
        traci.constants.VAR_REROUTED_VEHICLES_NUMBER,
    ))
    traci.simulationStep()
    check_reroutes((), subscribed=True)

    traci.vehicle.setRoute("manual", ("start", "lower", "lowerEnd", "end"))
    check_reroutes(("manual",))
    traci.vehicle.setRoute("manual", ("start", "upper", "upperEnd", "end"))
    check_reroutes(("manual", "manual"))

    traci.simulationStep(4)
    check_reroutes(("automatic",), subscribed=True)
    traci.simulationStep()
    check_reroutes((), subscribed=True)
finally:
    traci.close()
