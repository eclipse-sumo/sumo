#!/usr/bin/env python
"""
@file    netstats.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-08-13
@version $Id$

Prints some information about a given network

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os, string, sys, StringIO
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net


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

def renderPNG(values):
    from matplotlib import rcParams
    from pylab import *
    bar([0], [values["edgeNumber"]], 1, color='r')
    show()


if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
print "Reading net..."
net = sumolib.net.readNet(sys.argv[1])

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
renderPNG(values)
