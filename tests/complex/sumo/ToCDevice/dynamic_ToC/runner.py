#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import optparse

from runnerlib import get_options, printToCParams, requestToC  # sys.path modification is done there
import traci
from sumolib import checkBinary

ToC_vehicle = "ToC_veh"
timeTillMRM = 5


def run():
    """execute the TraCI control loop"""
    step = 0
    printToCParams(ToC_vehicle, False)
    t = traci.simulation.getTime()
    # prevent LCs
    origLCMode = traci.vehicle.getLaneChangeMode(ToC_vehicle)
    traci.vehicle.setLaneChangeMode(ToC_vehicle, 768)
    restoredLCMode = False
    traci.simulationStep()
    while traci.simulation.getMinExpectedNumber() > 0:
        t = traci.simulation.getTime()
        print("Time %s: Current lane of veh '%s': %s" % (t, ToC_vehicle, traci.vehicle.getLaneID(ToC_vehicle)))
        printToCParams(ToC_vehicle, True)
        if (not restoredLCMode and traci.vehicle.getParameter(ToC_vehicle, "device.toc.state") == "RECOVERING"):
            traci.vehicle.setLaneChangeMode(ToC_vehicle, origLCMode)
            # ~ restoredLCMode = True
            print("Switched to manual mode: Allowing LCs!")
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
