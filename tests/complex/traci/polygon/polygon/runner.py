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
polygonID = "0"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((1, 1), (1, 10), (10, 10)), (1, 2, 3, 4), True, "test")
try:
    traci.polygon.add(
        "invalidShape", ((1, 1), (float('nan'), 42), (1, 10), (10, 10)), (1, 2, 3, 4), True, "test")
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())
print("examining", polygonID)
print("shape", traci.polygon.getShape(polygonID))
print("type", traci.polygon.getType(polygonID))
print("color", traci.polygon.getColor(polygonID))
print("filled", traci.polygon.getFilled(polygonID))
traci.polygon.setShape(polygonID, ((11, 11), (11, 101), (101, 101)))
print("shape modified", traci.polygon.getShape(polygonID))
traci.polygon.setType(polygonID, "blub")
print("type modified", traci.polygon.getType(polygonID))
traci.polygon.setColor(polygonID, (5, 6, 7, 8))
print("color modified", traci.polygon.getColor(polygonID))
traci.polygon.setColor(polygonID, (5, 6, 7))
print("color modified2", traci.polygon.getColor(polygonID))
traci.polygon.setFilled(polygonID, False)
print("filled modified", traci.polygon.getFilled(polygonID))

print("getParameter='%s' (unset)" % (traci.polygon.getParameter(polygonID, "foo")))
traci.polygon.setParameter(polygonID, "foo", "42")
print("getParameter='%s' (after setting)" % (traci.polygon.getParameter(polygonID, "foo")))

traci.polygon.subscribe(polygonID)
print(traci.polygon.getSubscriptionResults(polygonID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.polygon.getSubscriptionResults(polygonID))

polygonID2 = "poly2"
traci.polygon.add(
    polygonID2, ((1, 1), (1, 10), (10, 10)), (1, 2, 3, 4), True, "test", lineWidth=3)
print("new polygon lineWidth", traci.polygon.getLineWidth(polygonID2))
traci.polygon.setLineWidth(polygonID2, 0.5)
print("lineWidth modified", traci.polygon.getLineWidth(polygonID2))

traci.close()
