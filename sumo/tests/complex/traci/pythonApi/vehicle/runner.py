#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Jakob Erdmann
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, subprocess, sys, random
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = random.randint(8000, 50000)
sumoProcess = subprocess.Popen("%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)

def step():
    s = traci.simulation.getCurrentTime() / 1000
    traci.simulationStep()
    return s

for i in range(3):
    print "step", step()

def check(vehID):
    print "vehicles", traci.vehicle.getIDList()
    print "vehicle count", traci.vehicle.getIDCount()
    print "examining", vehID
    print "speed", traci.vehicle.getSpeed(vehID)
    print "speed w/o traci", traci.vehicle.getSpeedWithoutTraCI(vehID)
    print "pos", traci.vehicle.getPosition(vehID)
    print "angle", traci.vehicle.getAngle(vehID)
    print "road", traci.vehicle.getRoadID(vehID)
    print "lane", traci.vehicle.getLaneID(vehID)
    print "laneIndex", traci.vehicle.getLaneIndex(vehID)
    print "type", traci.vehicle.getTypeID(vehID)
    print "routeID", traci.vehicle.getRouteID(vehID)
    print "route", traci.vehicle.getRoute(vehID)
    print "lanePos", traci.vehicle.getLanePosition(vehID)
    print "color", traci.vehicle.getColor(vehID)
    print "bestLanes", traci.vehicle.getBestLanes(vehID)
    print "CO2", traci.vehicle.getCO2Emission(vehID)
    print "CO", traci.vehicle.getCOEmission(vehID)
    print "HC", traci.vehicle.getHCEmission(vehID)
    print "PMx", traci.vehicle.getPMxEmission(vehID)
    print "NOx", traci.vehicle.getNOxEmission(vehID)
    print "fuel", traci.vehicle.getFuelConsumption(vehID)
    print "noise", traci.vehicle.getNoiseEmission(vehID)
    print "traveltime", traci.vehicle.getAdaptedTraveltime(vehID, 0, "1o")
    print "effort", traci.vehicle.getEffort(vehID, 0, "1o")
    print "route valid", traci.vehicle.isRouteValid(vehID)
    print "signals", traci.vehicle.getSignals(vehID)
    print "length", traci.vehicle.getLength(vehID)
    print "maxSpeed", traci.vehicle.getMaxSpeed(vehID)
    print "speedFactor", traci.vehicle.getSpeedFactor(vehID)
    print "allowedSpeed", traci.vehicle.getAllowedSpeed(vehID)
    print "accel", traci.vehicle.getAccel(vehID)
    print "decel", traci.vehicle.getDecel(vehID)
    print "imperfection", traci.vehicle.getImperfection(vehID)
    print "tau", traci.vehicle.getTau(vehID)
    print "vClass", traci.vehicle.getVehicleClass(vehID)
    print "emissionclass", traci.vehicle.getEmissionClass(vehID)
    print "shape", traci.vehicle.getShapeClass(vehID)
    print "MinGap", traci.vehicle.getMinGap(vehID)
    print "width", traci.vehicle.getWidth(vehID)
    print "waiting time", traci.vehicle.getWaitingTime(vehID)
    print "driving dist", traci.vehicle.getDrivingDistance(vehID, "4fi", 2.)
    print "driving dist 2D", traci.vehicle.getDrivingDistance2D(vehID, 100., 100.)

vehID = "horiz"
check(vehID)
traci.vehicle.subscribe(vehID)
print traci.vehicle.getSubscriptionResults(vehID)
for i in range(3):
    print "step", step()
    print traci.vehicle.getSubscriptionResults(vehID)
traci.vehicle.setLength(vehID, 1.0)
traci.vehicle.setMaxSpeed(vehID, 9.0)
traci.vehicle.setSpeedFactor(vehID, 1.1)
traci.vehicle.setAccel(vehID, 1.1)
traci.vehicle.setDecel(vehID, 5.1)
traci.vehicle.setImperfection(vehID, 0.1)
traci.vehicle.setTau(vehID, 1.1)
traci.vehicle.setVehicleClass(vehID, "bicycle")
traci.vehicle.setEmissionClass(vehID, "zero")
traci.vehicle.setShapeClass(vehID, "bicycle")
traci.vehicle.setMinGap(vehID, 1.1)
traci.vehicle.setWidth(vehID, 1.1)
traci.vehicle.setColor(vehID, (255, 0, 0, 255))
traci.vehicle.setStop(vehID, "2fi", pos=50.0, laneIndex=0, duration=2000, flags=1)
check(vehID)
try:
    check("bla")
except traci.TraCIException:
    print "recovering from exception after asking for unknown vehicle"
traci.vehicle.add("1", "horizontal")
check("1")
traci.vehicle.changeTarget("1", "4fi")
print "routeID", traci.vehicle.getRouteID(vehID)
print "route", traci.vehicle.getRoute(vehID)
print "step", step()
traci.vehicle.addFull("2", "horizontal", line="t")
print traci.vehicle.getIDList()
for i in range(6):
    print "step", step()
    print traci.vehicle.getSubscriptionResults(vehID)
check("2")
print "leader", traci.vehicle.getLeader("2")
traci.vehicle.subscribeLeader("2")
for i in range(6):
    print "step", step()
    print traci.vehicle.getSubscriptionResults("2")
    print traci.vehicle.getSubscriptionResults(vehID)
traci.vehicle.remove("1")
try:
    traci.vehicle.add("anotherOne", "horizontal", pos=-1)
except traci.TraCIException as e:
    print e
try:
    check("anotherOne")
except traci.TraCIException as e:
    print e
traci.close()
