#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib.net

net = sumolib.net.readNet(sys.argv[1], withInternal=True)
lane = net.getLane("SC_0")


print("connections from %s:\n%s" % (
    lane.getID(), 
    '\n'.join(list(map(str, lane.getOutgoing())))))
print("outgoing internal lanes of %s: %s" % (
    lane.getID(), [net.getLane(c.getViaLaneID()).getID() for c in lane.getOutgoing()]))

internal_edge = net.getEdge(":C_0")
internal_lane = net.getLane(":C_0_0")
internal_lane_cons = internal_lane.getOutgoing()
print("connections from %s:\n%s" % (internal_lane.getID(),
    '\n'.join(map(str, internal_lane_cons))))
assert(internal_edge.getFunction() == 'internal')
assert(internal_edge.isSpecial())
assert(internal_lane.getEdge().isSpecial())
assert(internal_edge.getFromNode().getID() == "C")
assert(internal_edge.getToNode().getID() == "C")
