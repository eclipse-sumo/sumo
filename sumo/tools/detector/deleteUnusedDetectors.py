#!/usr/bin/env python
"""
@file    deleteUnusedDetektors.py
@author  Laura Bieker
@author  Michael Behrisch
@date    2010-03-03
@version $Id$

This script reads a network as first parameter and a file with the positions
of detectors as second parameter. As result the script writes a new file
with detectors which are actually used in the given network. 

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2010-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""


import os, string, sys, StringIO
from xml.dom.minidom import *
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net

def writeDetectors(usedDet):
    """ Writes the detectors which are used in the network in a xml file. """

    fd = open("detectors.xml", "w")
    fd.write('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/additional_file.xsd">\n')
    for det in usedDet:
        fd.write("   <e1Detector id= \"" + det.getAttribute('id') + "\" lane=\"" + det.getAttribute('lane') + "\" pos=\"" + det.getAttribute('pos'))
        fd.write("\" freq=\"" + det.getAttribute('freq') + "\" file=\"" + det.getAttribute('file'))
        # the position of some detectors are over the end of an edge.
        # the tag friendlyPos corrects the position
        if det.hasAttribute('friendlyPos'):
            fd.write("\" friendlyPos=\"" + det.getAttribute('friendlyPos'))
        
        fd.write("\"")
        fd.write("/>\n")
    fd.write("</additional>\n")
    

def deleteUnused(detectors, net):
    """ This method gets all edges from a network and tests whether
the detectors are used in the network. Returns: all detectors which are located
in an edge from the network. """

    detAttributes = detectors.getElementsByTagName('e1-detector')
    edgelist = []
    # get all edges from the network
    for edge in net._edges:
        edgelist.append(str(edge._id))
    usedDet = []
    # get all detectors which are located inside the network
    for i in detAttributes:
        laneID = i.getAttribute('lane')
        # the id from lanes have as preffix the id from edges
        # the last two characters are not important they describe the position within the edge
        if laneID[: len(laneID)-2] in edgelist:
            usedDet.append(i)
    return usedDet 


if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <net> <detectors>"
    sys.exit()
    
print "Reading net..."
net = sumolib.net.readNet(sys.argv[1])

print "Reading detectors..."
detectors = parse(sys.argv[2])
detectors = deleteUnused(detectors, net)
writeDetectors(detectors)
