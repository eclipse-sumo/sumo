#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, subprocess, sys, random
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci, sumolib

def check(poiID):
    print "pois", traci.poi.getIDList()
    print "poi count", traci.poi.getIDCount()
    print "examining", poiID
    print "pos", traci.poi.getPosition(poiID)
    print "type", traci.poi.getType(poiID)
    print "color", traci.poi.getColor(poiID)

sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen("%s -S -Q -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print "step", step
    traci.simulationStep()
poiID = "0"
print "adding", poiID
traci.poi.add(poiID, 1, 1, (1,2,3,4), "test")
check(poiID)
traci.poi.subscribe(poiID)
print traci.poi.getSubscriptionResults(poiID)
for step in range(3,6):
    print "step", step
    traci.simulationStep(step)
    print traci.poi.getSubscriptionResults(poiID)

traci.poi.setPosition(poiID, 5, 5)
traci.poi.setType(poiID, "blub")
traci.poi.setColor(poiID, (222, 111, 221, 0))
check(poiID)
traci.poi.add("p2", 2, 2, (11,21,31,41), "ptest")
check("p2")
traci.poi.remove(poiID)
check("p2")
traci.close()
