#!/usr/bin/env python
"""
@file    createRoundaboutConnections.py
@author  Laura Bieker
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2007-02-21
@version $Id$

This script reads in the network given as
 first parameter and generates additional connections for roundabouts.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

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


def writeConnections(net):
    fd = open("roundabout-connection.con.xml", "w")
    fd.write("<connections>\n")
    for ra in net.getRoundabouts():
        for node in ra.getNodes():
            edgesOut = net.getNode(node)._outgoing
            edgesIn = net.getNode(node)._incoming
            for edgeOut in edgesOut:
                outNumber = edgeOut.getLaneNumber()
                for edgeIn in edgesIn:
                    if not edgeOut in edgeIn._outgoing:
                        continue
                    inNumber = edgeIn.getLaneNumber()
                    for x in range(inNumber):
                        if x < inNumber and x < outNumber:
                            fd.write("   <connection from=\"" + str(edgeIn._id) + "\" to=\"" + str(
                                edgeOut._id) + "\" lane=\"" + str(x) + ":" + str(x) + "\" />\n")
    fd.write("</connections>\n")


if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " <net>")
    sys.exit()
print("Reading net...")
net = sumolib.net.readNet(sys.argv[1])
print("Writing connections...")
writeConnections(net)
