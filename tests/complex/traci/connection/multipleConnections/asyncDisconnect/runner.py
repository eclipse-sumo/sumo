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


def traciLoop(port, traciEndTime, index, steplength=0):
    orderTime = 0.25
    time.sleep(orderTime * index)  # assure ordering of outputs
    if steplength == 0:
        steplength = DELTA_T / 1000.
    print("Starting process %s with steplength %s" % (index, steplength))
    sys.stdout.flush()
    traci.init(port)
    traci.setOrder(index)
    step = 1
    nrEnteredVehicles = 0
    sumoStop = False
    try:
        traciEndStep = math.ceil(traciEndTime / steplength)
        while not step > traciEndStep:
            traci.simulationStep(step * steplength)
            # print(index, "asking for vehicles")
            # sys.stdout.flush()
            traci.vehicle.getIDList()
            nrEnteredVehicles += traci.simulation.getDepartedNumber()
            # ~ print(index, "Newly entered vehicles: ", traci.simulation.getDepartedNumber(), "(vehs: ", vehs, ")")
            # ~ sys.stdout.flush()
            step += 1
        endTime = traci.simulation.getTime()
        traci.close()
    except traci.FatalTraCIError as e:
        if str(e) == "connection closed by SUMO":
            time.sleep(orderTime * index)  # assure ordering of outputs
            sumoStop = True
            print("client %s: " % index, str(e), " (at TraCIStep %s)" % step)
            sys.stdout.flush()
        else:
            raise
    if not sumoStop:
        time.sleep(orderTime * index)  # assure ordering of outputs
        print("Process %s ended at step %s" % (index, endTime))
        print("Process %s was informed about %s entered vehicles" % (index, nrEnteredVehicles))
        sys.stdout.flush()


def runSingle(sumoEndTime, traciEndTime, numClients, steplengths, runNr):
    fdi = open("sumo.sumocfg")
    fdo = open("used.sumocfg", "w")
    fdo.write(fdi.read() % {"end": sumoEndTime, "steplength": DELTA_T / 1000.})
    fdi.close()
    fdo.close()
    sumoProcess = subprocess.Popen(
        "%s -v --num-clients %s -c used.sumocfg -S -Q --remote-port %s" %
        (sumoBinary, numClients, PORT), shell=True, stdout=sys.stdout)
    # Alternate ordering
    indexRange = range(numClients) if (runNr % 2 == 0) else list(reversed(range(numClients)))
    procs = [Process(target=traciLoop, args=(
        PORT, (traciEndTime if i != 0 else 20), i + 1, steplengths[indexRange[i]])) for i in range(numClients)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    sumoProcess.wait()
    sys.stdout.flush()


if __name__ == '__main__':
    freeze_support()
    runNr = 2
    clientRange = [2, 3]
    steplengths = [0.1, 1.0, 1.7, 2.0]
    print("----------- SUMO ends first -----------")
    for numClients in clientRange:
        print("   -------- numClients: %s  --------    " % numClients)
        sys.stdout.flush()
        for i in range(0, runNr):
            print(" Run %s" % i)
            sys.stdout.flush()
            runSingle(50, 99, numClients, steplengths, i)

    print("----------- TraCI ends first -----------")
    for numClients in clientRange:
        print("   -------- numClients: %s  --------    " % numClients)
        sys.stdout.flush()
        for i in range(0, runNr):
            print(" Run %s" % i)
            sys.stdout.flush()
            runSingle(101, 99, numClients, steplengths, i)
