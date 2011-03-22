#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os, subprocess, sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools", "lib"))
import traci, testUtil

sumoBinary = testUtil.checkBinary('sumo-gui')

sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
for step in range(3):
    print "step", step
    traci.simulationStep(step)
poiID = "0"
print "adding", poiID
traci.poi.add(poiID, 1, 1, (1,2,3,4), "test")

print "pois", traci.poi.getIDList()
print "examining", poiID
print "pos", traci.poi.getPosition(poiID)
print "type", traci.poi.getType(poiID)
print "color", traci.poi.getColor(poiID)

traci.poi.subscribe(poiID)
print traci.poi.getSubscriptionResults(poiID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.poi.getSubscriptionResults(poiID)
traci.close()
