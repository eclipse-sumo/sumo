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
from multiprocessing import Process, freeze_support

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

PORT = sumolib.miscutils.getFreeSocketPort()
DELTA_T = 1000
sumoBinary = sumolib.checkBinary(sys.argv[1])


def traciLoop(port, traciEndTime, index, orderOdd):
    orderTime = 0.25
    time.sleep(orderTime * index)  # assure ordering of outputs
    print("Starting process %s" % (index))
    sys.stdout.flush()
    time.sleep(orderTime * index)  # assure ordering of outputs
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
        endTime = traci.simulation.getTime()
        traci.close()
    # ~ except traci.FatalTraCIError as e:
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
        "%s -v --num-clients %s -c used.sumocfg -S -Q --remote-port %s" %
        (sumoBinary, numClients, PORT), shell=True, stdout=sys.stdout)  # Alternate ordering
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
    sys.stdout.flush()
    runSingle(50, 99, 2, True)
