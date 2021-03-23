#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2010-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    computeStoppingPlaceUsage.py
# @author  Jakob Erdmann
# @date    2021-03-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
from collections import defaultdict

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
from sumolib.miscutils import parseTime

import os,sys

stopOutput = sys.argv[1]

stoppingPlace = "parkingArea"

vehCounts = defaultdict(list)

time = None
for stop in sumolib.xml.parse(stopOutput, "stopinfo"):
    splace = stop.getAttributeSecure(stoppingPlace, "")
    if splace != "":
        vehCounts[splace].append((parseTime(stop.started), 1))
        vehCounts[splace].append((parseTime(stop.ended), -1))


for splace, times in vehCounts.items():
    times.sort()
    with open(splace + ".txt", "w") as outf:
        tPrev = None
        count = 0
        outf.write("time count\n")
        for t,change in times:
            if t != tPrev and tPrev is not None:
                outf.write("%s %s\n" % (tPrev, count))
            count += change
            tPrev = t
        outf.write("%s %s\n" % (t, count))

