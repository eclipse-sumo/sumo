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
    maximalReactionTime = traci.vehicle.getParameter(
        vehID, "device.driverstate.maximalReactionTime")
    originalReactionTime = traci.vehicle.getParameter(
        vehID, "device.driverstate.originalReactionTime")
    actionStepLength = traci.vehicle.getParameter(
        vehID, "device.driverstate.actionStepLength")
    actionStepLengthVeh = traci.vehicle.getActionStepLength(vehID)
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
        print("  maximalReactionTime = %s" % maximalReactionTime)
        print("  originalReactionTime = %s" % originalReactionTime)
        print("Dynamic parameters:")
    print("  awareness = %s" % awareness)
    print("  errorNoiseIntensity = %s" % errorNoiseIntensity)
    print("  errorTimeScale = %s" % errorTimeScale)
    print("  errorState = %s" % errorState)
    print("  actionStepLength(dev) = %s" % actionStepLength)
    print("  actionStepLength(veh) = %s" % actionStepLengthVeh)


def resetParams(vehID):
    print("\nResetting parameters...\n")

    new_minAwareness = 0.05
    traci.vehicle.setParameter(vehID, "device.driverstate.minAwareness", str(new_minAwareness))
    minAwareness = float(traci.vehicle.getParameter(vehID, "device.driverstate.minAwareness"))
    print("new minAwareness:%s (diff=%s)" % (minAwareness, minAwareness - new_minAwareness))

    new_initialAwareness = 0.06
    traci.vehicle.setParameter(vehID, "device.driverstate.initialAwareness", str(new_initialAwareness))
    initialAwareness = float(traci.vehicle.getParameter(vehID, "device.driverstate.initialAwareness"))
    print("new initialAwareness:%s (diff=%s)" % (initialAwareness, initialAwareness - new_initialAwareness))

    new_errorTimeScaleCoefficient = 0.11
    traci.vehicle.setParameter(vehID, "device.driverstate.errorTimeScaleCoefficient",
                               str(new_errorTimeScaleCoefficient))
    errorTimeScaleCoefficient = float(traci.vehicle.getParameter(vehID, "device.driverstate.errorTimeScaleCoefficient"))
    print("new errorTimeScaleCoefficient:%s (diff=%s)" %
          (errorTimeScaleCoefficient, errorTimeScaleCoefficient - new_errorTimeScaleCoefficient))

    new_errorNoiseIntensityCoefficient = 0.22
    traci.vehicle.setParameter(vehID, "device.driverstate.errorNoiseIntensityCoefficient",
                               str(new_errorNoiseIntensityCoefficient))
    errorNoiseIntensityCoefficient = float(traci.vehicle.getParameter(
        vehID, "device.driverstate.errorNoiseIntensityCoefficient"))
    print("new errorNoiseIntensityCoefficient:%s (diff=%s)" %
          (errorNoiseIntensityCoefficient, errorNoiseIntensityCoefficient - new_errorNoiseIntensityCoefficient))

    new_speedDifferenceErrorCoefficient = 0.33
    traci.vehicle.setParameter(vehID, "device.driverstate.speedDifferenceErrorCoefficient",
                               str(new_speedDifferenceErrorCoefficient))
    speedDifferenceErrorCoefficient = float(traci.vehicle.getParameter(
        vehID, "device.driverstate.speedDifferenceErrorCoefficient"))
    print("new speedDifferenceErrorCoefficient:%s (diff=%s)" %
          (speedDifferenceErrorCoefficient, speedDifferenceErrorCoefficient - new_speedDifferenceErrorCoefficient))

    new_headwayErrorCoefficient = 0.44
    traci.vehicle.setParameter(vehID, "device.driverstate.headwayErrorCoefficient", str(new_headwayErrorCoefficient))
    headwayErrorCoefficient = float(traci.vehicle.getParameter(vehID, "device.driverstate.headwayErrorCoefficient"))
    print("new headwayErrorCoefficient:%s (diff=%s)" %
          (headwayErrorCoefficient, headwayErrorCoefficient - new_headwayErrorCoefficient))

    new_speedDifferenceChangePerceptionThreshold = 0.55
    traci.vehicle.setParameter(vehID, "device.driverstate.speedDifferenceChangePerceptionThreshold",
                               str(new_speedDifferenceChangePerceptionThreshold))
    speedDifferenceChangePerceptionThreshold = float(traci.vehicle.getParameter(
        vehID, "device.driverstate.speedDifferenceChangePerceptionThreshold"))
    print("new speedDifferenceChangePerceptionThreshold:%s (diff=%s)" % (speedDifferenceChangePerceptionThreshold,
                                                                         speedDifferenceChangePerceptionThreshold -
                                                                         new_speedDifferenceChangePerceptionThreshold))

    new_headwayChangePerceptionThreshold = 0.66
    traci.vehicle.setParameter(vehID, "device.driverstate.headwayChangePerceptionThreshold",
                               str(new_headwayChangePerceptionThreshold))
    headwayChangePerceptionThreshold = float(traci.vehicle.getParameter(
        vehID, "device.driverstate.headwayChangePerceptionThreshold"))
    print("new headwayChangePerceptionThreshold:%s (diff=%s)" %
          (headwayChangePerceptionThreshold, headwayChangePerceptionThreshold - new_headwayChangePerceptionThreshold))

    new_awareness = 0.77
    traci.vehicle.setParameter(vehID, "device.driverstate.awareness", str(new_awareness))
    awareness = float(traci.vehicle.getParameter(vehID, "device.driverstate.awareness"))
    print("new awareness:%s (diff=%s)" % (awareness, awareness - new_awareness))

    new_errorState = 0.88
    traci.vehicle.setParameter(vehID, "device.driverstate.errorState", str(new_errorState))
    errorState = float(traci.vehicle.getParameter(vehID, "device.driverstate.errorState"))
    print("new errorState:%s (diff=%s)" % (errorState, errorState - new_errorState))

    new_errorNoiseIntensity = 0.99
    traci.vehicle.setParameter(vehID, "device.driverstate.errorNoiseIntensity", str(new_errorNoiseIntensity))
    errorNoiseIntensity = float(traci.vehicle.getParameter(vehID, "device.driverstate.errorNoiseIntensity"))
    print("new errorNoiseIntensity:%s (diff=%s)" % (errorNoiseIntensity, errorNoiseIntensity - new_errorNoiseIntensity))

    new_errorTimeScale = 1.11
    traci.vehicle.setParameter(vehID, "device.driverstate.errorTimeScale", str(new_errorTimeScale))
    errorTimeScale = float(traci.vehicle.getParameter(vehID, "device.driverstate.errorTimeScale"))
    print("new errorTimeScale:%s (diff=%s)" % (errorTimeScale, errorTimeScale - new_errorTimeScale))

    new_maximalReactionTime = 5.55
    traci.vehicle.setParameter(vehID, "device.driverstate.maximalReactionTime", str(new_maximalReactionTime))
    maximalReactionTime = float(traci.vehicle.getParameter(vehID, "device.driverstate.maximalReactionTime"))
    print("new maximalReactionTime:%s (diff=%s)" % (maximalReactionTime, maximalReactionTime - new_maximalReactionTime))

    new_actionStepLength = float(traci.vehicle.getParameter(vehID, "device.driverstate.actionStepLength"))
    print("new actionStepLength:%s" % (new_actionStepLength))

    new_originalReactionTime = 3.0
    traci.vehicle.setParameter(vehID, "device.driverstate.originalReactionTime", str(new_originalReactionTime))
    originalReactionTime = float(traci.vehicle.getParameter(vehID, "device.driverstate.originalReactionTime"))
    print("new originalReactionTime:%s (diff=%s)" %
          (originalReactionTime, originalReactionTime - new_originalReactionTime))

    new_actionStepLength = float(traci.vehicle.getParameter(vehID, "device.driverstate.actionStepLength"))
    print("new actionStepLength:%s" % (new_actionStepLength))

    print("\n")


def run():
    for i in range(100):
        traci.simulationStep()
    resetParams(myVehicle)
    for i in range(100):
        traci.simulationStep()
    printParams(myVehicle)


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
    traci.start([sumoBinary, "-n", "input_net.net.xml", "-r",
                 "input_routes.rou.xml", "--no-step-log", "true",
                 "--default.speeddev", "0"])

    # Wait until the vehicle enters
    while myVehicle not in traci.vehicle.getIDList():
        traci.simulationStep()

    printParams(myVehicle)

    run()

    traci.close()
    sys.stdout.flush()
