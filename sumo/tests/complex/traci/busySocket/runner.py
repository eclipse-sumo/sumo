#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2011-03-02
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
from __future__ import print_function

import os
import subprocess
import sys
import time
import shutil

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib
import traci

if sys.argv[1] == "sumo":
    sumoBinary = os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = []
    secondConfig = "sumo.sumocfg"
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = ["-S", "-Q"]
    secondConfig = "sumo_log.sumocfg"
PORT = sumolib.miscutils.getFreeSocketPort()

sumoProc = subprocess.Popen(
    [sumoBinary, "-c", "sumo.sumocfg", "--remote-port", str(PORT)] + addOption)
traci.init(PORT)
subprocess.call([sumoBinary, "-c", secondConfig,
                 "--remote-port", str(PORT)] + addOption)
step = 0
while not step > 100:
    traci.simulationStep()
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz") < 0 or len(vehs) > 1:
        print("Something is false")
    step += 1
traci.close()
sumoProc.wait()
sys.stdout.flush()
if os.path.exists("lastrun.stderr"):
    f = open("lastrun.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
