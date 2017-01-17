#!/usr/bin/env python
"""
@file    runner.py
@author  Jakob Erdmann
@author  Laura Bieker
@date    2013-10-07
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import

import os
import subprocess
import sys
import time
import math

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib
import traci

if sys.argv[1] == "sumo":
    sumoBinary = os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = ""
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
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
            traci.trafficlights.setProgram("center", "0")
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
