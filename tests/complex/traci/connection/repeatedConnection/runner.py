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
# @date    2010-02-20

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

PORT = sumolib.miscutils.getFreeSocketPort()
sumoBinary = sumolib.checkBinary(sys.argv[1])


def runSingle(sumoEndTime, traciEndTime):
    fdi = open("sumo.sumocfg")
    fdo = open("used.sumocfg", "w")
    fdo.write(fdi.read() % {"end": sumoEndTime})
    fdi.close()
    fdo.close()
    step = 0
    sumoProcess = subprocess.Popen(
        "%s -c used.sumocfg -S -Q --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
    traci.init(PORT)
    while not step > traciEndTime:
        traci.simulationStep()
        vehs = traci.vehicle.getIDList()
        if vehs.index("horiz") < 0 or len(vehs) > 3:
            print("Something is wrong")
        step += 1
    print("Print ended at step %s" % traci.simulation.getTime())
    traci.close()
    sumoProcess.wait()
    sys.stdout.flush()


print("----------- SUMO ends first -----------")
sys.stdout.flush()
for i in range(0, 10):
    print(" Run %s" % i)
    runSingle(50, 99)

print("----------- TraCI ends first -----------")
sys.stdout.flush()
for i in range(0, 10):
    print(" Run %s" % i)
    runSingle(101, 99)
