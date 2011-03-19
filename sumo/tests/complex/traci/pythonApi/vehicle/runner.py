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
print "vehicles", traci.vehicle.getIDList()
vehID = "horiz"
print "examining", vehID
print "speed", traci.vehicle.getSpeed(vehID)
print "pos", traci.vehicle.getPosition(vehID)
print "angle", traci.vehicle.getAngle(vehID)
print "road", traci.vehicle.getRoadID(vehID)
print "lane", traci.vehicle.getLaneID(vehID)
print "laneIndex", traci.vehicle.getLaneIndex(vehID)
print "type", traci.vehicle.getTypeID(vehID)
print "routeID", traci.vehicle.getRouteID(vehID)
print "route", traci.vehicle.getRoute(vehID)
print "lanePos", traci.vehicle.getLanePosition(vehID)
print "color", traci.vehicle.getColor(vehID)
print "bestLanes", traci.vehicle.getBestLanes(vehID)
traci.vehicle.subscribe(vehID)
print traci.vehicle.getSubscriptionResults(vehID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.vehicle.getSubscriptionResults(vehID)
traci.close()
