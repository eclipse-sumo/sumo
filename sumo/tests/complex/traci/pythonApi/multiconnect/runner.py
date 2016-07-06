#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

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
import shutil
import struct
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORTS = [
    sumolib.miscutils.getFreeSocketPort(), sumolib.miscutils.getFreeSocketPort()]
sumoProcess = [subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, p), shell=True, stdout=sys.stdout) for p in PORTS]
conns = [traci.connect(p) for p in PORTS]
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
for p in sumoProcess:
    p.wait()
