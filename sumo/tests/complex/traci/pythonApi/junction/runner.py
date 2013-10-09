#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,subprocess,sys,shutil, struct
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

sumoBinary = sumolib.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(3):
    print "step", step
    traci.simulationStep()
print "junctions", traci.junction.getIDList()
print "junction count", traci.junction.getIDCount()
junctionID = "0"
print "examining", junctionID
print "pos", traci.junction.getPosition(junctionID)
traci.junction.subscribe(junctionID)
print traci.junction.getSubscriptionResults(junctionID)
for step in range(3,6):
    print "step", step
    traci.simulationStep()
    print traci.junction.getSubscriptionResults(junctionID)
traci.close()
