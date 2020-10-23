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
from sumolib.miscutils import Statistics  # noqa


def call(cmd):
    #    print(cmd)
    #    sys.stdout.flush()
    #    subprocess.call(cmd)#, stdout=open(os.devnull, "w"))
    subprocess.call(cmd, stdout=open(os.devnull, "w"))
    for s in sumolib.xml.parse("stats.xml", "vehicleTripStatistics"):
        return float(s.duration)


subprocess.call([sumolib.checkBinary("netgenerate"), "--grid", "--grid.length", "500", "-o", "int.net.xml"])
subprocess.call([sumolib.checkBinary("netgenerate"), "--grid", "--grid.length",
                 "500", "--no-internal-links", "-o", "noint.net.xml"])
sumoBinary = sumolib.checkBinary("sumo")
for net in ("int.net.xml", "noint.net.xml"):
    cmd = [sumoBinary, "-n", net, "-r", "input_routes.rou.xml", "--junction-taz", "--device.rerouting.probability", "1",
           "--statistic-output", "stats.xml", "--no-step-log", "--duration-log.statistics"]
    duration = call(cmd)
    stats = Statistics("Durations %s" % net)
    statsJC = Statistics("Durations with junction control %s" % net)
    for segLength in range(10, 200, 10):
        stats.add(call(cmd + ["--mesosim", "--meso-edgelength", str(segLength)]), label=segLength)
        statsJC.add(call(cmd + ["--mesosim", "--meso-junction-control",
                                "--meso-edgelength", str(segLength)]), label=segLength)
    print(stats)
    print(statsJC)
    print("Deviation %s meso vs. micro:" % net, (duration - stats.mean()) / duration)
    print("Deviation %s meso jc vs. micro:" % net, (duration - statsJC.mean()) / duration)
