#!/usr/bin/env python
"""
@file    pois2inductionLoops.py
@author  Daniel.Krajzewicz@dlr.de
@date    2010-02-18
@version $Id$


Converts a given pois located on lanes into induction loop detectors;
Each poi is replicated to cover all lanes of the road.
The detectors are named <POINAME>__l<LANE_INDEX>

Call: pois2inductionLoops.py <NET> <POIS> <OUTPUT>

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""


import os, string, sys, StringIO
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet
import sumopoi


if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " <NET> <POIS> <OUTPUT>"
    sys.exit()
parser = make_parser()
print "Reading net..."
net = sumonet.NetReader()
parser.setContentHandler(net)
parser.parse(sys.argv[1])
net = net.getNet()

print "Reading PoIs..."
pois = sumopoi.PoIReader()
parser.setContentHandler(pois)
parser.parse(sys.argv[2])
pois = pois._pois

fdo = open(sys.argv[3], "w")
print >> fdo, '<additional>'
for poi in pois:
    if not poi._lane:
        print "Error: poi '%s' is not on a lane" % poi._id
        continue
    edge = poi._lane[:poi._lane.rfind('_')]
    edge = net._id2edge[edge]
    for i, l in enumerate(edge._lanes):
        print >> fdo, '    <e1-detector id="%s__l%s" lane="%s" pos="%s" freq="60" file="e1_output.xml"/>' % (poi._id, i, l.getID(), poi._pos)
    print >> fdo, ''
print >> fdo, '</additional>'
fdo.close()


