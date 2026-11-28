#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-24

import os
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def check_crossing(route_file, overlap):
    traci.start([sumolib.checkBinary("sumo"), "-n", "generated.net.xml", "-r", route_file,
                 "--no-step-log", "--time-to-teleport", "-1"])
    try:
        traci.trafficlight.setRedYellowGreenState("j", "Gr")
        while traci.simulation.getTime() < 30:
            traci.simulationStep()
            if "crossing" in traci.vehicle.getIDList() and traci.vehicle.isStopped("crossing"):
                break
        else:
            raise AssertionError("The crossing vehicle did not reach its stop.")

        traci.trafficlight.setRedYellowGreenState("j", "rG")
        crossed = False
        while traci.simulation.getTime() < 80:
            traci.simulationStep()
            assert traci.simulation.getCollidingVehiclesNumber() == 0
            if traci.simulation.getTime() == 50 and overlap:
                assert traci.vehicle.getLaneID("waiting") == ":j_1_0"
                assert traci.vehicle.getSpeed("waiting") == 0
                assert traci.vehicle.isStopped("crossing")
                traci.vehicle.resume("crossing")
            if "waiting" in traci.vehicle.getIDList() and traci.vehicle.getLaneID("waiting") == "east_0":
                assert not overlap or traci.simulation.getTime() > 50
                assert overlap or traci.vehicle.isStopped("crossing")
                crossed = True
                break
        assert crossed, "The waiting vehicle did not cross."
    finally:
        traci.close()


# Split the south-to-north link beyond the crossing so a stopped rear can occupy its continuation.
subprocess.run([sumolib.checkBinary("netconvert"), "-n", "input_nodes.nod.xml",
                "-e", "input_edges.edg.xml", "-x", "input_connections.con.xml",
                "--no-turnarounds", "-o", "generated.net.xml"],
               check=True, stdout=subprocess.DEVNULL)
check_crossing("input_routes.rou.xml", overlap=False)
print("The waiting vehicle crossed while the other vehicle remained stopped.")
check_crossing("input_routes2.rou.xml", overlap=True)
print("The waiting vehicle crossed after the other vehicle moved clear.")
