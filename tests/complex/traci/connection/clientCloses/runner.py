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
# @date    2010-03-21

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

PORT = sumolib.miscutils.getFreeSocketPort()
DELTA_T = 1000

sumoBinary = sumolib.checkBinary(sys.argv[1])
if sys.argv[1] == "sumo":
    addOption = []
else:
    addOption = ["-S", "-Q"]


def runSingle(traciEndTime, sumoEndTime=None):
    step = 0
    if sumoEndTime is None:
        opt = addOption
    else:
        opt = addOption + ["--end", str(sumoEndTime)]
    sumoProcess = subprocess.Popen(
        [sumoBinary, "-c", "sumo.sumocfg", "--remote-port", str(PORT)] + opt)
    traci.init(PORT)
    while not step > traciEndTime:
        traci.simulationStep()
        step += 1
    print("Print ended at step", traci.simulation.getTime())
    traci.close()
    sumoProcess.wait()
    sys.stdout.flush()


print("=========== long route ===========")
fdo = open("input_routes.rou.xml", "w")
print('<routes>"', file=fdo)
print(
    '   <route id="horizontal" edges="2fi 2si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si"/>', file=fdo)
print('   <vehicle id="horiz" route="horizontal" depart="0"/>', file=fdo)
print('</routes>', file=fdo)
fdo.close()
print("----------- SUMO end time is smaller than TraCI's -----------")
sys.stdout.flush()
runSingle(99, 50)
print("----------- SUMO end time is not given -----------")
sys.stdout.flush()
runSingle(99)


print("=========== empty routes in SUMO ===========")
fdo = open("input_routes.rou.xml", "w")
print('<routes>"', file=fdo)
print('</routes>', file=fdo)
fdo.close()
print("----------- SUMO end time is smaller than TraCI's -----------")
sys.stdout.flush()
runSingle(99, 50)
print("----------- SUMO end time is not given -----------")
sys.stdout.flush()
runSingle(99)


print("=========== vehicle leaves before TraCI ends ===========")
fdo = open("input_routes.rou.xml", "w")
print('<routes>"', file=fdo)
print('   <route id="horizontal" edges="2fi 2si"/>', file=fdo)
print('   <vehicle id="horiz" route="horizontal" depart="0"/>', file=fdo)
print('</routes>', file=fdo)
fdo.close()
print("----------- SUMO end time is smaller than TraCI's -----------")
sys.stdout.flush()
runSingle(99, 50)
print("----------- SUMO end time is not given -----------")
sys.stdout.flush()
runSingle(99)
