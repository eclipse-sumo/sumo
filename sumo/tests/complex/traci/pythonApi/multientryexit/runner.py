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
print "multientryexits", traci.multientryexit.getIDList()
print "multientryexit count", traci.multientryexit.getIDCount()
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
    traci.simulationStep()
    print traci.multientryexit.getSubscriptionResults(detID)
traci.close()
