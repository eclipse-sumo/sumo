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
import traci.constants as tc

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
    sys.stdout.flush()
    traci.init(port)
    traci.setOrder(index)
    message = ("Starting process %s (order: %s) with steplength %s\n" % (i, index, steplength))
    step = 1
    vehID = ""
    traciEndStep = math.ceil(traciEndTime / steplength)
    vehResults = traci.vehicle.getSubscriptionResults()
    simResults = traci.simulation.getSubscriptionResults()
    while not step > traciEndStep:
        message = ""
        message += ("Process %s:\n" % (i))
        message += ("   %s vehicle subscription results: %s\n" % (i, str(vehResults)))
        message += ("   %s simulation subscription results: %s\n" % (i, str(simResults)))
        if (vehID == ""):
            vehs = traci.vehicle.getIDList()
            if len(vehs) > 0:
                vehID = vehs[0]
                if i == 1:
                    message += ("   %s subscribing to speed (ID = %s) of vehicle '%s'\n" % (i, tc.VAR_SPEED, vehID))
                    traci.vehicle.subscribe(vehID, [tc.VAR_SPEED])
                    message += ("   -> %s\n" % str(traci.vehicle.getSubscriptionResults()))
                else:
                    message += ("   %s subscribing to acceleration (ID = %s) of vehicle '%s'\n" %
                                (i, tc.VAR_ACCEL, vehID))
                    traci.vehicle.subscribe(vehID, [tc.VAR_ACCEL])
                    message += ("   -> %s\n" % str(traci.vehicle.getSubscriptionResults()))
                    sys.stdout.flush()
        elif len(vehs) == 0:
            message += ("   %s breaking execution: traced vehicle '%s' left." % (i, vehID))
            print(message)
            sys.stdout.flush()
            break
        message += ("   %s stepping (step %s)..." % (i, step))
        print(message)
        sys.stdout.flush()
        message = ""
        time.sleep(0.01)  # give message time to be printed
        simResults = traci.simulationStep(int(step * steplength * 1000))
        vehResults = traci.vehicle.getSubscriptionResults()
        step += 1
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
    print(" Testing multiclient subscriptions...")
    for i in range(0, runNr):
        print("\n###### Run %s ######" % i)
        sys.stdout.flush()
        runSingle(50, 120, numClients, i)
