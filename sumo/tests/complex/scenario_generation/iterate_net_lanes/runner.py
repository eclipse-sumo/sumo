#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import osm network 


import sys,os,subprocess
del sys.path[0]
del sys.path[0]
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))
sys.path.append("D:\\projects\\x_EU_COLOMBO_318622\\svn_smartSVN\\trunk\\software\\sumo\\tools")

import sumolib.net.generator.cross as generator
from sumolib.net.generator.network import *
from sumolib.net.generator.demand import *

import sumolib.net.generator.cross as generator
for i in range(1, 5):
  defaultEdge = Edge(numLanes=i, maxSpeed=13.89)
  net = generator.cross(None, defaultEdge)
  net.build("net_with_%s_lanes.net.xml" % i)


