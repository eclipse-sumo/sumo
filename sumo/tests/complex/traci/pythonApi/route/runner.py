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
print "routes", traci.route.getIDList()
print "route count", traci.route.getIDCount()
routeID = "horizontal"
print "examining", routeID
print "edges", traci.route.getEdges(routeID)
traci.route.subscribe(routeID)
print traci.route.getSubscriptionResults(routeID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.route.getSubscriptionResults(routeID)
traci.route.add("h2", ["1o"])
print "routes", traci.route.getIDList()
print "edges", traci.route.getEdges("h2")
traci.close()
