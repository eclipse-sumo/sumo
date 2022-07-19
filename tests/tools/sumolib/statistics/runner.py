#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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
# @author  Laura Bieker-Walz
# @date 14.9.2020

from __future__ import absolute_import
from __future__ import print_function


import os
import sys
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib.statistics  # noqa

print(sumolib.statistics.round(1.5))
print(sumolib.statistics.round(-1.5))
print(sumolib.statistics.setPrecision("2.01%.2f", 3))
print(sumolib.statistics.geh(0, 0))
print(sumolib.statistics.geh(10, 1))

s = sumolib.statistics.Statistics(10)
print(s.toString())
s.add(1)
print(s.toString())
s2 = sumolib.statistics.Statistics(5)
s2.add(5)
s2.add(4)
s2.add(3)
print(s2.toString())
s.update(s2)
print(s.toString())
s.clear()
print(s.toString())
s.add(1)
s.add(10)
s.add(1)
s.add(1)
print(s.count())
print(s.avg())
print(s.avg_abs())
print(s.meanAndStdDev())
print(s.relStdDev())
print(s.mean())
print(s.mean_abs())
print(s.average_absolute_deviation_from_mean())
print(s.median())
print(s.median_abs())
print(s.quartiles())
print(s.rank(0.5))
print(s.histogram())
s3 = sumolib.statistics.Statistics(10, histogram=True)
for i in range(10):
    s3.add(i)
    i += 1
print(s3.histogram())
