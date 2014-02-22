#!/usr/bin/env python
"""
@file    netextract.py
@author  Daniel Krajzewicz
@author  Laura Bieker
@author  Michael Behrisch
@date    2007-02-21
@version $Id$

This script reads in the network given as
 first parameter and extracts nodes and edges 
 from it which are saved into "nodes.xml" and 
 "edges.xml" for their reuse in NETCONVERT

todo:
- parse connections
- parse tls information

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net


def writeNodes(net):
    """ Writes nodes in a xml file """
    
    fd = open("nodes.xml", "w")
    fd.write("<nodes>\n")
    for node in net._nodes:
        fd.write("   <node id=\"" + node._id + "\" x=\"" + str(node._coord[0]) + "\" y=\"" + str(node._coord[1]) + "\"/>\n")
    fd.write("</nodes>\n")

def writeEdges(net):
    """ Writes edges in a xml file """

    fd = open("edges.xml", "w")
    fd.write("<edges>\n")
    for edge in net._edges:
        fd.write("   <edge id=\"" + edge._id + "\" from=\"" + edge._from._id + "\" to=\"" + edge._to._id)
        fd.write("\" speed=\"" + str(edge._speed))
        fd.write("\" priority=\"" + str(edge._priority))
        fd.write("\" spreadType=\"center")
        fd.write("\" numLanes=\"" + str(len(edge._lanes)) + "\"")
        shape = edge.getShape()
        fd.write(" shape=\"")
        for i,c in enumerate(shape):
            if i!=0:
                fd.write(" ")
            fd.write(str(c[0]) + "," + str(c[1]))
        fd.write("\"")
        fd.write("/>\n")
    fd.write("</edges>\n")


    
if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
print "Reading net..."
net = sumolib.net.readNet(sys.argv[1])
print "Writing nodes..."
writeNodes(net)
print "Writing edges..."
writeEdges(net)
