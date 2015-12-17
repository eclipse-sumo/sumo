#!/usr/bin/env python
"""
A script for building grid-like networks.

@file    grid.py
@author  Daniel Krajzewicz
@date    2013-10-10
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(__file__), '..', '..', '..', '..')), 'tools'))

from . import network
from . import demand


def grid(numIntersectionsX=10, numIntersectionsY=5, defaultNode=None, defaultEdge=None, centralReservation=0):
    net = network.Net(defaultNode, defaultEdge)
    for x in range(0, numIntersectionsX):
        net.addNode(
            network.Node(str(x + 1) + "/0", (x + 1) * 500, 0, "priority"))
        net.addNode(network.Node(str(x + 1) + "/" + str(numIntersectionsY + 1),
                                 (x + 1) * 500, (numIntersectionsY + 1) * 500, "priority"))
    for y in range(0, numIntersectionsY):
        net.addNode(
            network.Node("0/" + str(y + 1), 0, (y + 1) * 500, "priority"))
        net.addNode(network.Node(str(numIntersectionsX + 1) + "/" +
                                 str(y + 1), (numIntersectionsX + 1) * 500, (y + 1) * 500, "priority"))
    for x in range(0, numIntersectionsX):
        for y in range(0, numIntersectionsY):
            net.addNode(network.Node(
                str(x + 1) + "/" + str(y + 1), (x + 1) * 500, (y + 1) * 500, "traffic_light"))
    for x in range(0, numIntersectionsX):
        for y in range(0, numIntersectionsY):
            net.connectNodes(str(
                x) + "/" + str(y + 1), str(x + 1) + "/" + str(y + 1), True, centralReservation)
            net.connectNodes(str(
                x + 1) + "/" + str(y), str(x + 1) + "/" + str(y + 1), True, centralReservation)
    for x in range(0, numIntersectionsX):
        net.connectNodes(str(x + 1) + "/" + str(numIntersectionsY),
                         str(x + 1) + "/" + str(numIntersectionsY + 1), True, centralReservation)
    for y in range(0, numIntersectionsY):
        net.connectNodes(str(numIntersectionsX) + "/" + str(y + 1),
                         str(numIntersectionsX + 1) + "/" + str(y + 1), True, centralReservation)
    return net
#  d = demand.Demand()
#  d.addStream(demand.Stream("1/0_to_1/2", 10, "1/0 1/2"))
#  d.build(3600)

if __name__ == "__main__":
    net = grid()
    net.build()
