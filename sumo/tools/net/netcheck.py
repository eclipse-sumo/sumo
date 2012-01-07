#!/usr/bin/env python
"""
@file    netcheck.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@author  Laura Bieker
@date    2007-03-20
@version $Id$

This script does simple check for the network.
It tests whether the network is (weakly) connected.
It needs one parameter, the SUMO net (.net.xml).

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2012 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

def getWeaklyConnected(net):
    components = []
    edgesLeft = set(net.getEdges())
    queue = list()
    while len(edgesLeft) != 0:
        component = set()
        queue.append(edgesLeft.pop())
        while not len(queue) == 0:
            edge = queue.pop(0)
            component.add(edge.getID())
            for n in edge.getOutgoing().iterkeys():
                if n in edgesLeft:
                    queue.append(n)
                    edgesLeft.remove(n)
            for n in edge.getIncoming().iterkeys():
                if n in edgesLeft:
                    queue.append(n)
                    edgesLeft.remove(n)
        components.append(component)
    return components


if len(sys.argv) != 2:
    print "Usage: " + sys.argv[0] + " <net>"
    sys.exit()
net = sumolib.net.readNet(sys.argv[1])
components = getWeaklyConnected(net)
if len(components) != 1:
    print "Warning! Net is not connected."
    for idx, comp in enumerate(components):
        print "Component", idx
        print " ".join(comp)
        print
