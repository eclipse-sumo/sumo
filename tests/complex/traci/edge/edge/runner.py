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

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg",
             '--pedestrian.model', 'nonInteracting'] + sys.argv[1:])
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("edges", traci.edge.getIDList())
print("edge count", traci.edge.getIDCount())
edgeID = "2fi"
print("examining", edgeID)
print("laneNumber", traci.edge.getLaneNumber(edgeID))
print("streetName", traci.edge.getStreetName(edgeID))
print("adaptedTraveltime", traci.edge.getAdaptedTraveltime(edgeID, 0))
print("effort", traci.edge.getEffort(edgeID, 0))
print("CO2", traci.edge.getCO2Emission(edgeID))
print("CO", traci.edge.getCOEmission(edgeID))
print("HC", traci.edge.getHCEmission(edgeID))
print("PMx", traci.edge.getPMxEmission(edgeID))
print("NOx", traci.edge.getNOxEmission(edgeID))
print("Fuel", traci.edge.getFuelConsumption(edgeID))
print("Noise", traci.edge.getNoiseEmission(edgeID))
print("Elec", traci.edge.getElectricityConsumption(edgeID))
print("meanSpeed", traci.edge.getLastStepMeanSpeed(edgeID))
print("occupancy", traci.edge.getLastStepOccupancy(edgeID))
print("lastLength", traci.edge.getLastStepLength(edgeID))
print("traveltime", traci.edge.getTraveltime(edgeID))
print("numVeh", traci.edge.getLastStepVehicleNumber(edgeID))
print("haltVeh", traci.edge.getLastStepHaltingNumber(edgeID))
print("vehIds", traci.edge.getLastStepVehicleIDs(edgeID))
print("personIds", traci.edge.getLastStepPersonIDs(edgeID))
print("waiting time", traci.edge.getWaitingTime(edgeID))
# testing unicode
print("checking occupancy with unicode id",
      traci.edge.getLastStepOccupancy(str(edgeID)))

traci.edge.adaptTraveltime(edgeID, 42.)
print("traveltime after adaption", traci.edge.getTraveltime(edgeID))
print("adaptedTraveltime after adaption",
      traci.edge.getAdaptedTraveltime(edgeID, 0))

traci.edge.adaptTraveltime(edgeID, 24., 10, 20)
print("adaptedTraveltime after adaption in interval (check time 0)",
      traci.edge.getAdaptedTraveltime(edgeID, 0))
print("adaptedTraveltime after adaption in interval (check time 15)",
      traci.edge.getAdaptedTraveltime(edgeID, 15))
print("adaptedTraveltime after adaption in interval (check time 25)",
      traci.edge.getAdaptedTraveltime(edgeID, 25))

traci.edge.setEffort(edgeID, 1234.)
print("effort after adaption", traci.edge.getEffort(edgeID, 0))

traci.edge.setEffort(edgeID, 2468., 10, 20)
print("effort after adaption in interval (check time 0)",
      traci.edge.getEffort(edgeID, 0))
print("effort after adaption in interval (check time 15)",
      traci.edge.getEffort(edgeID, 15))
print("effort after adaption in interval (check time 25)",
      traci.edge.getEffort(edgeID, 25))


traci.edge.setMaxSpeed(edgeID, 23.)
print("max speed after adaption", traci.lane.getMaxSpeed(edgeID + "_0"))

traci.edge.subscribe(edgeID)
print(traci.edge.getSubscriptionResults(edgeID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.edge.getSubscriptionResults(edgeID))
print(traci.edge.getAllSubscriptionResults())
for step in range(10):
    traci.simulationStep()
    print("3si count=%s meanSpeed=%s travelTime=%s" % (
        traci.edge.getLastStepVehicleNumber("3si"),
        traci.edge.getLastStepMeanSpeed("3si"),
        traci.edge.getTraveltime("3si")))

traci.close()
