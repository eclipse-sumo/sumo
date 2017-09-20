#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@author  Leonhard Luecken
@date    2010-02-20
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
"""
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
            traci.simulationStep(int(step * steplength * 1000))
            #print(index, "asking for vehicles")
            # sys.stdout.flush()
            vehs = traci.vehicle.getIDList()
            nrEnteredVehicles += traci.simulation.getDepartedNumber()
            #~ print(index, "Newly entered vehicles: ", traci.simulation.getDepartedNumber(), "(vehs: ", vehs, ")")
            #~ sys.stdout.flush()
            step += 1
        endTime = traci.simulation.getCurrentTime() / DELTA_T
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
        "%s -v --num-clients %s -c used.sumocfg %s" % (sumoBinary, numClients, addOption), shell=True, stdout=sys.stdout)
    # Alternate ordering
    indexRange = range(numClients) if (runNr % 2 == 0) else list(reversed(range(numClients)))
    procs = [Process(target=traciLoop, args=(PORT, traciEndTime, i + 1, steplengths[indexRange[i]]))
             for i in range(numClients)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    sumoProcess.wait()
    sys.stdout.flush()


if __name__ == '__main__':
    freeze_support()
    runNr = 2
    clientRange = [4]
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
