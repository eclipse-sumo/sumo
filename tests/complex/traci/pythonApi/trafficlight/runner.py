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
print("trafficlights deprecated", traci.trafficlights.getIDList())
print("trafficlight", traci.trafficlight.getIDList())
print("trafficlight count", traci.trafficlight.getIDCount())
tlsID = "0"


def check():
    print("examining", tlsID)
    print("ryg", traci.trafficlight.getRedYellowGreenState(tlsID))
    print("rygdef", traci.trafficlight.getCompleteRedYellowGreenDefinition(tlsID))
    print("lanes", traci.trafficlight.getControlledLanes(tlsID))
    print("links", traci.trafficlight.getControlledLinks(tlsID))
    print("program", traci.trafficlight.getProgram(tlsID))
    print("phase", traci.trafficlight.getPhase(tlsID))
    print("switch", traci.trafficlight.getNextSwitch(tlsID))

phases = []
phases.append(traci.trafficlight.Phase(30, 0, 0, "rrrrGGggrrrrGGgg"))
phases.append(traci.trafficlight.Phase(10, 0, 0, "rrrrGGggrrrrGGgg"))
phases.append(traci.trafficlight.Phase(40, 0, 0, "rrrrGGggrrrrGGgg"))
phases.append(traci.trafficlight.Phase(20, 0, 0, "rrrrGGggrrrrGGgg"))
phases.append(traci.trafficlight.Phase(20, 0, 0, "rrrrGGggrrrrGGgg"))
phases.append(traci.trafficlight.Phase(20, 0, 0, "rrrrGGggrrrrGGgg"))
logic = traci.trafficlight.Logic("custom", 0, 0, 0, phases)
traci.trafficlight.setCompleteRedYellowGreenDefinition(tlsID, logic)

traci.trafficlight.setPhase(tlsID, 4)
traci.trafficlight.setPhaseDuration(tlsID, 23)
check()
defs = traci.trafficlight.getCompleteRedYellowGreenDefinition(tlsID)
print("numDefs=%s numPhases=%s" % (len(defs), map(lambda d : len(d.getPhases()), defs)))
traci.trafficlight.subscribe(tlsID)
print(traci.trafficlight.getSubscriptionResults(tlsID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.trafficlight.getSubscriptionResults(tlsID))
traci.trafficlight.setLinkState(tlsID, 4, 'u')
try:
    traci.trafficlight.setLinkState(tlsID, 16, 'u')
except traci.exceptions.TraCIException as e:
    print("caught", e)
check()
traci.close()
sumoProcess.wait()
