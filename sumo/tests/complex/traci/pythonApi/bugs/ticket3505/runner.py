#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    runner.py
# @author  Leonhard Luecken
# @date    2017-10-16
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import traci

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin', 'sumo'))
            
## LOAD
loadParams = ["-c", "sumo.sumocfg"]
print("Starting...")
print("loadParams: %s"%str(loadParams))
v=traci.start([sumoBinary] + loadParams)
print("Version%s"%v)
traci.simulationStep()
traci.close()
sumoProcess.wait()
