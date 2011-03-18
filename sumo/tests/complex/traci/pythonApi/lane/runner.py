#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,subprocess,sys,shutil, struct
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools", "lib"))
import traci, testUtil

sumoBinary = testUtil.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(3):
    print "step", step
    traci.simulationStep(step)
print "lanes", traci.lane.getIDList()
laneID = "2fi_0"
print "examining", laneID
print "length", traci.lane.getLength(laneID)
print "maxSpeed", traci.lane.getMaxSpeed(laneID)
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
traci.lane.subscribe(laneID)
print traci.lane.getSubscriptionResults(laneID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.lane.getSubscriptionResults(laneID)
traci.close()
