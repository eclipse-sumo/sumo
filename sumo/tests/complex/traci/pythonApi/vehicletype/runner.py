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
print "vehicletypes", traci.vehicletype.getIDList()
typeID = "DEFAULT_VEHTYPE"
print "examining", typeID
print "length", traci.vehicletype.getLength(typeID)
print "maxSpeed", traci.vehicletype.getMaxSpeed(typeID)
print "accel", traci.vehicletype.getAccel(typeID)
print "decel", traci.vehicletype.getDecel(typeID)
print "tau", traci.vehicletype.getTau(typeID)
print "vClass", traci.vehicletype.getVehicleClass(typeID)
print "emissionclass", traci.vehicletype.getEmissionClass(typeID)
print "shape", traci.vehicletype.getShapeClass(typeID)
print "guiOffset", traci.vehicletype.getGUIOffset(typeID)
print "width", traci.vehicletype.getWidth(typeID)
print "color", traci.vehicletype.getColor(typeID)
traci.vehicletype.subscribe(typeID)
print traci.vehicletype.getSubscriptionResults(typeID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.vehicletype.getSubscriptionResults(typeID)
traci.close()
