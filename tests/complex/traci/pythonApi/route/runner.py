#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


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
import sumolib  # noqa

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
    # XXX: This stepping is a bit strange (intentional?)
    traci.simulationStep(step)
    print(traci.route.getSubscriptionResults(routeID))
traci.route.add("h2", ["1o"])
traci.route.add("withTaz", ["taz12-source", "taz34-sink"])
print("routes", traci.route.getIDList())
print("edges", traci.route.getEdges("h2"))
print("edges", traci.route.getEdges("withTaz"))
traci.close()
sumoProcess.wait()
