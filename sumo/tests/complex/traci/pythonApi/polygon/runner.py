#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = random.randint(8000, 50000)
sumoProcess = subprocess.Popen("%s -S -Q -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print "step", step
    traci.simulationStep()
polygonID = "0"
print "adding", polygonID
traci.polygon.add(polygonID, ((1,1), (1,10), (10,10)), (1,2,3,4), True, "test")

print "polygons", traci.polygon.getIDList()
print "polygon count", traci.polygon.getIDCount()
print "examining", polygonID
print "shape", traci.polygon.getShape(polygonID)
print "type", traci.polygon.getType(polygonID)
print "color", traci.polygon.getColor(polygonID)

traci.polygon.subscribe(polygonID)
print traci.polygon.getSubscriptionResults(polygonID)
for step in range(3,6):
    print "step", step
    traci.simulationStep()
    print traci.polygon.getSubscriptionResults(polygonID)
traci.close()
