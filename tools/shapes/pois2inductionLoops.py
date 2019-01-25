#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    pois2inductionLoops.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2010-02-18
# @version $Id$

"""

Converts a given pois located on lanes into induction loop detectors;
Each poi is replicated to cover all lanes of the road.
The detectors are named <POINAME>__l<LANE_INDEX>

Call: pois2inductionLoops.py <NET> <POIS> <OUTPUT>
"""
from __future__ import absolute_import
from __future__ import print_function


import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


if len(sys.argv) < 4:
    print("Usage: " + sys.argv[0] + " <NET> <POIS> <OUTPUT>")
    sys.exit()

print("Reading net...")
net = sumolib.net.readNet(sys.argv[1])

print("Reading PoIs...")
pois = sumolib.poi.readPois(sys.argv[2])

fdo = open(sys.argv[3], "w")
print('<additional>', file=fdo)
for poi in pois:
    if not poi._lane:
        print("Error: poi '%s' is not on a lane" % poi._id)
        continue
    edge = poi._lane[:poi._lane.rfind('_')]
    edge = net._id2edge[edge]
    for i, l in enumerate(edge._lanes):
        print('    <e1Detector id="%s__l%s" lane="%s" pos="%s" freq="60" file="e1_output.xml"/>' % (
            poi._id, i, l.getID(), poi._pos), file=fdo)
    print('', file=fdo)
print('</additional>', file=fdo)
fdo.close()
