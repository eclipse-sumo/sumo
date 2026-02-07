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
import sumolib  # noqa

net = sumolib.net.readNet(sys.argv[1], withInternal=True, withLatestPrograms=True)
SC = net.getEdge("SC")
CN = net.getEdge("CN")
CE = net.getEdge("CE")
EC = net.getEdge("EC")
print("%s %.2f" % net.getShortestPath(SC, CN, vClass="passenger"))
print("%s %.2f" % net.getShortestPath(SC, CN, vClass="passenger", withInternal=True))
print("%s %.2f" % net.getShortestPath(SC, CN, ignoreDirection=True))
print("%s %.2f" % net.getShortestPath(SC, CN, withInternal=True, ignoreDirection=True))
print()
print("%s %.2f" % net.getShortestPath(SC, CE, vClass="passenger"))
print("%s %.2f" % net.getShortestPath(SC, CE, vClass="passenger", withInternal=True))
print("%s %.2f" % net.getShortestPath(SC, CE, ignoreDirection=True))
print("%s %.2f" % net.getShortestPath(SC, CE, withInternal=True, ignoreDirection=True))
print()
print("%s %.2f" % net.getShortestPath(SC, EC, vClass="passenger"))
print("%s %.2f" % net.getShortestPath(SC, EC, vClass="passenger", withInternal=True))
print("%s %.2f" % net.getShortestPath(SC, EC, ignoreDirection=True))
print("%s %.2f" % net.getShortestPath(SC, EC, withInternal=True, ignoreDirection=True))
print()
print("%s %.2f" % net.getShortestPath(CE, SC, vClass="passenger"))
print("%s %.2f" % net.getShortestPath(CE, SC, vClass="passenger", withInternal=True))
print("%s %.2f" % net.getShortestPath(CE, SC, ignoreDirection=True))
print("%s %.2f" % net.getShortestPath(CE, SC, withInternal=True, ignoreDirection=True))
