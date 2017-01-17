#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-03-21
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib
import traci

PORT = sumolib.miscutils.getFreeSocketPort()
DELTA_T = 1000

if sys.argv[1] == "sumo":
    sumoBinary = os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = []
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
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
    print("Print ended at step %s" %
          (traci.simulation.getCurrentTime() / DELTA_T))
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
