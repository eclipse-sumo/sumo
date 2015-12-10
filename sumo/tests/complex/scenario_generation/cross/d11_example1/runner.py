#!/usr/bin/env python
# -*- coding: utf-8 -*-

# import osm network 


import sys,os,subprocess
del sys.path[0]
del sys.path[0]
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))

import sumolib.net.generator.cross as generator
from sumolib.net.generator.network import *

defaultEdge = Edge(numLanes=3, maxSpeed=13.89, lanes=[Lane(dirs="rs"), Lane(dirs="s"), Lane(dirs="l")])
net = generator.cross(None, defaultEdge)
net.build()
