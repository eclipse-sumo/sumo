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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2010-03-02

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
import traci  # noqa
import sumolib  # noqa
from traci.exceptions import FatalTraCIError  # noqa

DELTA_T = 1000

netconvertBinary = sumolib.checkBinary('netconvert')
sumoBinary = sumolib.checkBinary('sumo')


def runSingle(addOption):
    step = 0
    timeline = []
    traci.start([sumoBinary, "-c", "sumo.sumocfg"] + addOption)
    while not step > 10000:
        try:
            traci.simulationStep()
            vehs = traci.vehicle.getIDList()
            timeline.append({})
            for v in vehs:
                timeline[-1][v] = traci.vehicle.getSpeed(v)
            step += 1
        except FatalTraCIError:
            print("Closed by SUMO")
            break
    traci.close()
    sys.stdout.flush()
    return timeline


def evalTimeline(timeline):
    ct = 0
    for t in timeline:
        if ct > 1000:
            for v in t:
                s = t[v]
                if s < 8.9 or s > 9.1:
                    print(
                        " Mismatching velocity of vehicle %s at time %s (%s)" % (v, ct, s))
        ct = ct + 1


print(">>> Building the network (with internal)")
sys.stdout.flush()
retcode = subprocess.call(
    [netconvertBinary, "-c", "netconvert.netccfg"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print(">>> Checking Simulation (network: internal, simulation: internal)")
evalTimeline(runSingle([]))
time.sleep(1)
print(">>> Checking Simulation (network: internal, simulation: no internal)")
evalTimeline(runSingle(["--no-internal-links"]))
time.sleep(1)

print("")
print(">>> Building the network (without internal)")
sys.stdout.flush()
retcode = subprocess.call([netconvertBinary, "-c", "netconvert.netccfg",
                           "--no-internal-links"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print(">>> Checking Simulation (network: no internal, simulation: internal)")
evalTimeline(runSingle([]))
time.sleep(1)
print(
    ">>> Checking Simulation (network: no internal, simulation: no internal)")
evalTimeline(runSingle(["--no-internal-links"]))
