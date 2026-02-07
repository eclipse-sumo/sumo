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
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2009-11-04

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg"])
traci.simulationStep()
print("Mass default:", traci.vehicletype.getMass("DEFAULT_VEHTYPE"))
print("Mass t1:", traci.vehicletype.getMass("t1"))
print("Mass t2:", traci.vehicletype.getMass("t2"))
print("Mass t3:", traci.vehicletype.getMass("t3"))
traci.vehicletype.setMass("t1", 4321.0)
print("Mass t1:", traci.vehicletype.getMass("t1"))
traci.simulationStep(10)
print("Mass veh0:", traci.vehicle.getMass("veh0"))
print("Mass veh1:", traci.vehicle.getMass("veh1"))
traci.simulationStep(30)
print("Mass veh2:", traci.vehicle.getMass("veh2"))
print("Mass veh3:", traci.vehicle.getMass("veh3"))
traci.simulation.saveState("state.xml")
traci.close()
