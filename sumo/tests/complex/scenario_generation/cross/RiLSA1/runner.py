#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Daniel Krajzewicz
@date    2013-10-23
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

flows = [
    ["nm", [
        ["ms", 159, 9],
        ["me", 59, 9],
        ["mw", 64, 12]
    ]],

    ["wm", [
        ["me", 708, 10],
        ["mn", 80, 14],
        ["ms", 130, 2]
    ]],

    ["em", [
        ["mw", 571, 10],
        ["mn", 57, 9],
        ["ms", 47, 3]
    ]],

    ["sm", [
        ["mn", 154, 2],
        ["me", 49, 2],
        ["mw", 92, 2]
    ]]

]

import sys
import os
import subprocess
del sys.path[0]
del sys.path[0]
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))

import sumolib.net.generator.cross as generator
from sumolib.net.generator.network import *
from sumolib.net.generator.demand import *

defaultEdge = Edge(numLanes=1, maxSpeed=13.89)
defaultEdge.addSplit(100, 1)
defaultEdge.lanes = [Lane(dirs="rs"), Lane(dirs="l")]
net = generator.cross(None, defaultEdge)
net.build()


flows = [
    ["nm", [
        ["ms", 159, 9],
        ["me", 59, 9],
        ["mw", 64, 12]
    ]],

    ["wm", [
        ["me", 708, 10],
        ["mn", 80, 14],
        ["ms", 130, 2]
    ]],

    ["em", [
        ["mw", 571, 10],
        ["mn", 57, 9],
        ["ms", 47, 3]
    ]],

    ["sm", [
        ["mn", 154, 2],
        ["me", 49, 2],
        ["mw", 92, 2]
    ]]

]


demand = Demand()
for f in flows:
    for rel in f[1]:
        prob = rel[2] / 100.
        iprob = 1. - prob
        demand.addStream(
            Stream(None, rel[1], f[0], rel[0], {prob: "lkw", iprob: "pkw"}))
demand.build(0, 3600, 3600, "net.net.xml")
