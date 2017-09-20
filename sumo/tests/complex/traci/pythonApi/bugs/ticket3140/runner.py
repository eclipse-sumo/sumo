#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Laura Bieker
@date    2017-05-23
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-c', 'sumo.sumocfg',
                                '--fcd-output', 'fcd.xml',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)

traci.init(PORT)
traci.simulationStep()
for i in range(45):
    traci.simulationStep()
traci.vehicle.setSpeedMode("rescue", 7)
traci.vehicle.setSpeed("rescue", 13.9)
traci.trafficlights.setRedYellowGreenState("C", "rrrrrrrrrrrrrrrrrr")
for i in range(20):
    traci.simulationStep()
traci.close()
sumoProcess.wait()
