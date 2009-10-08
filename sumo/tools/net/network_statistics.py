#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
@file    network_statistics.py
@author  Ronald.Nippold@dlr.de
@date    20089-10-06
@version
@usage   "python network_statistics.py network-file-to-be-analyzed.net.xml"

Prints some information about a given network

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

if len(sys.argv) < 2:
	print "No net-file given. Usage: " + sys.argv[0] + " <network file>."
	sys.exit()
print("Reading network ...\n")
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net = net.getNet()

# ----------
narea = (net._ranges[0][1] - net._ranges[0][0]) * (net._ranges[1][1] - net._ranges[1][0])
if narea < 1000 * 1000:
	print("area of network:\t" + str(round(narea,2)) + " m²" + "\n")
else:
	print("area of network:\t" + str(round(narea/1000000,2)) + " km²" + "\n")
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
	print("total length of all edges:\t" + str(round(lengthOfEdgeSum,2)) + " m" + "\n")
else:
	print("total length of all edges:\t" + str(round(lengthOfEdgeSum/1000,2)) + " km" + "\n")
print("number of edges with 1 lane:\t" + str(lane_1) + "\n")
print("number of edges with 2 lanes:\t" + str(lane_2) + "\n")
print("number of edges with 3 lanes:\t" + str(lane_3) + "\n")
print("number of edges with 4 or more lanes:\t" + str(lane_4) + "\n")
print("total number of nodes:\t" + str(len(net._nodes)) + "\n")
