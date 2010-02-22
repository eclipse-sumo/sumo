#!/usr/bin/env python
"""
@file    poi_atTLS.py
@author  Daniel.Krajzewicz@dlr.de
@date    2010-02-20
@version $Id: poi_atTLS.py 8236 2010-02-10 11:16:41Z behrisch $

Generates a PoI-file containing a PoI for each tls controlled intersection
 from the given net.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""

import sys, os
from xml.sax import saxutils, make_parser, handler

sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


if len(sys.argv)<2:
    print >> sys.stderr, "Usage: " + sys.argv[0] + " <NET>"
    sys.exit()

print("Reading net...")
parser = make_parser()
net1 = sumonet.NetReader(withPrograms=True)
parser.setContentHandler(net1)
parser.parse(sys.argv[1])
net1 = net1.getNet()


print("Writing output...")
fdo = open('pois.add.xml', 'w')
print >> fdo, '<?xml version="1.0"?>'
print >> fdo, '<!-- poi_atTLS %s -->' % sys.argv[1]
print >> fdo, '<additional>'
for tlsID in net1._id2tls:
    tls = net1._id2tls[tlsID]
    nodes = set()
    for c in tls._connections:
        iLane = c[0]
        iEdge = iLane.getEdge()
        nodes.add(iEdge._to)
    c = [0, 0]
    for n in nodes:
        c[0] += n._coord[0]
        c[1] += n._coord[1]
    if len(nodes)>1:
        c[0] = c[0] / float(len(nodes))
        c[1] = c[1] / float(len(nodes))
    print >> fdo, '    <poi id="%s" type="default" color="1,0,0" layer="0" x="%s" y="%s"/>' % (tlsID, c[0], c[1])
print >> fdo, '</additional>'
fdo.close()
