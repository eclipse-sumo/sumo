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
import shutil
import struct
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib  # noqa

def checkVehicleStates():
    print("time", traci.simulation.getCurrentTime())
    print("#loaded", traci.simulation.getLoadedNumber())
    print("loaded", traci.simulation.getLoadedIDList())
    print("#departed", traci.simulation.getDepartedNumber())
    print("departed", traci.simulation.getDepartedIDList())
    print("#arrived", traci.simulation.getArrivedNumber())
    print("arrived", traci.simulation.getArrivedIDList())
    print("#parkstart", traci.simulation.getParkingStartingVehiclesNumber())
    print("parkstart", traci.simulation.getParkingStartingVehiclesIDList())
    print("#parkend", traci.simulation.getParkingEndingVehiclesNumber())
    print("parkend", traci.simulation.getParkingEndingVehiclesIDList())
    print("#stopstart", traci.simulation.getStopStartingVehiclesNumber())
    print("stopstart", traci.simulation.getStopStartingVehiclesIDList())
    print("#stopend", traci.simulation.getStopEndingVehiclesNumber())
    print("stopend", traci.simulation.getStopEndingVehiclesIDList())
    print("#colliding", traci.simulation.getCollidingVehiclesNumber())
    print("colliding", traci.simulation.getCollidingVehiclesIDList())
    print("min#expected", traci.simulation.getMinExpectedNumber())
    print("#teleportStart", traci.simulation.getStartingTeleportNumber())
    print("teleportStart", traci.simulation.getStartingTeleportIDList())
    print("#teleportEnd", traci.simulation.getEndingTeleportNumber())
    print("teleportEnd", traci.simulation.getEndingTeleportIDList())

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
traci.simulation.subscribe(
    (traci.constants.VAR_LOADED_VEHICLES_IDS, traci.constants.VAR_DEPARTED_VEHICLES_IDS))
print(traci.simulation.getSubscriptionResults())
for step in range(6):
    print("step", step)
    traci.simulationStep()
    print(traci.simulation.getSubscriptionResults())
checkVehicleStates()
print("deltaT", traci.simulation.getDeltaT())
print("boundary", traci.simulation.getNetBoundary())
print("convertRoad2D", traci.simulation.convert2D("o", 0.))
print("convertRoad3D", traci.simulation.convert3D("o", 0.))
print("convertRoadGeo", traci.simulation.convert2D("o", 0., toGeo=True))
print("convertRoadGeoAlt", traci.simulation.convert3D("o", 0., toGeo=True))
print("convert2DGeo", traci.simulation.convertGeo(488.65, 501.65))
print("convertGeo2D", traci.simulation.convertGeo(12, 48, True))
print("convert2DRoad", traci.simulation.convertRoad(488.65, 501.65))
print("convertGeoRoad", traci.simulation.convertRoad(12, 48.1, True))
print("distance2D", traci.simulation.getDistance2D(
    488.65, 501.65, 498.65, 501.65))
print("drivingDistance2D", traci.simulation.getDistance2D(
    488.65, 501.65, 498.65, 501.65, isDriving=True))
print("distanceRoad", traci.simulation.getDistanceRoad("o", 0., "2o", 0.))
print("drivingDistanceRoad", traci.simulation.getDistanceRoad(
    "o", 0., "2o", 0., isDriving=True))
print("clearing pending")
traci.simulation.clearPending()
print("save simstate")
traci.simulation.saveState("state.xml")
try:
    print("getParameter", traci.simulation.getParameter("foo", "foo.bla"))
except traci.TraCIException:
    pass
try:
    print("getParameter", traci.simulation.getParameter("cs1", "chargingStation.bla"))
except traci.TraCIException:
    pass
try:
    print("getParameter", traci.simulation.getParameter("foo", "chargingStation.totalEnergyCharged"))
except traci.TraCIException:
    pass
print("getParameter", traci.simulation.getParameter("cs1", "chargingStation.totalEnergyCharged"))
print("getParameter parkingArea.capacity", traci.simulation.getParameter("pa1", "parkingArea.capacity"))
print("getParameter parkingArea.occupancy", traci.simulation.getParameter("pa1", "parkingArea.occupancy"))
try:
    print("getBusStopWaiting", traci.simulation.getBusStopWaiting("foo"))
except traci.TraCIException:
    pass
print("getBusStopWaiting", traci.simulation.getBusStopWaiting("bs"))
try:
    print("findRoute", traci.simulation.findRoute("foo", "fup"))
except traci.TraCIException:
    pass
print("findRoute", traci.simulation.findRoute("o", "2o"))
try:
    print("findIntermodalRoute", traci.simulation.findIntermodalRoute("foo", "fup"))
except traci.TraCIException:
    pass
print("findIntermodalRoute", traci.simulation.findIntermodalRoute("o", "2o"))

for step in range(10):
    print("step", step)
    traci.simulationStep()
    if traci.simulation.getCollidingVehiclesNumber() > 0:
        checkVehicleStates()
    print(traci.simulation.getSubscriptionResults())
traci.close()
sumoProcess.wait()
