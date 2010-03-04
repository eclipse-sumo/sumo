#!/usr/bin/env python
"""
@file    deleteUnusedDetektors.py
@author  Laura.Bieker@dlr.de
@date    2010-03-03

This script reads a network as first parameter and a file with the positions
of detectors as second parameter. As result the script writes a new file
with detectors which are actually used in the given network. 

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler
from xml.dom.minidom import *

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

def writeDetectors(usedDet):
    """ Writes the detectors which are used in the network in a xml file. """

    fd = open("detectors.xml", "w")
    fd.write("<pois>\n")
    for det in usedDet:
        fd.write("   <e1-detector id= \"" + det.getAttribute('id') + "\" lane=\"" + det.getAttribute('lane') + "\" pos=\"" + det.getAttribute('pos'))
        fd.write("\" freq=\"" + det.getAttribute('freq') + "\" file=\"" + det.getAttribute('file'))
        # the position of some detectors are over the end of an edge.
        # the tag friendly_pos corrects the position
        if det.hasAttribute('friendly_pos'):
            fd.write("\" friendly_pos=\"" + det.getAttribute('friendly_pos'))
        
        fd.write("\"")
        fd.write("/>\n")
    fd.write("</pois>\n")
    

def deleteUnused(detectors, net):
    """ This method gets all edges from a network and tests whether
the detectors are used in the network. Returns: all detectors which are located
in an edge from the network. """

    detAttributes= detectors.getElementsByTagName('e1-detector')
    edgelist= []
    # get all edges from the network
    for edge in net._edges:
        edgelist.append(str(edge._id))
    usedDet= []
    # get all detectors which are located inside the network
    for i in detAttributes:
        laneID= i.getAttribute('lane')
        # the id from lanes have as preffix the id from edges
        # the last two characters are not important they describe the position within the edge
        if laneID[: len(laneID)-2] in edgelist:
            usedDet.append(i)
    return usedDet 


if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " <net> <detectors>"
    sys.exit()
    
print "Reading net..."
parser = make_parser()
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])

print "Reading detectors..."
detectors= parse(sys.argv[2])
detectors= deleteUnused(detectors, net.getNet())
writeDetectors(detectors)


