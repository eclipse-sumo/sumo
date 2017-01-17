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
