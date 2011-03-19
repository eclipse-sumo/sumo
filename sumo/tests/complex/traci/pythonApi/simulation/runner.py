#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,subprocess,sys,shutil, struct
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools", "lib"))
import traci, testUtil

sumoBinary = testUtil.checkBinary('sumo')

sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
traci.init(8813)
traci.simulation.subscribe()
print traci.simulation.getSubscriptionResults()
for step in range(6):
    print "step", step
    traci.simulationStep(step)
    print traci.simulation.getSubscriptionResults()
print "time", traci.simulation.getCurrentTime()
print "departed", traci.simulation.getDepartedIDList()
print "arrived", traci.simulation.getArrivedIDList()
print "teleportStart", traci.simulation.getStartingTeleportIDList()
print "teleportEnd", traci.simulation.getEndingTeleportIDList()
traci.close()
