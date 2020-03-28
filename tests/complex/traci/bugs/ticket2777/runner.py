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
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa


def check(vehID, steps=1):
    for i in range(steps):
        if i > 0:
            traci.simulationStep()
        try:
            print("%s vehicle %s on lane=%s pos=%s speed=%s" % (
                traci.simulation.getTime(),
                vehID,
                traci.vehicle.getLaneID(vehID),
                traci.vehicle.getLanePosition(vehID),
                traci.vehicle.getSpeed(vehID)))
        except traci.TraCIException as e:
            if traci.isLibsumo():
                print(e, file=sys.stderr)
                sys.stderr.flush()
    if steps > 1:
        print()


vehID = "v0"
traci.start([sumolib.checkBinary("sumo"), '-c', 'sumo.sumocfg', '-S', '-Q'])
traci.simulationStep()
check(vehID)
try:
    print("%s setStop for %s" % (traci.simulation.getTime(), vehID))
    traci.vehicle.setStop(vehID, "beg", pos=1.0, laneIndex=0, duration=5)
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
        sys.stderr.flush()
check(vehID, 10)

traci.simulationStep(21)
vehID = "v1"
check(vehID)
try:
    print("%s setStop for %s" % (traci.simulation.getTime(), vehID))
    traci.vehicle.setStop(vehID, "end", pos=1.0, laneIndex=0, duration=5)
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
        sys.stderr.flush()
check(vehID, 10)

traci.simulationStep(41)
vehID = "v2"
check(vehID)
try:
    print("%s setStop for %s" % (traci.simulation.getTime(), vehID))
    traci.vehicle.setStop(vehID, "middle", pos=1.0, laneIndex=0, duration=5)
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
check(vehID, 10)

traci.simulationStep(61)
vehID = "v3"
check(vehID)
try:
    print("%s setStop for %s" % (traci.simulation.getTime(), vehID))
    traci.vehicle.setStop(vehID, "middle", pos=1.0, laneIndex=0, duration=5)
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
check(vehID, 10)

traci.close()
