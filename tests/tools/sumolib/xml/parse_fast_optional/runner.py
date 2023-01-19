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
# @author  Michael Behrisch
# @date    2023-01-18

from __future__ import print_function
import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib  # noqa

for f in sumolib.xml.parse_fast(sys.argv[1], "edge", ["id", "sampledSeconds", "traveltime", "overlapTraveltime",
                                                      "density", "laneDensity", "occupancy", "waitingTime",
                                                      "timeLoss", "speed", "speedRelative", "departed"], optional=True):
    print(f)
print("this does not extract all, because intermediate attributes are missing")
for f in sumolib.xml.parse_fast(sys.argv[1], "edge", ["id", "sampledSeconds", "density", "departed"], optional=True):
    print(f)
