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
print "edges", traci.edge.getIDList()
print "edge count", traci.edge.getIDCount()
edgeID = "2fi"
print "examining", edgeID
print "adaptedTraveltime", traci.edge.getAdaptedTraveltime(edgeID, 0)
print "effort", traci.edge.getEffort(edgeID, 0)
print "CO2", traci.edge.getCO2Emission(edgeID)
print "CO", traci.edge.getCOEmission(edgeID)
print "HC", traci.edge.getHCEmission(edgeID)
print "PMx", traci.edge.getPMxEmission(edgeID)
print "NOx", traci.edge.getNOxEmission(edgeID)
print "Fuel", traci.edge.getFuelConsumption(edgeID)
print "Noise", traci.edge.getNoiseEmission(edgeID)
print "meanSpeed", traci.edge.getLastStepMeanSpeed(edgeID)
print "occupancy", traci.edge.getLastStepOccupancy(edgeID)
print "lastLength", traci.edge.getLastStepLength(edgeID)
print "traveltime", traci.edge.getTraveltime(edgeID)
print "numVeh", traci.edge.getLastStepVehicleNumber(edgeID)
print "haltVeh", traci.edge.getLastStepHaltingNumber(edgeID)
print "vehIds", traci.edge.getLastStepVehicleIDs(edgeID)
print "waiting time", traci.edge.getWaitingTime(edgeID)

traci.edge.adaptTraveltime(edgeID, 42.)
print "traveltime after adaption", traci.edge.getTraveltime(edgeID)
print "adaptedTraveltime after adaption", traci.edge.getAdaptedTraveltime(edgeID, 0)
traci.edge.setEffort(edgeID, 1234.)
print "effort after adaption", traci.edge.getEffort(edgeID, 0)
traci.edge.setMaxSpeed(edgeID, 23.)
print "max speed after adaption", traci.lane.getMaxSpeed(edgeID+"_0")

traci.edge.subscribe(edgeID)
print traci.edge.getSubscriptionResults(edgeID)
for step in range(3,6):
    print "step", step
    traci.simulationStep()
    print traci.edge.getSubscriptionResults(edgeID)
traci.close()
