#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @author  Matthias Schwamborn
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
import sumolib  # noqa

net = sumolib.net.readNet(sys.argv[1], withInternal=True, withLatestPrograms=True)
SC = net.getEdge("SC")
CE = net.getEdge("CE")
print("%s %.2f" % net.getFastestPath(SC, CE))
route = net.getFastestPath(SC, CE, vClass="passenger")
print("%s %.2f" % route)
print("%s %.2f" % net.getFastestPath(SC, CE, vClass="bus"))
print("%s %.2f" % net.getFastestPath(SC, CE, withInternal=True))
route2 = net.getFastestPath(SC, CE, vClass="passenger", withInternal=True)
print("%s %.2f" % route2)
routeA = tuple(sumolib.route.addInternal(net, route[0]))
print(routeA, routeA == route2[0])
print("%s %.2f" % net.getFastestPath(net.getEdge(":C_12"), CE))
print("%s %.2f" % net.getFastestPath(net.getEdge(":C_22"), CE))
print("%s %.2f" % net.getFastestPath(SC, net.getEdge(":C_15")))
print("%s %.2f" % net.getFastestPath(net.getEdge(":C_12"), net.getEdge(":C_15")))
print("%s %.2f" % net.getFastestPath(CE, SC))
print("%s %.2f" % net.getFastestPath(CE, SC, ignoreDirection=True))

net = sumolib.net.readNet(sys.argv[2], withInternal=True, withLatestPrograms=True)
print("%s %.2f" % net.getFastestPath(net.getEdge("beg"), net.getEdge("rend")))
print("%s %.2f" % net.getFastestPath(net.getEdge("rend"), net.getEdge("beg"), ignoreDirection=True))
print("%s %.2f" % net.getFastestPath(net.getEdge("rend"), net.getEdge("beg")))
print("%s %.2f" % net.getFastestPath(net.getEdge("beg2left"), net.getEdge("rend")))
print("%s %.2f" % net.getFastestPath(net.getEdge("rend"), net.getEdge("beg2left"), ignoreDirection=True))
