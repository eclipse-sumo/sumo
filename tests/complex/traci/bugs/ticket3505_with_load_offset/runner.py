#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @author  Leonhard Luecken
# @date    2017-10-16

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
subprocess.call([sumoBinary, "save_state.sumocfg"])
# LOAD
loadParams = ["-c", "sumo.sumocfg"]
print("Starting...")
print("loadParams: %s" % str(loadParams))
v = traci.start([sumoBinary] + loadParams)
while traci.simulation.getTime() < 300:
    traci.simulationStep()
print(traci.simulation.getTime())
traci.close()
