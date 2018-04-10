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
# @author  Jakob Erdmann
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
traci.simulationStep()

# cause an error and catch it
try:
    traci.vehicle.setRoute("horiz", "dummy")
except traci.TraCIException:
    pass

traci.load(["sumo.sumocfg"])
traci.simulationStep()
traci.close()
sumoProcess.wait()


sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -S -Q -c sumo.sumocfg -l log.txt --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for i in range(3):
    traci.simulationStep()
    print("step=%s departed=%s" % (traci.simulation.getCurrentTime(),
                                   traci.simulation.getDepartedIDList()))


print("reloading")
traci.load(["-S", "-Q", "-c", "sumo.sumocfg"])
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    print("step=%s departed=%s" % (traci.simulation.getCurrentTime(),
                                   traci.simulation.getDepartedIDList()))
traci.close()
sumoProcess.wait()
