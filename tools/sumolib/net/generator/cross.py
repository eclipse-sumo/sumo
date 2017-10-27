#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    cross.py
# @author  Daniel Krajzewicz
# @date    2013-10-10
# @version $Id$

from __future__ import absolute_import
import os
import sys
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(__file__), '..', '..', '..', '..')), 'tools'))

from . import network


def cross(defaultNode=None, defaultEdge=None, centralReservation=0):
    net = network.Net(defaultNode, defaultEdge)
    net.addNode(network.Node("1/0", 500, 0, "priority"))
    net.addNode(network.Node("0/1", 0, 500, "priority"))
    net.addNode(network.Node("1/1", 500, 500, "traffic_light"))
    net.addNode(network.Node("2/1", 1000, 500, "priority"))
    net.addNode(network.Node("1/2", 500, 1000, "priority"))
    net.connectNodes("1/0", "1/1", True, centralReservation)
    net.connectNodes("0/1", "1/1", True, centralReservation)
    net.connectNodes("2/1", "1/1", True, centralReservation)
    net.connectNodes("1/2", "1/1", True, centralReservation)
    return net

if __name__ == "__main__":
    net = cross()
    net.build()
