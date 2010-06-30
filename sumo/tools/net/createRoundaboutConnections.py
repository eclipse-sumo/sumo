#!/usr/bin/env python
"""
@file    createRoundaboutConnections.py
@author  Laura.Bieker@dlr.de
@date    2007-02-21
@version $Id: createRoundaboutConnections.py 8378 2010-03-04 14:34:06Z bieker $

This script reads in the network given as
 first parameter and generates additional connections for roundabouts.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

def writeConnections(net):
    fd = open("roundabout-connection.con.xml", "w")
    fd.write("<connections>\n")
    for ra in net.getRoundabouts():
        for node in ra.getNodes():
            edgesOut= net.getNode(node)._outgoing
            edgesIn= net.getNode(node)._incoming
            for edgeOut in edgesOut:
                outNumber= edgeOut.getLaneNumber() 
                for edgeIn in edgesIn:
                    inNumber= edgeIn.getLaneNumber() 
                    for x in range(inNumber):
                        if x < inNumber and x < outNumber:
                            fd.write("   <connection from=\"" +str(edgeIn._id)+ "\" to=\"" + str(edgeOut._id)+ "\" lane=\""+ str(x) +":"+ str(x) +"\" />\n")
    fd.write("</connections>\n")

    
if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
print "Writing connections..."
writeConnections(net.getNet())