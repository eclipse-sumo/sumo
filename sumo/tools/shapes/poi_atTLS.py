#!/usr/bin/env python
"""
@file    poi_atTLS.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-02-20
@version $Id$

Generates a PoI-file containing a PoI for each tls controlled intersection
 from the given net.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib.net


if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " <NET>", file=sys.stderr)
    sys.exit()

print("Reading net...")
net1 = sumolib.net.readNet(sys.argv[1], withPrograms=True)


print("Writing output...")
fdo = open('pois.add.xml', 'w')
print('<?xml version="1.0"?>', file=fdo)
print('<!-- poi_atTLS %s -->' % sys.argv, file=fdo)
print('<additional>', file=fdo)
for tlsID in net1._id2tls:
    tls = net1._id2tls[tlsID]
    nodes = set()
    for c in tls._connections:
        iLane = c[0]
        iEdge = iLane.getEdge()
        nodes.add(iEdge._to)
    if len(sys.argv) > 2 and sys.argv[2] != "nojoin":
        c = [0, 0]
        for n in nodes:
            c[0] += n._coord[0]
            c[1] += n._coord[1]
        if len(nodes) > 1:
            c[0] = c[0] / float(len(nodes))
            c[1] = c[1] / float(len(nodes))
        print('    <poi id="%s" type="default" color="1,0,0" layer="0" x="%s" y="%s"/>' % (
            tlsID, c[0], c[1]), file=fdo)
    else:
        for n in nodes:
            print('    <poi id="%s_at_%s" type="default" color="1,0,0" layer="0" x="%s" y="%s"/>' % (
                tlsID, n._id, n._coord[0], n._coord[1]), file=fdo)

print('</additional>', file=fdo)
fdo.close()
