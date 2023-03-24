#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @date    2023-01-08

from __future__ import absolute_import
from __future__ import print_function
import os
import subprocess
import sys
import time
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


sumoBinary = sumolib.checkBinary("sumo")
os.mkdir("src")
traci.start([sumoBinary, "--no-step-log", "--no-duration-log", "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml", "--save-state.period", "1", "--output-prefix", "src/"])
replay = subprocess.Popen([sys.executable, os.path.join(os.environ["SUMO_HOME"], "tools", "stateReplay.py"),
                           "--src", "src/", "--iterations", "10"])
for _ in range(10):
    traci.simulationStep()
    time.sleep(1)
traci.close()
replay.wait()
