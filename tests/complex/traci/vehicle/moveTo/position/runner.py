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

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import sumolib  # noqa
import traci  # noqa

traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml", "--no-step-log"] + sys.argv[1:])
try:
    traci.simulationStep()
    print("Initial position:", traci.vehicle.getPosition("ego"))
    for lane_pos in (50., 30., 75.):
        traci.vehicle.moveTo("ego", "1fi_0", lane_pos)
        # Read both Cartesian positions before advancing the simulation.
        print("Lane position:", traci.vehicle.getLanePosition("ego"),
              "position:", traci.vehicle.getPosition("ego"),
              "position3D:", traci.vehicle.getPosition3D("ego"))
finally:
    traci.close()
