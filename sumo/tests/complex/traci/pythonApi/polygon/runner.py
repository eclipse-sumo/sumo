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
polygonID = "0"
print "adding", polygonID
traci.polygon.add(polygonID, ((1,1), (1,10), (10,10)), (1,2,3,4), True, "test")

print "polygons", traci.polygon.getIDList()
print "examining", polygonID
print "shape", traci.polygon.getShape(polygonID)
print "type", traci.polygon.getType(polygonID)
print "color", traci.polygon.getColor(polygonID)

traci.polygon.subscribe(polygonID)
print traci.polygon.getSubscriptionResults(polygonID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.polygon.getSubscriptionResults(polygonID)
traci.close()
