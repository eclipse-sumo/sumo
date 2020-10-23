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
import traci.constants as tc  # noqa

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
    sys.stdout.flush()
    traci.init(port)
    traci.setOrder(index)
    message = ("Starting process %s (order: %s) with steplength %s\n" % (i, index, steplength))
    step = 1
    vehID = ""
    traciEndStep = math.ceil(traciEndTime / steplength)
    vehResults = sorted((k, sorted(v.items())) for k, v in traci.vehicle.getAllSubscriptionResults().items())
    simResults = traci.simulation.getSubscriptionResults()
    while not step > traciEndStep:
        message = ""
        message += ("Process %s:\n" % (i))
        message += ("   %s vehicle subscription results: %s\n" % (i, vehResults))
        message += ("   %s simulation subscription results: %s\n" % (i, simResults))
        if (vehID == ""):
            vehs = traci.vehicle.getIDList()
            if len(vehs) > 0:
                vehID = vehs[0]
                if i == 1:
                    message += ("   %s subscribing to speed (ID = %s) of vehicle '%s'\n" % (i, tc.VAR_SPEED, vehID))
                    traci.vehicle.subscribe(vehID, [tc.VAR_SPEED])
                    message += ("   -> %s\n" % str(traci.vehicle.getAllSubscriptionResults()))
                else:
                    message += ("   %s subscribing to acceleration (ID = %s) of vehicle '%s'\n" %
                                (i, tc.VAR_ACCEL, vehID))
                    traci.vehicle.subscribe(vehID, [tc.VAR_ACCEL])
                    message += ("   -> %s\n" % str(traci.vehicle.getAllSubscriptionResults()))
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
        simResults = traci.simulationStep(step * steplength)
        vehResults = sorted((k, sorted(v.items())) for k, v in traci.vehicle.getAllSubscriptionResults().items())
        step += 1
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
    print(" Testing multiclient subscriptions...")
    for i in range(0, runNr):
        print("\n###### Run %s ######" % i)
        sys.stdout.flush()
        runSingle(50, 120, numClients, i)
