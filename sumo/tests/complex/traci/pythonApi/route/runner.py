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
print("routes", traci.route.getIDList())
print("route count", traci.route.getIDCount())
routeID = "horizontal"
print("examining", routeID)
print("edges", traci.route.getEdges(routeID))
traci.route.subscribe(routeID)
print(traci.route.getSubscriptionResults(routeID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep(step)
    print(traci.route.getSubscriptionResults(routeID))
traci.route.add("h2", ["1o"])
traci.route.add("withTaz", ["taz12-source", "taz34-sink"])
print("routes", traci.route.getIDList())
print("edges", traci.route.getEdges("h2"))
print("edges", traci.route.getEdges("withTaz"))
traci.close()
sumoProcess.wait()
