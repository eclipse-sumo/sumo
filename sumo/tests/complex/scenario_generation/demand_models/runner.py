#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Daniel Krajzewicz
@date    2013-10-30
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
# import osm network


import sys
import os
import subprocess
del sys.path[0]
del sys.path[0]
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))
sys.path.append(
    "D:\\projects\\x_EU_COLOMBO_318622\\svn_smartSVN\\trunk\\software\\sumo\\tools")

import sumolib.net.generator.cross as generator
from sumolib.net.generator.network import *
from sumolib.net.generator.demand import *

demand = Demand()
demand.addStream(Stream(None, None, None, 800, "from", "to", "passenger"))
vehicles1 = []
for s in demand.streams:
    vehicles1.extend(s.toVehicles(0, 86400))

#demand.build(0, 3600, 3600, "net.net.xml", "linear.rou.xml")

demand = Demand()
demand.addStream(Stream(None, 0, 39600, 400, "from", "to", "passenger"))
demand.addStream(Stream(None, 39600, 46800, LinearChange(
    400, 1200, 39600, 46800), "from", "to", "passenger"))
demand.addStream(Stream(None, 46800, 86400, 1200, "from", "to", "passenger"))
vehicles2 = []
for s in demand.streams:
    vehicles2.extend(s.toVehicles(0, 86400))
#demand.build(0, 3600, 3600, "net.net.xml", "linear.rou.xml")

vehicles3 = []  # [600, 0, .000025, 14400]
demand = Demand()
demand.addStream(Stream(None, None, None, WaveComposition(
    800, [[400, 0, .000025, 14400], [200, 0, .00001, 14400]]), "from", "to", "passenger"))
for s in demand.streams:
    vehicles3.extend(s.toVehicles(0, 86400))
#demand.build(0, 3600, "net.net.xml", "linear.rou.xml")

STEP = 300
d1 = [0] * (86400 / STEP)
for v in vehicles1:
    d = v.depart / STEP
    d1[int(d)] = d1[int(d)] + 1
d2 = [0] * (86400 / STEP)
for v in vehicles2:
    d = v.depart / STEP
    d2[int(d)] = d2[int(d)] + 1
d3 = [0] * (86400 / STEP)
for v in vehicles3:
    d = v.depart / STEP
    d3[int(d)] = d3[int(d)] + 1

fdo = open("t1.csv", "w")
for i in range(0, 86400 / STEP):
    fdo.write("%s;%s;%s;%s\n" % (
        i * STEP, d1[i] * 3600 / STEP, d2[i] * 3600 / STEP, d3[i] * 3600 / STEP))
fdo.close()
