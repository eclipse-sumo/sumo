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
# @date    2013-10-23
# @version $Id$

# import osm network


import sys
import os
import subprocess
del sys.path[0]
del sys.path[0]
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))

import sumolib.net.generator.cross as generator
from sumolib.net.generator.network import *

defaultEdge = Edge(numLanes=1, maxSpeed=13.89)
defaultEdge.addSplit(100, 1)
defaultEdge.lanes = [Lane(dirs="rs"), Lane(dirs="l")]
net = generator.cross(None, defaultEdge)
net.addEdge(Edge("0/1_to_1/1", net.getNode("0/1"), net.getNode("1/1"), numLanes=3,
                 maxSpeed=13.89, lanes=[Lane(dirs="r"), Lane(dirs="s"), Lane(dirs="l")]))
net.build()
