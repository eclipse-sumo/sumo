#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
@file    network_statistics.py
@author  Ronald Nippold
@author  Michael Behrisch
@date    2009-10-06
@version
@usage   "python network_statistics.py network-file-to-be-analyzed.net.xml"

Prints some information about a given network

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

if len(sys.argv) < 2:
    print("No net-file given. Usage: " + sys.argv[0] + " <network file>.")
    sys.exit()
print("Reading network ...\n")
net = sumolib.net.readNet(sys.argv[1])

# ----------
narea = (net._ranges[0][1] - net._ranges[0][0]) * \
    (net._ranges[1][1] - net._ranges[1][0])
if narea < 1000 * 1000:
    print("area of network:\t" + str(round(narea, 2)) + " m²" + "\n")
else:
    print("area of network:\t" +
          str(round(narea / 1000000, 2)) + " km²" + "\n")
print("total number of edges:\t" + str(len(net._edges)) + "\n")
# ----------
lengthOfEdgeSum = .0
lane_1 = 0
lane_2 = 0
lane_3 = 0
lane_4 = 0
for n in net._edges:
    lengthOfEdgeSum = lengthOfEdgeSum + n._length
    if len(n._lanes) == 1:
        lane_1 = lane_1 + 1
    elif len(n._lanes) == 2:
        lane_2 = lane_2 + 1
    elif len(n._lanes) == 3:
        lane_3 = lane_3 + 1
    else:
        lane_4 = lane_4 + 1
if lengthOfEdgeSum < 10000:
    print("total length of all edges:\t" +
          str(round(lengthOfEdgeSum, 2)) + " m" + "\n")
else:
    print("total length of all edges:\t" +
          str(round(lengthOfEdgeSum / 1000, 2)) + " km" + "\n")
print("number of edges with 1 lane:\t" + str(lane_1) + "\n")
print("number of edges with 2 lanes:\t" + str(lane_2) + "\n")
print("number of edges with 3 lanes:\t" + str(lane_3) + "\n")
print("number of edges with 4 or more lanes:\t" + str(lane_4) + "\n")
print("total number of nodes:\t" + str(len(net._nodes)) + "\n")
