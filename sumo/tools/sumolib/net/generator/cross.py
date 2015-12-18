#!/usr/bin/env python
"""
A script for building simple crosses networks.

@file    cross.py
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
