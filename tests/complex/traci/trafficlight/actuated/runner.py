#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

tlsID = "C"
traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])

logics = traci.trafficlight.getAllProgramLogics(tlsID)
programID = traci.trafficlight.getProgram(tlsID)
logic = None
for cand in logics:
    if cand.programID == programID:
        logic = cand

assert(logic)
numPhases = len(logic.phases)
print("current program '%s' has %s phases" % (programID, numPhases))


def check():
    print("%s phase=%s" % (
        traci.simulation.getTime(),
        traci.trafficlight.getPhase(tlsID)))


# default actuated
print("max-gap:", traci.trafficlight.getParameter(tlsID, "max-gap"))
# make the first actuated phase 2 seconds longer
traci.trafficlight.setPhaseDuration(tlsID, 7)
traci.trafficlight.setPhase(tlsID, 0)
for i in range(180):
    check()
    if i == 50:
        phase = traci.trafficlight.getPhase(tlsID)
        newPhase = (phase + 3) % numPhases
        print("jumping from phase %s to %s" % (phase, newPhase))
        traci.trafficlight.setPhase(tlsID, newPhase)
        traci.trafficlight.setPhaseDuration(tlsID, 20)
    traci.simulationStep()

# change parameter
traci.trafficlight.setParameter(tlsID, "max-gap", "10")
print("max-gap:", traci.trafficlight.getParameter(tlsID, "max-gap"))
prolonged = False
for i in range(180):
    if not prolonged and traci.trafficlight.getPhase(tlsID) == 0:
        # make the first actuated phase shorter once
        traci.trafficlight.setPhaseDuration(tlsID, 30)
        prolonged = True
    traci.simulationStep()

print("cycleSecond (before):", traci.trafficlight.getParameter(tlsID, "cycleSecond"))
for key, value in [
        ("cycleTime", "55"),
        ("coordinated", "true"),
        ("offset", "12")]:
    print("%s (before):" % key, traci.trafficlight.getParameter(tlsID, key))
    traci.trafficlight.setParameter(tlsID, key, value)
    print("%s (after):" % key, traci.trafficlight.getParameter(tlsID, key))
print("cycleSecond (after):", traci.trafficlight.getParameter(tlsID, "cycleSecond"))

traci.close()
