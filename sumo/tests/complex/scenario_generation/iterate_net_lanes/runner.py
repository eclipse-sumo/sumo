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

import sumolib.net.generator.cross as generator
for i in range(1, 5):
    defaultEdge = Edge(numLanes=i, maxSpeed=13.89)
    net = generator.cross(None, defaultEdge)
    net.build("net_with_%s_lanes.net.xml" % i)
