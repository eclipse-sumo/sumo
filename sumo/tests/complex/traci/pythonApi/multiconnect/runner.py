#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')
conns = []
sumoProcess = []
for i in range(2):
    p = sumolib.miscutils.getFreeSocketPort()
    sumoProcess = subprocess.Popen([sumoBinary, "-c", "sumo.sumocfg",
                                    "--remote-port", str(p)], stdout=sys.stdout)
    conns.append(traci.connect(p, proc=sumoProcess))
for c in conns:
    for step in range(3):
        print("step", step)
        c.simulationStep()
for c in conns:
    print("routes", c.route.getIDList())
    print("route count", c.route.getIDCount())
    routeID = "horizontal"
    print("examining", routeID)
    print("edges", c.route.getEdges(routeID))
    c.route.subscribe(routeID)
for c in conns:
    print(c.route.getSubscriptionResults(routeID))
    for step in range(3, 6):
        print("step", step)
        c.simulationStep(step)
        print(c.route.getSubscriptionResults(routeID))
    c.route.add("h2", ["1o"])
    print("routes", c.route.getIDList())
    print("edges", c.route.getEdges("h2"))
    c.close()
