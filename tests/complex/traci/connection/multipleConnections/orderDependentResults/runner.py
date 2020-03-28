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
# @author  Leonhard Luecken
# @date    2010-02-20

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import math
from multiprocessing import Process, freeze_support

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

PORT = sumolib.miscutils.getFreeSocketPort()
DELTA_T = 1000
sumoBinary = sumolib.checkBinary(sys.argv[1])


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
        traci.simulationStep(step * steplength)
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
    endTime = traci.simulation.getTime()
    traci.close()
    time.sleep(orderTime * i)  # assure ordering of outputs
    print("Process %s (order %s) ended at step %s" % (i, index, endTime))
    sys.stdout.flush()


def runSingle(sumoEndTime, traciEndTime, numClients, runNr):
    sumoProcess = subprocess.Popen(
        "%s -v --num-clients %s -c sumo.sumocfg -S -Q --remote-port %s" %
        (sumoBinary, numClients, PORT), shell=True, stdout=sys.stdout)  # Alternate ordering
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
