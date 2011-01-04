#!/usr/bin/env python
"""
@file    netstats.py
@author  Daniel.Krajzewicz@dlr.de 
@date    2008-08-13
@version $Id$

Prints some information about a given network

Copyright (C) 2009-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


def renderHTML(values):
    print "<html><body>"
    print "<h1>" + values["netname"] + "</h1></br>"
    # network
    print "<h2>Network</h2></br>"
    # edges
    print "<h2>Edges</h2></br>"
    print "Edge number: " + str(values["edgeNumber"]) + "</br>"
    print "Edgelength sum: " + str(values["edgeLengthSum"]) + "</br>"
    print "Lanelength sum: " + str(values["laneLengthSum"]) + "</br>"
    # nodes
    print "<h2>Nodes</h2></br>"
    print "Node number: " + str(values["nodeNumber"]) + "</br>"
    print "</body></html>"


if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net = net.getNet()

values = {}
values["netname"] = "hallo"
values["edgesPerLaneNumber"] = {}
values["edgeLengthSum"] = 0
values["laneLengthSum"] = 0
values["edgeNumber"] = len(net._edges)
values["nodeNumber"] = len(net._nodes)
for e in net._edges:
    values["edgeLengthSum"] = values["edgeLengthSum"] + e._length
    values["laneLengthSum"] = values["laneLengthSum"] + (e._length * float(len(e._lanes)))
    if len(e._lanes) not in values["edgesPerLaneNumber"]:
        values["edgesPerLaneNumber"][len(e._lanes)] = 0
    values["edgesPerLaneNumber"][len(e._lanes)] = values["edgesPerLaneNumber"][len(e._lanes)] + 1

renderHTML(values)


