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
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

VERBOSE = False
COLOR_RESULTS = True
surroundingVehicleColor = (64, 224, 208)

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

if sys.argv[1] == "sumo":
    sumoCall = [sumolib.checkBinary('sumo')]
else:
    sumoCall = [sumolib.checkBinary('sumo-gui'), '-S', '-Q']

egoID = "ego"


def colorResults(surroundingVehs):
    global coloredVehs
    arrived = traci.simulation.getArrivedIDList()
    toRemoveFromColored = []
    if VERBOSE:
        print("# coloredVehs:")
    for v, origCol in coloredVehs.items():
        if VERBOSE:
            print(v)
        if v not in surroundingVehs:
            toRemoveFromColored.append(v)
            if v not in arrived:
                traci.vehicle.setColor(v, origCol)
    if VERBOSE:
        print("# toRemoveFromColored:")
    for v in toRemoveFromColored:
        coloredVehs.pop(v)
        if VERBOSE:
            print(v)
    if VERBOSE:
        print("# new in coloredVehs:")
    for v in surroundingVehs:
        if v not in coloredVehs and v != "ego":
            if VERBOSE:
                print(v)
            coloredVehs[v] = traci.vehicle.getColor(v)
            traci.vehicle.setColor(v, surroundingVehicleColor)


def runSingle(traciEndTime, lateralDist, downstreamDist, upstreamDist, foeDistToJunction):
    global coloredVehs
    step = 0
    traci.start(sumoCall + ["-n", "input_net.net.xml", "-r", "input_routes.rou.xml", "--no-step-log", "true"])
    subscribed = False
    coloredVehs = {}
    while not step > traciEndTime:
        responses = traci.simulationStep()
        if subscribed:
            print("Step %s:\nContext results for veh '%s':" % (step, egoID))
            results = traci.vehicle.getContextSubscriptionResults(egoID)
            surroundingVehs = []
            for v in sorted(results or []):
                print(v)
                surroundingVehs += [v]
            if COLOR_RESULTS:
                colorResults(surroundingVehs)
            if egoID in traci.simulation.getArrivedIDList():
                break

        if not subscribed:
            print("Subscribing to context of vehicle '%s'" % (egoID))
            traci.vehicle.subscribeContext(egoID, traci.constants.CMD_GET_VEHICLE_VARIABLE, 0.0,
                                           [traci.constants.VAR_POSITION])
            print("Adding turn filter ... \n(downstreamDist=%s, foeDistToJunction=%s)" %
                  (downstreamDist, foeDistToJunction))
            sys.stdout.flush()
            traci.vehicle.addSubscriptionFilterTurn(downstreamDist, foeDistToJunction)
            print("Adding lateral distance subscription filter ... " +
                  "(lateralDistance=%s, downstreamDistance=%s, upstreamDistance=%s)" %
                  (lateralDist, downstreamDist, upstreamDist))
            traci.vehicle.addSubscriptionFilterLateralDistance(lateralDist, downstreamDist, upstreamDist)
            subscribed = True
        step += 1

    if step > traciEndTime:
        traci.vehicle.unsubscribeContext(egoID, traci.constants.CMD_GET_VEHICLE_VARIABLE, 0.0)
        responses = traci.simulationStep()
        if responses:
            print("Error: Unsubscribe did not work")
        else:
            print("Ok: Unsubscribe successful")
    print("Print ended at step %s" % traci.simulation.getTime())
    traci.close()
    sys.stdout.flush()


if len(sys.argv) < 6:
    print("Usage: runner <sumo/sumo-gui> <lateralDistance> <downstreamDistance> <upstreamDistance> <foeDistToJunction>")
    sys.exit("")
sys.stdout.flush()
runSingle(100, float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5]))
