#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
print("version:", net.getVersion())
lane = net.getLane("SC_0")
lane2 = net.getLane("CN_0")
print("lanes to %s: %s" % (lane2.getID(), ' '.join(sorted([li.getID() for li in lane2.getIncoming()]))))

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
                           ' '.join([li.getID() for li in internal_lane_incoming])))

node = net.getNode("C")

toLane = net.getLane("CE_0")
connection = lane.getConnection(toLane)
assert connection.getFrom() is lane.getEdge()
assert connection.getTo() is toLane.getEdge()
assert connection in connection.getTo().getIncoming()[connection.getFrom()]
assert connection.getJunction() is node and connection.getJunctionIndex() == 9
viaLane = net.getLane(connection.getViaLaneID())
viaConnection = viaLane.getIncomingConnections()[0]
assert viaLane.getID() == ":C_9_0"
assert viaConnection.getFromLane() is lane and viaConnection.getToLane() is viaLane
assert viaConnection.getFrom() is lane.getEdge() and viaConnection.getTo() is viaLane.getEdge()
assert viaConnection.getViaLaneID() == ""

withoutInternal = sumolib.net.readNet(sys.argv[1])
plain = withoutInternal.getLane("SC_0").getConnection(withoutInternal.getLane("CE_0"))
assert plain.getFrom().getID() == "SC" and plain.getTo().getID() == "CE"
assert plain.getViaLaneID() == ":C_9_0" and plain.getJunctionIndex() == 9

legacy = sumolib.net.connection.Connection(
    fromEdge=connection.getFrom(), toEdge=connection.getTo(),
    fromLane=lane, toLane=toLane, direction=connection.getDirection(),
    tls="C", tllink=9, tllink2=-1, allow=None, disallow=None, state=connection.getState(),
    viaLaneID=viaLane.getID())
assert str(legacy) == str(connection)
assert {"_from", "_to"}.isdisjoint(vars(legacy))

assert internal_edge.getFunction() == 'internal'
assert internal_edge.isSpecial()
assert internal_lane.getEdge().isSpecial()
assert internal_edge.getFromNode().getID() == "C"
assert internal_edge.getToNode().getID() == "C"
assert node.getTLSID() == "C"

# params
print("edgeParams",     printSorted(net.getEdge("CE").getParams()))
print("laneParams",     printSorted(net.getLane("CE_0").getParams()))
print("laneParams",     printSorted(net.getLane("CE_1").getParams()))
print("junctionParams", printSorted(net.getNode("C").getParams()))
print("tlsParams",      printSorted(net.getTLS("C").getPrograms()["0"].getParams()))

# functions
print("getNeighboringEdges", ' '.join(sorted([e.getID() for e, _ in net.getNeighboringEdges(100, 0, 10)])))
print("getNeighboringLanes", ' '.join(sorted([ln.getID() for ln, _ in net.getNeighboringLanes(100, 0, 10)])))
print("getNeighboringNodes only for incoming edges",
      ' '.join(sorted([n.getID() for n in net.getNode("N").getNeighboringNodes(False, True)])))
print("getNeighboringNodes only for outgoing edges",
      ' '.join(sorted([n.getID() for n in net.getNode("S").getNeighboringNodes(True, False)])))
print("getNeighboringNodes", ' '.join(sorted([n.getID() for n in net.getNode("C").getNeighboringNodes()])))
