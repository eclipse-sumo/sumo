#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, subprocess, sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools", "lib"))
import traci, testUtil

sumoBinary = testUtil.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(3):
    print "step", step
    traci.simulationStep(step)
print "inductionloops", traci.inductionloop.getIDList()
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
    traci.simulationStep(step)
    print traci.inductionloop.getSubscriptionResults(loopID)
traci.close()
