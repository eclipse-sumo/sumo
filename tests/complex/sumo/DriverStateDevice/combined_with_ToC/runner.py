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

myVehicle = "deviceHolder"


def printParams(vehID, only_dynamic=False):
    awareness = traci.vehicle.getParameter(vehID, "device.driverstate.awareness")
    errorState = traci.vehicle.getParameter(vehID, "device.driverstate.errorState")
    errorTimeScale = traci.vehicle.getParameter(vehID, "device.driverstate.errorTimeScale")
    errorNoiseIntensity = traci.vehicle.getParameter(vehID, "device.driverstate.errorNoiseIntensity")
    minAwareness = traci.vehicle.getParameter(vehID, "device.driverstate.minAwareness")
    initialAwareness = traci.vehicle.getParameter(vehID, "device.driverstate.initialAwareness")
    errorTimeScaleCoefficient = traci.vehicle.getParameter(vehID, "device.driverstate.errorTimeScaleCoefficient")
    errorNoiseIntensityCoefficient = traci.vehicle.getParameter(
        vehID, "device.driverstate.errorNoiseIntensityCoefficient")
    speedDifferenceErrorCoefficient = traci.vehicle.getParameter(
        vehID, "device.driverstate.speedDifferenceErrorCoefficient")
    headwayErrorCoefficient = traci.vehicle.getParameter(vehID, "device.driverstate.headwayErrorCoefficient")
    speedDifferenceChangePerceptionThreshold = traci.vehicle.getParameter(
        vehID, "device.driverstate.speedDifferenceChangePerceptionThreshold")
    headwayChangePerceptionThreshold = traci.vehicle.getParameter(
        vehID, "device.driverstate.headwayChangePerceptionThreshold")
    traci.vehicle.getSpeed(vehID)

    print("time", traci.simulation.getTime())
    print("Driver state device infos for vehicle '%s'" % vehID)
    if not only_dynamic:
        print("Static parameters:")
        print("  minAwareness = %s" % minAwareness)
        print("  initialAwareness = %s" % initialAwareness)
        print("  errorTimeScaleCoefficient = %s" % errorTimeScaleCoefficient)
        print("  errorNoiseIntensityCoefficient = %s" % errorNoiseIntensityCoefficient)
        print("  speedDifferenceErrorCoefficient = %s" % speedDifferenceErrorCoefficient)
        print("  headwayErrorCoefficient = %s" % headwayErrorCoefficient)
        print("  speedDifferenceChangePerceptionThreshold = %s" % speedDifferenceChangePerceptionThreshold)
        print("  headwayChangePerceptionThreshold = %s" % headwayChangePerceptionThreshold)
        print("Dynamic parameters:")
    print("  awareness = %s" % awareness)
    print("  errorNoiseIntensity = %s" % errorNoiseIntensity)
    print("  errorTimeScale = %s" % errorTimeScale)
    print("  errorState = %s" % errorState)


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
    traci.start([sumoBinary, "-n", "input_net.net.xml", "-r", "input_routes.rou.xml", "--no-step-log", "true"])

    # Wait until the vehicle enters
    while myVehicle not in traci.vehicle.getIDList():
        traci.simulationStep()

    printParams(myVehicle)
    printToCParams(myVehicle)

    traci.close()
    sys.stdout.flush()
