#!/usr/bin/env python
"""
@file    netextract.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-02-21
@version $Id: $


This script reads in the network given as
 first parameter and extracts nodes and edges 
 from it which are saved into "nodes.xml" and 
 "edges.xml" for their reuse in NETCONVERT

todo:
- parse connections
- parse tls information

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


def writeNodes(net):
    fd = open("nodes.xml", "w")
    fd.write("<nodes>\n")
    for node in net._nodes:
        fd.write("   <node id=\"" + node._id + "\" x=\"" + str(node._coord[0]) + "\" y=\"" + str(node._coord[0]) + "\"/>\n")
    fd.write("</nodes>\n")

def writeEdges(net):
    fd = open("edges.xml", "w")
    fd.write("<edges>\n")
    for edge in net._edges:
        fd.write("   <edge id=\"" + edge._id + "\" fromnode=\"" + edge._from._id + "\" tonode=\"" + edge._to._id)
        fd.write("\" speed=\"" + str(edge._speed))
        fd.write("\" priority=\"" + str(edge._priority))
        fd.write("\" nolanes=\"" + str(len(edge._lanes)) + "\"")
#        if self._edge2shape[edge]!="":
#           fd.write(" shape=\"" + self._edge2shape[edge] + "\"")
        fd.write("/>\n")
    fd.write("</edges>\n")


    
if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
print "Writing nodes..."
writeNodes(net.getNet())
print "Writing edges..."
writeEdges(net.getNet())

