#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
import sys
import time
import threading

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def traciLoop():
    print("Starting thread")
    for step in range(10):
        traci.simulationStep()
        # print(index, "asking for vehicles")
        # sys.stdout.flush()
        vehs = traci.vehicle.getIDList()
        print(vehs)
        # time.sleep(.1)
        # ~ print(index, "vehs: ", vehs)
        # ~ sys.stdout.flush()
        if len(vehs) > 3:
            print("Something is wrong")
        time.sleep(.1)
        print("speed", traci.vehicle.getSpeed(vehs[0]))
        step += 1
        print(traci.simulation.getTime())


if __name__ == '__main__':
    traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg"])
    threads = []
    for _ in range(3):
        threads.append(threading.Thread(target=traciLoop))
        threads[-1].start()
    for t in threads:
        t.join()
    traci.close()
