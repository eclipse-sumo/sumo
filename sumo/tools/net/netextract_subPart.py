#!/usr/bin/env python
"""
@file    netextract_subPart.py
@author  Daniel.Krajzewicz@dlr.de
@date    2007-02-21
@version $Id$


This script reads in the network given as first parameter.
It extracts nodes given on the command line as well as the edges 
 that participate in these nodes and the nodes these edges
 start/end at. Write the so obtained nodes and edges
 into "<prefix>_nodes.nod.xml" and "<prefix>_edges.edg.xml" 
 for their reuse in NETCONVERT

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


def writeNodes(net, file):
    fd = open(file, "w")
    fd.write("<nodes>\n")
    for node in net._nodes:
        fd.write("   <node id=\"" + node._id + "\" x=\"" + str(node._coord[0]) + "\" y=\"" + str(node._coord[1]) + "\"/>\n")
    fd.write("</nodes>\n")

def writeEdges(net, file):
    fd = open(file, "w")
    fd.write("<edges>\n")
    for edge in net._edges:
        fd.write("   <edge id=\"" + edge._id + "\" fromnode=\"" + edge._from._id + "\" tonode=\"" + edge._to._id)
        fd.write("\" speed=\"" + str(edge._speed))
        fd.write("\" priority=\"" + str(edge._priority))
        fd.write("\" spread_type=\"center")
        fd.write("\" nolanes=\"" + str(len(edge._lanes)) + "\"")
        shape = edge.getShape()
        fd.write(" shape=\"")
        for i,c in enumerate(shape):
            if i!=0:
                fd.write(" ")
            fd.write(str(c[0]) + "," + str(c[1]))
        fd.write("\"")
        fd.write("/>\n")
    fd.write("</edges>\n")


    
if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <net> <prefix> <nodes>"
    sys.exit()
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])

edges = set()
nodes = set()
for n in sys.argv[3].split(","):
    n = net.getNet().getNode(n)
    nodes.add(n)
    for e in n._incoming:
        edges.add(e)
        nodes.add(e._from)
    for e in n._outgoing:
        edges.add(e)
        nodes.add(e._to)
net = sumonet.Net()
for e in edges:
    c = net.addEdge(e._id, e._from._id, e._to._id, e._priority, e._function)
    for l in e._lanes:
        lane = sumonet.NetLane(c, l.getSpeed(), l.getLength())
        lane.setShape(l.getShape())
    c.rebuildShape()
for n in nodes:
    net.addNode(n._id, n._coord)

print "Writing nodes..."
writeNodes(net, sys.argv[2]+"_nodes.nod.xml")
print "Writing edges..."
writeEdges(net, sys.argv[2]+"_edges.edg.xml")

