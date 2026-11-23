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
# @date    2026-11-23

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import sumolib  # noqa
import traci  # noqa


def check_rejection(veh_id, edges, message):
    try:
        traci.vehicle.setRoute(veh_id, edges)
    except traci.TraCIException as error:
        assert message in str(error), str(error)
        return
    raise AssertionError("Unexpectedly accepted route %s" % edges)


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net2.net.xml", "--no-step-log"] + sys.argv[1:])
try:
    veh_id = "ego"
    traci.route.add("initial", ["SC", "CE"])
    traci.vehicle.add(veh_id, "initial")
    traci.vehicle.setRoute(veh_id, ["SC", "CN"])
    assert traci.vehicle.getRoute(veh_id) == ("SC", "CN")

    entered_junction = False
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        edge = traci.vehicle.getRoadID(veh_id)
        if edge.startswith(":") and not entered_junction:
            entered_junction = True
            traci.vehicle.setRoute(veh_id, [edge, "CN", "NC"])
        elif edge == "CN":
            break
    assert entered_junction and edge == "CN"

    traci.vehicle.setRoute(veh_id, ["CN"])
    expected_err = "Replacement route starts on edge 'SC' instead of the current edge 'CN'."
    check_rejection(veh_id, ["SC", "CN", "NC"], expected_err)
    check_rejection(veh_id, ["CN", "CE"], "No connection between edge 'CN' and edge 'CE'.")
    assert traci.vehicle.getRoute(veh_id) == ("SC", "CN")
    traci.vehicle.setRoute(veh_id, ["CN", "NC"])
    assert traci.vehicle.getRoute(veh_id) == ("SC", "CN", "NC")
finally:
    traci.close()
