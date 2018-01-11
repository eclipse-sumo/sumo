#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Leonhard Luecken
# @date    2017-10-16
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..", ".."))
sys.path.append(os.path.join(SUMO_HOME, "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
subprocess.call([sumoBinary, "save_state.sumocfg"])
## LOAD
loadParams = ["-c", "sumo.sumocfg"]
print("Starting...")
print("loadParams: %s"%str(loadParams))
v=traci.start([sumoBinary] + loadParams)
while traci.simulation.getCurrentTime() < 300*1000:
    traci.simulationStep()
print(traci.simulation.getCurrentTime())
traci.close()
