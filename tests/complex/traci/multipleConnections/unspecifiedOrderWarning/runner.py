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


def traciLoop(port, traciEndTime, index, orderOdd):
    orderTime = 0.25
    time.sleep(orderTime * index)  # assure ordering of outputs
    print("Starting process %s" % (index))
    sys.stdout.flush()
    step = 1
    try:
        traci.init(port)
        if orderOdd and index % 2 == 1:
            traci.setOrder(index)
        sumoStop = False
        while not step > traciEndTime:
            traci.simulationStep()
            vehs = traci.vehicle.getIDList()
            if len(vehs) > 3:
                print("Something is wrong")
            step += 1
        endTime = traci.simulation.getCurrentTime() / DELTA_T
        traci.close()
    #~ except traci.FatalTraCIError as e:
    except Exception as e:
        time.sleep(orderTime * index)  # assure ordering of outputs
        sumoStop = True
        print("client %s: " % index, str(e), " (at TraCIStep %s)" % step)
        sys.stdout.flush()
    if not sumoStop:
        time.sleep(orderTime * index)  # assure ordering of outputs
        print("Process %s ended at step %s" % (index, endTime))
        sys.stdout.flush()


def runSingle(sumoEndTime, traciEndTime, numClients, orderOdd=False):
    fdi = open("sumo.sumocfg")
    fdo = open("used.sumocfg", "w")
    fdo.write(fdi.read() % {"end": sumoEndTime, "steplength": DELTA_T / 1000.})
    fdi.close()
    fdo.close()
    sumoProcess = subprocess.Popen(
        "%s -v --num-clients %s -c used.sumocfg %s" % (sumoBinary, numClients, addOption), shell=True, stdout=sys.stdout)  # Alternate ordering
    procs = [Process(target=traciLoop, args=(PORT, traciEndTime, i + 1, orderOdd)) for i in range(numClients)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    sumoProcess.wait()
    sys.stdout.flush()


if __name__ == '__main__':
    freeze_support()
    print("----------- Warning Test -----------")
    print(" Run 1")
    sys.stdout.flush()
    runSingle(50, 99, 2)
    print(" Run 2")
    runSingle(50, 99, 2, True)
