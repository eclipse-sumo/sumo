#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
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

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib.net.generator.cross as generator  # noqa
from sumolib.net.generator.network import Edge  # noqa

for i in range(1, 5):
    defaultEdge = Edge(numLanes=i, maxSpeed=13.89)
    net = generator.cross(None, defaultEdge)
    net.build("net_with_%s_lanes.net.xml" % i)
