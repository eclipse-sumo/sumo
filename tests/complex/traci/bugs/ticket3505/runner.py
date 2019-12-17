#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Leonhard Luecken
# @date    2017-10-16

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

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
