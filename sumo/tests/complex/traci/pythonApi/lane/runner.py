#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, subprocess, sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

sumoBinary = sumolib.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(3):
    print "step", step
    traci.simulationStep()
print "lanes", traci.lane.getIDList()
print "lane count", traci.lane.getIDCount()
laneID = "2fi_0"
print "examining", laneID
print "length", traci.lane.getLength(laneID)
print "maxSpeed", traci.lane.getMaxSpeed(laneID)
print "width", traci.lane.getWidth(laneID)
print "allowed", traci.lane.getAllowed(laneID)
print "disallowed", traci.lane.getDisallowed(laneID)
print "linkNum", traci.lane.getLinkNumber(laneID)
print "links", traci.lane.getLinks(laneID)
print "shape", traci.lane.getShape(laneID)
print "edge", traci.lane.getEdgeID(laneID)
print "CO2", traci.lane.getCO2Emission(laneID)
print "CO", traci.lane.getCOEmission(laneID)
print "HC", traci.lane.getHCEmission(laneID)
print "PMx", traci.lane.getPMxEmission(laneID)
print "NOx", traci.lane.getNOxEmission(laneID)
print "Fuel", traci.lane.getFuelConsumption(laneID)
print "Noise", traci.lane.getNoiseEmission(laneID)
print "meanSpeed", traci.lane.getLastStepMeanSpeed(laneID)
print "occupancy", traci.lane.getLastStepOccupancy(laneID)
print "lastLength", traci.lane.getLastStepLength(laneID)
print "traveltime", traci.lane.getTraveltime(laneID)
print "numVeh", traci.lane.getLastStepVehicleNumber(laneID)
print "haltVeh", traci.lane.getLastStepHaltingNumber(laneID)
print "vehIds", traci.lane.getLastStepVehicleIDs(laneID)
print "waiting time", traci.lane.getWaitingTime(laneID)

traci.lane.setAllowed(laneID, ["taxi"])
print "after setAllowed", traci.lane.getAllowed(laneID), traci.lane.getDisallowed(laneID)
traci.lane.setDisallowed(laneID, ["bus"])
print "after setDisallowed", traci.lane.getAllowed(laneID), traci.lane.getDisallowed(laneID)
traci.lane.setMaxSpeed(laneID, 42.)
print "after setMaxSpeed", traci.lane.getMaxSpeed(laneID)
traci.lane.setLength(laneID, 123.)
print "after setLength", traci.lane.getLength(laneID)

traci.lane.subscribe(laneID)
print traci.lane.getSubscriptionResults(laneID)
for step in range(3,6):
    print "step", step
    traci.simulationStep()
    print traci.lane.getSubscriptionResults(laneID)
traci.close()
