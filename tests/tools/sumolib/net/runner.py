#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Jakob Erdmann
# @date

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.net  # noqa


def printSorted(d):
    return ' '.join(["%s:%s" % (k, d[k]) for k in sorted(d.keys())])


net = sumolib.net.readNet(sys.argv[1], withInternal=True, withLatestPrograms=True)
lane = net.getLane("SC_0")
lane2 = net.getLane("CN_0")
print("lanes to %s: %s" % (lane2.getID(), ' '.join(sorted([l.getID() for l in lane2.getIncoming()]))))

print("connections from %s:\n%s" % (
    lane.getID(),
    '\n'.join(list(map(str, lane.getOutgoing())))))
print("outgoing internal lanes of %s: %s" % (
    lane.getID(), ' '.join([net.getLane(c.getViaLaneID()).getID() for c in lane.getOutgoing()])))

internal_edge = net.getEdge(":C_0")
internal_lane = net.getLane(":C_0_0")
internal_lane_cons = internal_lane.getOutgoing()
print("connections from %s:\n%s" % (internal_lane.getID(),
                                    '\n'.join(map(str, internal_lane_cons))))
internal_lane_incoming = sorted(internal_lane.getIncoming())
print("lanes to %s: %s" % (internal_lane.getID(),
                           ' '.join([l.getID() for l in internal_lane_incoming])))
assert(internal_edge.getFunction() == 'internal')
assert(internal_edge.isSpecial())
assert(internal_lane.getEdge().isSpecial())
assert(internal_edge.getFromNode().getID() == "C")
assert(internal_edge.getToNode().getID() == "C")

# params
print("edgeParams",     printSorted(net.getEdge("CE").getParams()))
print("laneParams",     printSorted(net.getLane("CE_0").getParams()))
print("laneParams",     printSorted(net.getLane("CE_1").getParams()))
print("junctionParams", printSorted(net.getNode("C").getParams()))
print("tlsParams",      printSorted(net.getTLS("C").getPrograms()["0"].getParams()))

# functions
print("getNeighboringEdges", ' '.join(sorted([e.getID() for e, d in net.getNeighboringEdges(100, 0, 10)])))
print("getNeighboringLanes", ' '.join(sorted([l.getID() for l, d in net.getNeighboringLanes(100, 0, 10)])))
print("getNeighboringNodes only for incoming edges",
      ' '.join(sorted([n.getID() for n in net.getNode("N").getNeighboringNodes(False, True)])))
print("getNeighboringNodes only for outgoing edges",
      ' '.join(sorted([n.getID() for n in net.getNode("S").getNeighboringNodes(True, False)])))
print("getNeighboringNodes", ' '.join(sorted([n.getID() for n in net.getNode("C").getNeighboringNodes()])))
