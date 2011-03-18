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
print "edges", traci.edge.getIDList()
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
traci.edge.subscribe(edgeID)
print traci.edge.getSubscriptionResults(edgeID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.edge.getSubscriptionResults(edgeID)
traci.close()
