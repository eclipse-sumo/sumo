#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @author  Laura Bieker
# @date    2013-10-07

from __future__ import absolute_import

import os
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
sumoBinary = sumolib.checkBinary(sys.argv[1])

if sys.argv[1] == "sumo":
    addOption = ""
else:
    addOption = "-S -Q"
PORT = sumolib.miscutils.getFreeSocketPort()


def run():
    """execute the TraCI control loop"""
    traci.init(PORT)
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0 and step < 100:
        traci.simulationStep()
        step += 1
        if step == 4:
            traci.trafficlight.setProgram("center", "0")
    traci.close()
    sys.stdout.flush()


sumoProcess = subprocess.Popen([sumoBinary,
                                "-n", "input_net.net.xml",
                                "-r", "input_routes.rou.xml",
                                "-a", "input_additional.add.xml",
                                "--no-step-log",
                                "--remote-port", str(PORT)],
                               stdout=sys.stdout, stderr=sys.stderr)
run()
sumoProcess.wait()
