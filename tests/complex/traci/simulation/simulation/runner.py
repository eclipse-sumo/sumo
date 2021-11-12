#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
# @author  Jakob Erdmann
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa


def checkVehicleStates():
    print("time", traci.simulation.getTime())
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
    print("#emergencystopping", traci.simulation.getEmergencyStoppingVehiclesNumber())
    print("emergencystopping", traci.simulation.getEmergencyStoppingVehiclesIDList())
    print("min#expected", traci.simulation.getMinExpectedNumber())
    print("#teleportStart", traci.simulation.getStartingTeleportNumber())
    print("teleportStart", traci.simulation.getStartingTeleportIDList())
    print("#teleportEnd", traci.simulation.getEndingTeleportNumber())
    print("teleportEnd", traci.simulation.getEndingTeleportIDList())


def ppStages(comment, stages):
    print("%s\n  %s\n" % (comment, "\n  ".join(map(str, stages))))


print("loaded?", traci.isLoaded())
version = traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg",
                       "--ignore-route-errors",
                       "--end", "42",
                       "--log", "log.txt"])
print("version at start", version)
print("version", traci.getVersion())
print("loaded?", traci.isLoaded())
print("endTime", traci.simulation.getEndTime())

traci.simulation.subscribe(
    [traci.constants.VAR_LOADED_VEHICLES_IDS, traci.constants.VAR_DEPARTED_VEHICLES_IDS])
print(traci.simulation.getSubscriptionResults())
for step in range(6):
    print("step", step)
    traci.simulationStep()
    print("pendingVehicles", traci.simulation.getPendingVehicles())
    print(traci.simulation.getSubscriptionResults())
checkVehicleStates()
print("deltaT", traci.simulation.getDeltaT())
print("boundary", traci.simulation.getNetBoundary())
print("convertRoad2D", traci.simulation.convert2D("o", 0.))
print("convertRoad3D", traci.simulation.convert3D("o", 0.))
print("convertRoadGeo", traci.simulation.convert2D("o", 0., toGeo=True))
print("convertRoadGeoAlt", traci.simulation.convert3D("o", 0., toGeo=True))
print("convertRoad2D length", traci.simulation.convert2D("2o", 8000.))
print("convert2DGeo", traci.simulation.convertGeo(488.65, 501.65))
print("convertGeo2D", traci.simulation.convertGeo(12, 48, True))
print("convert2DRoad", traci.simulation.convertRoad(488.65, 501.65))
print("convertGeoRoad", traci.simulation.convertRoad(12, 48.1, True))
print("convertGeoRoadBus", traci.simulation.convertRoad(12, 48.1, True, "bus"))
traci.lane.setDisallowed("o_0", ["bus"])
print("convertGeoRoadBusDisallowed", traci.simulation.convertRoad(12, 48.1, True, "bus"))
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
    traci.simulation.setParameter("foo", "foo.bla", "42")
except traci.TraCIException:
    pass
traci.simulation.setParameter("", "bar", "42")
print("getParameter (generic)", traci.simulation.getParameter("", "bar"))

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
print("getParameter charginStation.totalEnergyCharged",
      traci.simulation.getParameter("cs1", "chargingStation.totalEnergyCharged"))
print("getParameter chargingStation.name", traci.simulation.getParameter("cs1", "chargingStation.name"))
print("getParameter chargingStation.lane", traci.simulation.getParameter("cs1", "chargingStation.lane"))
print("getParameter chargingStation.key1", traci.simulation.getParameter("cs1", "chargingStation.key1"))

print("getParameter parkingArea.capacity", traci.simulation.getParameter("pa1", "parkingArea.capacity"))
print("getParameter parkingArea.occupancy", traci.simulation.getParameter("pa1", "parkingArea.occupancy"))
print("getParameter parkingArea.name", traci.simulation.getParameter("pa1", "parkingArea.name"))
print("getParameter parkingArea.lane", traci.simulation.getParameter("pa1", "parkingArea.lane"))
print("getParameter parkingArea.key3", traci.simulation.getParameter("pa1", "parkingArea.key3"))

print("getParameter busStop.name", traci.simulation.getParameter("bs", "busStop.name"))
print("getParameter busStop.lane", traci.simulation.getParameter("bs", "busStop.lane"))
print("getParameter busStop.key2", traci.simulation.getParameter("bs", "busStop.key2"))

try:
    print("getBusStopWaiting", traci.simulation.getBusStopWaiting("foo"))
except traci.TraCIException:
    pass

print("getBusStopIDList", traci.simulation.getBusStopIDList())
print("getBusStopWaiting", traci.simulation.getBusStopWaiting("bs"))
print("getBusStopWaitingIDList", traci.simulation.getBusStopWaitingIDList("bs"))

try:
    print("findRoute", traci.simulation.findRoute("foo", "fup"))
except traci.TraCIException:
    pass
print("findRoute", traci.simulation.findRoute("o", "2o"))
print("findRoute with routing mode", traci.simulation.findRoute(
    "o", "2o", routingMode=traci.constants.ROUTING_MODE_AGGREGATED))
try:
    print("findRoute", traci.simulation.findRoute("footpath", "footpath2", "DEFAULT_VEHTYPE"))
except traci.TraCIException:
    pass

try:
    print("findIntermodalRoute", traci.simulation.findIntermodalRoute("foo", "fup"))
except traci.TraCIException:
    pass
print("findIntermodalRoute", traci.simulation.findIntermodalRoute(
    "footpath", "footpath2", "bicycle", vType="DEFAULT_BIKETYPE"))
ppStages("findIntermodalRoute (walk)", traci.simulation.findIntermodalRoute("o", "2o"))
ppStages("findIntermodalRoute (bike)", traci.simulation.findIntermodalRoute("o", "2o", modes="bicycle"))
ppStages("findIntermodalRoute (car)", traci.simulation.findIntermodalRoute("o", "2o", modes="car"))
ppStages("findIntermodalRoute (bike,car,public)",
         traci.simulation.findIntermodalRoute("o", "2o", modes="car bicycle public"))

traci.vehicle.setSpeedMode("emergencyStopper", 0)
traci.vehicle.setSpeed("emergencyStopper", 100)
try:
    traci.simulation.subscribeContext("",
                                      traci.constants.CMD_GET_VEHICLE_VARIABLE, 42,
                                      [traci.constants.VAR_SPEED])
    print("contextSubscriptions:", traci.simulation.getAllContextSubscriptionResults())
except traci.TraCIException:
    pass

for step in range(12):
    print("step", step)
    traci.simulationStep()
    if traci.simulation.getCollidingVehiclesNumber() > 0:
        print("detected collision")
        checkVehicleStates()
    if traci.simulation.getEmergencyStoppingVehiclesNumber() > 0:
        print("detected emergency stop")
        checkVehicleStates()
    print(traci.simulation.getSubscriptionResults())

print("check whether GUI is present", traci.hasGUI())
traci.simulation.writeMessage("custom log message")
traci.simulationStep()
print("loaded?", traci.isLoaded())
traci.close()
print("loaded?", traci.isLoaded())
