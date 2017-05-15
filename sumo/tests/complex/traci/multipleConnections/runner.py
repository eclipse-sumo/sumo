#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2010-02-20
@version $Id: runner.py 22608 2017-01-17 06:28:54Z behrisch $


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
from multiprocessing import Process, freeze_support

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
    addOption = "--remote-port %s" % PORT
else:
    sumoBinary = os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))
    addOption = "-S -Q --remote-port %s" % PORT

def traciLoop(port, traciEndTime, index):
    traci.init(port)
    step = 0
    while not step > traciEndTime:
        traci.simulationStep()
        #print(index, "asking for vehicles")
        sys.stdout.flush()
        vehs = traci.vehicle.getIDList()
        if vehs.index("horiz") < 0 or len(vehs) > 1:
            print("Something is false")
        step += 1
    print("Print ended at step %s" %
          (traci.simulation.getCurrentTime() / DELTA_T))
    traci.close()

def runSingle(sumoEndTime, traciEndTime):
    fdi = open("sumo.sumocfg")
    fdo = open("used.sumocfg", "w")
    fdo.write(fdi.read() % {"end": sumoEndTime})
    fdi.close()
    fdo.close()
    numClients = 2
    sumoProcess = subprocess.Popen(
        "%s -v --num-clients %s -c used.sumocfg %s" % (sumoBinary, numClients, addOption), shell=True, stdout=sys.stdout)
    procs = [Process(target=traciLoop, args=(PORT, traciEndTime, i)) for i in range(numClients)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    sumoProcess.wait()
    sys.stdout.flush()


if __name__ == '__main__':
    freeze_support()
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
