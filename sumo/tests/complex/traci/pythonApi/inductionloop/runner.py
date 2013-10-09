#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, subprocess, sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

sumoBinary = sumolib.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(4):
    print "step", step
    traci.simulationStep()
print "inductionloops", traci.inductionloop.getIDList()
print "inductionloop count", traci.inductionloop.getIDCount()
loopID = "0"
print "examining", loopID
print "vehNum", traci.inductionloop.getLastStepVehicleNumber(loopID)
print "meanSpeed", traci.inductionloop.getLastStepMeanSpeed(loopID)
print "vehIDs", traci.inductionloop.getLastStepVehicleIDs(loopID)
print "occupancy", traci.inductionloop.getLastStepOccupancy(loopID)
print "meanLength", traci.inductionloop.getLastStepMeanLength(loopID)
print "timeSinceDet", traci.inductionloop.getTimeSinceDetection(loopID)
print "vehData", traci.inductionloop.getVehicleData(loopID)

traci.inductionloop.subscribe(loopID)
print traci.inductionloop.getSubscriptionResults(loopID)
for step in range(3,6):
    print "step", step
    traci.simulationStep()
    print traci.inductionloop.getSubscriptionResults(loopID)
traci.close()
