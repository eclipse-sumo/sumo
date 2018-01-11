#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-07-01
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import shutil

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci

PORT = sumolib.miscutils.getFreeSocketPort()

if sys.argv[1] == "sumo":
    sumoBinary = os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = []
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = ["-S", "-Q"]

sumoProcess = subprocess.Popen(
    [sumoBinary, "-c", "sumo.sumocfg", "--remote-port", str(PORT)] + addOption)
traci.init(PORT)
time.sleep(10)
step = 0
while not step > 100:
    traci.simulationStep()
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz") < 0 or len(vehs) > 1:
        print("Something is false")
    step += 1
traci.close()
sumoProcess.wait()
sys.stdout.flush()
