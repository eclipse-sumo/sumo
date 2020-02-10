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

import os
import sys
import optparse

# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.path.dirname(
        __file__), '..', '..', '..', '..', "tools"))  # tutorial in tests
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
        os.path.dirname(__file__), "..", "..", "..")), "tools"))  # tutorial in docs
    from sumolib import checkBinary  # noqa
except ImportError:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import traci  # noqa

ToC_vehicle = "ToC_veh"
timeTillMRM = 5


def run():
    """execute the TraCI control loop"""
    step = 0
    lastLane = traci.vehicle.getLaneIndex(ToC_vehicle)
    t = traci.simulation.getTime()
    print("Time %s: Current lane of veh '%s': %s" % (t, ToC_vehicle, lastLane))
    traci.simulationStep()
    while traci.simulation.getMinExpectedNumber() > 0:
        if step == 29:
            traci.vehicle.changeLane(ToC_vehicle, 1, 5)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 1))
        if step == 49:
            # Let vehicle go to left lane, which is strategically not adequate
            traci.vehicle.changeLane(ToC_vehicle, 1, 5.)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 1))
        if step == 50:
            requestToC(ToC_vehicle, timeTillMRM)
            t = traci.simulation.getTime()
            print("Requested ToC of veh0 at t=%s (until t=%s)" % (t, t + timeTillMRM))
        if step == 51:
            traci.vehicle.changeLane(ToC_vehicle, 0, 5.)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 0))
        if step == 53:
            traci.vehicle.changeLane(ToC_vehicle, 1, 4.)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 1))
        if step == 75:
            traci.vehicle.changeLane(ToC_vehicle, 0, 10.)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 0))
        if step == 78:
            traci.vehicle.changeLane(ToC_vehicle, 1, 2.)
            t = traci.simulation.getTime()
            print("Time %s: Requested lanechange of veh '%s' to lane %s" % (t, ToC_vehicle, 1))
        newLane = traci.vehicle.getLaneIndex(ToC_vehicle)
        if newLane != lastLane:
            t = traci.simulation.getTime()
            print("Time %s: veh '%s' changed lanes. Current: %s" % (t, ToC_vehicle, newLane))
            lastLane = newLane

        printToCParams(ToC_vehicle, True)
        sys.stdout.flush()
        step += 1
        traci.simulationStep()


def requestToC(vehID, timeTillMRM):
    traci.vehicle.setParameter(vehID, "device.toc.requestToC", str(timeTillMRM))


def printToCParams(vehID, only_dynamic=False):
    holder = traci.vehicle.getParameter(vehID, "device.toc.holder")
    manualType = traci.vehicle.getParameter(vehID, "device.toc.manualType")
    automatedType = traci.vehicle.getParameter(vehID, "device.toc.automatedType")
    responseTime = traci.vehicle.getParameter(vehID, "device.toc.responseTime")
    recoveryRate = traci.vehicle.getParameter(vehID, "device.toc.recoveryRate")
    initialAwareness = traci.vehicle.getParameter(vehID, "device.toc.initialAwareness")
    mrmDecel = traci.vehicle.getParameter(vehID, "device.toc.mrmDecel")
    currentAwareness = traci.vehicle.getParameter(vehID, "device.toc.currentAwareness")
    state = traci.vehicle.getParameter(vehID, "device.toc.state")
    speed = traci.vehicle.getSpeed(vehID)

    print("time", traci.simulation.getTime())
    print("ToC device infos for vehicle '%s'" % vehID)
    if not only_dynamic:
        print("Static parameters:")
        print("  holder = %s" % holder)
        print("  manualType = %s" % manualType)
        print("  automatedType = %s" % automatedType)
        print("  responseTime = %s" % responseTime)
        print("  recoveryRate = %s" % recoveryRate)
        print("  initialAwareness = %s" % initialAwareness)
        print("  mrmDecel = %s" % mrmDecel)
        print("Dynamic parameters:")
    print("  currentAwareness = %s" % currentAwareness)
    print("  currentSpeed = %s" % speed)
    print("  state = %s" % state)


def get_options():
    optParser = optparse.OptionParser()
    optParser.add_option("--nogui", action="store_false",
                         default=True, help="run the commandline version of sumo")
    options, args = optParser.parse_args()
    return options


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
                 "--fcd-output", "fcd.xml",
                 "--no-step-log", "true",
                 "--default.speeddev", "0"])

    # Wait until the vehicle enters
    while ToC_vehicle not in traci.vehicle.getIDList():
        traci.simulationStep()

    printToCParams(ToC_vehicle)

    run()

    traci.close()
    sys.stdout.flush()
