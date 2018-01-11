#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @date    2013-10-30
# @version $Id$

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
