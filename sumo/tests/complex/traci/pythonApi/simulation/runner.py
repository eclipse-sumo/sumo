#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,subprocess,sys,shutil, struct
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

sumoBinary = sumolib.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumocfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
traci.simulation.subscribe()
print traci.simulation.getSubscriptionResults()
for step in range(6):
    print "step", step
    traci.simulationStep(step)
    print traci.simulation.getSubscriptionResults()
print "time", traci.simulation.getCurrentTime()
print "#loaded", traci.simulation.getLoadedNumber()
print "loaded", traci.simulation.getLoadedIDList()
print "#departed", traci.simulation.getDepartedNumber()
print "departed", traci.simulation.getDepartedIDList()
print "#arrived", traci.simulation.getArrivedNumber()
print "arrived", traci.simulation.getArrivedIDList()
print "min#expected", traci.simulation.getMinExpectedNumber()
print "#teleportStart", traci.simulation.getStartingTeleportNumber()
print "teleportStart", traci.simulation.getStartingTeleportIDList()
print "#teleportEnd", traci.simulation.getEndingTeleportNumber()
print "teleportEnd", traci.simulation.getEndingTeleportIDList()
print "deltaT", traci.simulation.getDeltaT()
print "boundary", traci.simulation.getNetBoundary()
print "convert2D", traci.simulation.convert2D("1o", 0.)
print "convertGeo", traci.simulation.convert2D("1o", 0., toGeo=True)
print "convertRoad", traci.simulation.convertRoad(488.65, 501.65)
print "distance2D", traci.simulation.getDistance2D(488.65, 501.65, 498.65, 501.65)
print "drivingDistance2D", traci.simulation.getDistance2D(488.65, 501.65, 498.65, 501.65, isDriving=True)
print "distanceRoad", traci.simulation.getDistanceRoad("1o", 0., "2o", 0.)
print "drivingDistanceRoad", traci.simulation.getDistanceRoad("1o", 0., "2o", 0., isDriving=True)
traci.close()
