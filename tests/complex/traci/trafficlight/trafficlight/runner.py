#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
for step in range(3):
    print("step", step)
    traci.simulationStep()
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
    print("phaseName", traci.trafficlight.getPhaseName(tlsID))
    print("switch", traci.trafficlight.getNextSwitch(tlsID))


phases = []
phases.append(traci.trafficlight.Phase(30, "rrrrGGggrrrrGGgg", 0, 0, [1, 2, 3], "setViaComplete"))
phases.append(traci.trafficlight.Phase(10, "rrrrGGggrrrrGGgg", 0, 0))
phases.append(traci.trafficlight.Phase(40, "rrrrGGggrrrrGGgg", 0, 0))
phases.append(traci.trafficlight.Phase(20, "rrrrGGggrrrrGGgg", 0, 0))
phases.append(traci.trafficlight.Phase(20, "rrrrGGggrrrrGGgg", 0, 0))
phases.append(traci.trafficlight.Phase(20, "rrrrGGggrrrrGGgg", 0, 0))
logic = traci.trafficlight.Logic("custom", 0, 0, phases)
traci.trafficlight.setCompleteRedYellowGreenDefinition(tlsID, logic)

traci.trafficlight.setPhase(tlsID, 4)
traci.trafficlight.setPhaseName(tlsID, "setByTraCI")
traci.trafficlight.setPhaseDuration(tlsID, 23)
print("waitingPersons", traci.trafficlight.getServedPersonCount(tlsID, 2))
check()
defs = traci.trafficlight.getCompleteRedYellowGreenDefinition(tlsID)
print("numDefs=%s numPhases=%s" % (len(defs), list(map(lambda d: len(d.getPhases()), defs))))
traci.trafficlight.subscribe(tlsID)
print(traci.trafficlight.getSubscriptionResults(tlsID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.trafficlight.getSubscriptionResults(tlsID))
traci.trafficlight.setLinkState(tlsID, 4, 'u')
try:
    traci.trafficlight.setLinkState(tlsID, 16, 'u')
except traci.TraCIException as e:
    print("caught", e)
check()
traci.trafficlight.setRedYellowGreenState(tlsID, "gGyruOorrrrrrrrr")
print("set ryg", traci.trafficlight.getRedYellowGreenState(tlsID))
print("program", traci.trafficlight.getProgram(tlsID))
traci.trafficlight.setProgram(tlsID, "off")
print("ryg", traci.trafficlight.getRedYellowGreenState(tlsID))

traci.close()
