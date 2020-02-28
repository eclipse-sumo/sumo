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


def examine(polygonID):
    print("# examining", polygonID)
    print("shape", traci.polygon.getShape(polygonID))
    print("type", traci.polygon.getType(polygonID))
    print("color", traci.polygon.getColor(polygonID))
    print("filled", traci.polygon.getFilled(polygonID))


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
# traci.init(port=12345) # debug
for step in range(3):
    print("step", step)
    traci.simulationStep()
polygonID = "poly0"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((960, 510), (960, 550), (1000, 550), (1000, 510)), (100, 200, 0, 255), True, "test")

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())
examine(polygonID)

for step in range(3, 6):
    print("step", step)
    traci.simulationStep()

# Failing specification tests
print("# (1) Adding underspecified dynamics...")
try:
    traci.polygon.addDynamics(polygonID)
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

print("# (2) Adding malformed dynamics 1 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [0, 1, 2, 4, 3])
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

print("# (3) Adding malformed dynamics 2 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [1, 2, 3, 4], [200, 20, 2, 1])
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

print("# (4) Adding malformed dynamics 3 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [0, 1, 2, 3], [200, 20, 2])
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

print("# (5) Adding malformed dynamics 4 ...")
try:
    traci.polygon.addDynamics(polygonID, "", [0], [200])
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

print("# (6) Adding malformed dynamics 5 ...")
try:
    traci.polygon.addDynamics(polygonID, "horiz", [], [], True)
except traci.exceptions.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
    print("Caught TraCIException")
    pass

examine(polygonID)

for step in range(6, 9):
    print("step", step)
    traci.simulationStep()

# Successful specifications

print("# (1) No tracking with alpha animation")
traci.polygon.addDynamics(polygonID, "", [0, 1, 2, 5], [0, 200, 100, 0])

for step in range(9, 12):
    print("step", step)
    traci.simulationStep()
    examine(polygonID)

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

print("vehicles", traci.vehicle.getIDList())
print("vehicle count", traci.vehicle.getIDCount())

for step in range(12, 15):
    print("step", step)
    traci.simulationStep()

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

polygonID = "poly1"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((960, 510), (960, 550), (1000, 550), (1000, 510)), (0, 100, 100, 255), True, "test")

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

print("# (2) No tracking with time line but no animation (removal after time line expired)")
traci.polygon.addDynamics(polygonID, "", [0, 1, 2, 3])

for step in range(15, 18):
    print("step", step)
    traci.simulationStep()

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

polygonID = "poly2"
print("adding", polygonID)
traci.polygon.add(
    polygonID, ((960, 510), (960, 550), (1000, 550), (1000, 510)), (150, 0, 100, 255), True, "test")

vehID = "veh0"
print("adding", vehID)
traci.route.add("trip0", ["2fi", "1o"])
traci.vehicle.add(vehID, "trip0")

print("vehicles", traci.vehicle.getIDList())
print("vehicle count", traci.vehicle.getIDCount())

print("# (3) Tracking without time line")
traci.polygon.addDynamics(polygonID, vehID)

for step in range(18, 21):
    print("step", step)
    traci.simulationStep()

print("# (4) Tracking existing vehicle with time line")
polygonID = "poly3"
vehID = "horiz"
traci.polygon.add(
    polygonID, ((750, 490), (750, 450), (790, 450), (790, 490)), (0, 0, 200, 255), True, "test")
traci.polygon.addDynamics(polygonID, vehID, [0, 10])

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for step in range(21, 25):
    print("step", step)
    traci.simulationStep()

print("# (5) replacing tracking dynamics and highlighting vehicle permanently")
polygonID = "poly2"
vehID = "veh0"
traci.polygon.addDynamics(polygonID, vehID, [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20], [
                          50, 200, 50, 200, 50, 200, 50, 200, 50, 200, 50])
traci.vehicle.highlight(vehID)

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for step in range(25, 35):
    print("step", step)
    traci.simulationStep()

# "poly3" should have been removed
print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

print("# (6) Tracking with alpha animation")
vehID = "horiz"
polygonID = "poly4"
traci.polygon.add(
    polygonID, ((550, 510), (550, 550), (590, 550), (590, 510)), (20, 20, 200, 255), True, "test")
traci.polygon.addDynamics(polygonID, vehID, [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20], [
                          50, 200, 50, 200, 50, 200, 50, 200, 50, 200, 50])

for step in range(35, 40):
    print("step", step)
    traci.simulationStep()

print("# (7) Add a second tracking polygon")
vehID = "horiz"
polygonID = "poly5"
traci.polygon.add(
    polygonID, ((350, 490), (350, 450), (390, 450), (390, 490)), (200, 20, 20, 255), True, "test")
traci.polygon.addDynamics(polygonID, vehID)

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for step in range(40, 45):
    print("step", step)
    traci.simulationStep()

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

print("# (8) Parking vehicle and renew dynamics and highlight")
vehID = "veh0"
polygonID = "poly2"
traci.vehicle.setStop(vehID, "2si", 100, 1, 5, 1)
traci.polygon.addDynamics(polygonID, vehID, [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20], [
                          50, 200, 50, 200, 50, 200, 50, 200, 50, 200, 50])
traci.vehicle.highlight(vehID, (0, 100, 100, 100))

for step in range(45, 65):
    print("step", step)
    traci.simulationStep()

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for step in range(65, 110):
    print("step", step)
    traci.simulationStep()

print("# (9) transfer tracking dynamics from vehicle 'horiz' to 'veh0' and redefine as looped animation")
vehID = "veh0"
polygonID = "poly5"
traci.polygon.addDynamics(polygonID, vehID, [0, 2, 4], [50, 200, 50], True)

print("# (10) Highlight vehicle 'horiz' for limited duration")
vehID = "horiz"
traci.vehicle.highlight(vehID, (0, 255, 0), -1, 200, 4)

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for pID in traci.polygon.getIDList():
    examine(pID)

print("vehicles", traci.vehicle.getIDList())
print("vehicle count", traci.vehicle.getIDCount())

for step in range(110, 120):
    print("step", step)
    traci.simulationStep()

print("# (11) Tracked vehicle 'veh0' arrives")
for step in range(120, 125):
    print("step", step)
    traci.simulationStep()

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

print("vehicles", traci.vehicle.getIDList())
print("vehicle count", traci.vehicle.getIDCount())

print("# (12) Adding and highlighting POIs (two highlights for the same must specify highlight type)")

print("pois", traci.poi.getIDList())
print("poi count", traci.poi.getIDCount())

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

poiID = "myPOI0"
print("adding", poiID)
traci.poi.add(poiID, 350, 510, (255, 255, 255, 255), "test", 0, "img.jpeg")
traci.poi.highlight(poiID, (0, 0, 255), -1, 200, 4)

poiID = "myPOI1"
print("adding", poiID)
traci.poi.add(poiID, 370, 510, (255, 255, 255, 255), "test", 0, "img.jpeg")
traci.poi.highlight(poiID, (0, 0, 255), -1, 200, 4)

poiID = "myPOI0"
traci.poi.highlight(poiID, (55, 55, 100), 20, 200, 4, 1)

print("pois", traci.poi.getIDList())
print("poi count", traci.poi.getIDCount())

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for pID in traci.polygon.getIDList():
    examine(pID)

for step in range(125, 130):
    print("step", step)
    traci.simulationStep()

print("pois", traci.poi.getIDList())
print("poi count", traci.poi.getIDCount())

print("polygons", traci.polygon.getIDList())
print("polygon count", traci.polygon.getIDCount())

for pID in traci.polygon.getIDList():
    examine(pID)

traci.close()
