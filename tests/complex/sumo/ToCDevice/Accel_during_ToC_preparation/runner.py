#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2020 German Aerospace Center (DLR) and others.
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
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26

from __future__ import absolute_import
from __future__ import print_function

import sys

from runnerlib import get_options, printToCParams, requestToC  # sys.path modification is done there
import traci
from sumolib import checkBinary

ToC_vehicle = "ToC_veh"
timeTillMRM = 30


def run():
    """execute the TraCI control loop"""
    step = 0
    lastLane = traci.vehicle.getLaneIndex(ToC_vehicle)
    t = traci.simulation.getTime()
    print("Time %s: Current lane of veh '%s': %s" % (t, ToC_vehicle, lastLane))
    traci.simulationStep()
    while traci.simulation.getMinExpectedNumber() > 0:
        if step == 2:
            requestToC(ToC_vehicle, timeTillMRM)
            t = traci.simulation.getTime()
            print("Requested ToC of veh0 at t=%s (until t=%s)" % (t, t + timeTillMRM))
        newLane = traci.vehicle.getLaneIndex(ToC_vehicle)
        acceleration = traci.vehicle.getAcceleration(ToC_vehicle)
        accel = traci.vehicle.getAccel(ToC_vehicle)
        if newLane != lastLane:
            t = traci.simulation.getTime()
            print("Time %s: veh '%s' changed lanes. Current: %s" % (t, ToC_vehicle, newLane))
            lastLane = newLane
        print("Acceleration = %s" % acceleration)
        print("Accel = %s" % accel)
        printToCParams(ToC_vehicle, True)
        sys.stdout.flush()
        step += 1
        traci.simulationStep()


# this is the main entry point of this script
if __name__ == "__main__":
    options = get_options()

    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if options.nogui:
        sumoBinary = checkBinary('sumo')
    else:
        sumoBinary = checkBinary('sumo-gui')

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    traci.start([sumoBinary, "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
                 "--no-step-log", "true",
                 "--default.speeddev", "0"])

    # Wait until the vehicle enters
    while ToC_vehicle not in traci.vehicle.getIDList():
        traci.simulationStep()

    printToCParams(ToC_vehicle)

    run()

    traci.close()
    sys.stdout.flush()
