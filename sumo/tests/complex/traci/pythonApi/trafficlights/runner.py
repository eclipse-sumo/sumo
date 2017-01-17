#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import shutil
import struct
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("trafficlights", traci.trafficlights.getIDList())
print("trafficlights count", traci.trafficlights.getIDCount())
tlsID = "0"
print("examining", tlsID)
print("ryg", traci.trafficlights.getRedYellowGreenState(tlsID))
print("rygdef", traci.trafficlights.getCompleteRedYellowGreenDefinition(tlsID))
print("lanes", traci.trafficlights.getControlledLanes(tlsID))
print("links", traci.trafficlights.getControlledLinks(tlsID))
print("program", traci.trafficlights.getProgram(tlsID))
print("phase", traci.trafficlights.getPhase(tlsID))
print("switch", traci.trafficlights.getNextSwitch(tlsID))
traci.trafficlights.subscribe(tlsID)
print(traci.trafficlights.getSubscriptionResults(tlsID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.trafficlights.getSubscriptionResults(tlsID))
traci.close()
sumoProcess.wait()
