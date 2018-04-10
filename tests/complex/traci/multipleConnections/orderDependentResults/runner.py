#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Leonhard Luecken
# @date    2010-02-20
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import math
from multiprocessing import Process, freeze_support

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci

PORT = sumolib.miscutils.getFreeSocketPort()
DELTA_T = 1000

if sys.argv[1] == "sumo":
    sumoBinary = os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))
    addOption = "--remote-port %s" % PORT
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q --remote-port %s" % PORT


def traciLoop(port, traciEndTime, i, runNr, steplength=0):
    orderTime = 0.25
    time.sleep(orderTime * i)  # assure ordering of outputs
    if steplength == 0:
        steplength = DELTA_T / 1000.
    # order index dependent on runNr
    index = i if (runNr % 2 == 0) else 10 - i
    print("Starting process %s (order: %s) with steplength %s" % (i, index, steplength))
    sys.stdout.flush()
    traci.init(port)
    traci.setOrder(index)
    step = 1
    lastVehID = ""
    traciEndStep = math.ceil(traciEndTime / steplength)
    while not step > traciEndStep:
        print("Process %s:" % (i))
        print("   stepping (step %s)..." % step)
        traci.simulationStep(int(step * steplength * 1000))
        vehs = traci.vehicle.getIDList()
        if len(vehs) != 0:
            vehID = vehs[0]
            if vehID != lastVehID and lastVehID != "":
                print("   breaking execution: traced vehicle '%s' left." % lastVehID)
                break
            else:
                lastVehID = vehID
            print("   Retrieving position for vehicle '%s' -> %s on lane '%s'" %
                  (vehID, traci.vehicle.getLanePosition(vehID), traci.vehicle.getLaneID(vehID)))
            print("   Retrieving speed for vehicle '%s' -> %s" % (vehID, traci.vehicle.getSpeed(vehID)))
            traci.vehicle.setSpeedMode(vehID, 0)
            newSpeed = i * 5
            print("   Setting speed for vehicle '%s' -> %s" % (vehID, newSpeed))
            print("   Retrieving speed for vehicle '%s' -> %s" % (vehID, traci.vehicle.getSpeed(vehID)))
            traci.vehicle.setSpeed(vehID, newSpeed)
        elif lastVehID != "":
            print("   breaking execution: traced vehicle '%s' left." % lastVehID)
            break
        step += 1
        sys.stdout.flush()
    endTime = traci.simulation.getCurrentTime() / DELTA_T
    traci.close()
    time.sleep(orderTime * i)  # assure ordering of outputs
    print("Process %s (order %s) ended at step %s" % (i, index, endTime))
    sys.stdout.flush()


def runSingle(sumoEndTime, traciEndTime, numClients, runNr):
    sumoProcess = subprocess.Popen(
        "%s -v --num-clients %s -c sumo.sumocfg %s" % (sumoBinary, numClients, addOption), shell=True, stdout=sys.stdout)  # Alternate ordering
    procs = [Process(target=traciLoop, args=(PORT, traciEndTime, (i + 1), runNr)) for i in range(numClients)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    sumoProcess.wait()
    sys.stdout.flush()


if __name__ == '__main__':
    freeze_support()
    numClients = 2
    runNr = 2
    print(" Testing client order dependence ...")
    for i in range(0, runNr):
        print("\n###### Run %s ######" % i)
        sys.stdout.flush()
        runSingle(50, 500, numClients, i)
