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
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2011-07-22

from __future__ import print_function
from __future__ import division

import os
import subprocess
import sys
sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa


def call(cmd):
    subprocess.call(cmd)
    for s in sumolib.xml.parse("stats.xml", "vehicleTripStatistics"):
        return float(s.duration)


sumoBinary = sumolib.checkBinary("sumo")
cmd = [sumoBinary, "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
       "--statistic-output", "stats.xml", "--no-step-log", "--duration-log.statistics"]
duration = call(cmd)
durationMeso = call(cmd + ["--mesosim"])
cmd[2] = "input_net2.net.xml"
duration2 = call(cmd)
duration2Meso = call(cmd + ["--mesosim"])

print("Deviation split vs. joined (meso):", (durationMeso - duration2Meso) / duration2Meso)
print("Deviation split vs. joined (micro):", (duration - duration2) / duration2)
print("Deviation meso vs. micro (split):", (duration - durationMeso) / duration)
print("Deviation meso vs. micro (joined):", (duration2 - duration2Meso) / duration2)
