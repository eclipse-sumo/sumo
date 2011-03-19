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
print "routes", traci.route.getIDList()
routeID = "horizontal"
print "examining", routeID
print "edges", traci.route.getEdges(routeID)
traci.route.subscribe(routeID)
print traci.route.getSubscriptionResults(routeID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.route.getSubscriptionResults(routeID)
traci.close()
