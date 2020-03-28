#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Daniel Krajzewicz
# @date    2013-10-30

# import osm network


import sys
import os
SUMO_HOME = os.environ.get('SUMO_HOME',
                           os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
sys.path.append(os.path.join(SUMO_HOME, 'tools'))

from sumolib.net.generator.network import Edge  # noqa
from sumolib.net.generator.demand import Demand, Stream, LinearChange, WaveComposition  # noqa

demand = Demand()
demand.addStream(Stream(None, None, None, 800, "from", "to", "passenger"))
vehicles1 = []
for s in demand.streams:
    vehicles1.extend(s.toVehicles(0, 86400))

# demand.build(0, 3600, 3600, "net.net.xml", "linear.rou.xml")

demand = Demand()
demand.addStream(Stream(None, 0, 39600, 400, "from", "to", "passenger"))
demand.addStream(Stream(None, 39600, 46800, LinearChange(
    400, 1200, 39600, 46800), "from", "to", "passenger"))
demand.addStream(Stream(None, 46800, 86400, 1200, "from", "to", "passenger"))
vehicles2 = []
for s in demand.streams:
    vehicles2.extend(s.toVehicles(0, 86400))
# demand.build(0, 3600, 3600, "net.net.xml", "linear.rou.xml")

vehicles3 = []  # [600, 0, .000025, 14400]
demand = Demand()
demand.addStream(Stream(None, None, None, WaveComposition(
    800, [[400, 0, .000025, 14400], [200, 0, .00001, 14400]]), "from", "to", "passenger"))
for s in demand.streams:
    vehicles3.extend(s.toVehicles(0, 86400))
# demand.build(0, 3600, "net.net.xml", "linear.rou.xml")

STEP = 300
d1 = [0] * (86400 // STEP)
for v in vehicles1:
    d = v.depart / STEP
    d1[int(d)] = d1[int(d)] + 1
d2 = [0] * (86400 // STEP)
for v in vehicles2:
    d = v.depart / STEP
    d2[int(d)] = d2[int(d)] + 1
d3 = [0] * (86400 // STEP)
for v in vehicles3:
    d = v.depart / STEP
    d3[int(d)] = d3[int(d)] + 1

fdo = open("t1.csv", "w")
for i in range(0, 86400 // STEP):
    fdo.write("%s;%s;%s;%s\n" % (
        i * STEP, d1[i] * 3600 / STEP, d2[i] * 3600 / STEP, d3[i] * 3600 / STEP))
fdo.close()
