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
print "multientryexits", traci.multientryexit.getIDList()
detID = "0"
print "examining", detID
print "vehNum", traci.multientryexit.getLastStepVehicleNumber(detID)
print "meanSpeed", traci.multientryexit.getLastStepMeanSpeed(detID)
print "vehIDs", traci.multientryexit.getLastStepVehicleIDs(detID)
print "haltNum", traci.multientryexit.getLastStepHaltingNumber(detID)
traci.multientryexit.subscribe(detID)
print traci.multientryexit.getSubscriptionResults(detID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.multientryexit.getSubscriptionResults(detID)
traci.close()
