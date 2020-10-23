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
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sumoHome = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', ".."))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa

if sys.argv[1] == "sumo":
    sumoCall = [sumolib.checkBinary('sumo')]
else:
    sumoCall = [sumolib.checkBinary('sumo-gui')]  # , '-S', '-Q']

egoID = "ego"


def runSingle(traciEndTime, downstreamDist, upstreamDist, lanes, opposite, vTypes, vClasses):
    step = 0
    traci.start(sumoCall + ["-n", "input_net.net.xml", "-r", "input_routes.rou.xml", "--no-step-log", "true"])
    subscribed = False
    while not step > traciEndTime:
        responses = traci.simulationStep()
        near1 = set()
        if subscribed:
            print("Context results for veh '%s':" % egoID)
            for v in sorted(traci.vehicle.getContextSubscriptionResults(egoID) or []):
                print(v)
                near1.add(v)

        if not subscribed:
            print("Subscribing to context of vehicle '%s'" % (egoID))
            traci.vehicle.subscribeContext(egoID, traci.constants.CMD_GET_VEHICLE_VARIABLE, 0.0,
                                           [traci.constants.VAR_POSITION])
            print("""Adding subscription filters ...
(downstreamDist=%s, upstreamDist=%s, lanes=%s, opposite=%s
   vTypes:%s, vClasses:%s)""" % (downstreamDist, upstreamDist, lanes, opposite, vTypes, vClasses))
            sys.stdout.flush()
            traci.vehicle.addSubscriptionFilterDownstreamDistance(downstreamDist)
            traci.vehicle.addSubscriptionFilterUpstreamDistance(upstreamDist)
            traci.vehicle.addSubscriptionFilterLanes(lanes)
            if vClasses:
                traci.vehicle.addSubscriptionFilterVClass(vClasses)
            if vTypes:
                traci.vehicle.addSubscriptionFilterVType(vTypes)
            if not opposite:
                traci.vehicle.addSubscriptionFilterNoOpposite()
            subscribed = True
        step += 1

    traci.vehicle.unsubscribeContext(egoID, traci.constants.CMD_GET_VEHICLE_VARIABLE, 0.0)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work")
    else:
        print("Ok: Unsubscribe successful")
    print("Print ended at step %s" % traci.simulation.getTime())
    traci.close()
    sys.stdout.flush()


if len(sys.argv) < 8:
    print("Usage: runner <sumo/sumo-gui> <downstreamDist> <upstreamDist> <lanes(csv)> " +
          "<opposite{0,1}> <vTypes> <vClasses>")
    sys.exit("")
sys.stdout.flush()
opposite = bool(int(sys.argv[5]))
lanes = [int(s.strip()) for s in sys.argv[4].split(",")]
if sys.argv[6].strip() != "":
    vTypes = [s.strip() for s in sys.argv[6].split(",")]
else:
    vTypes = []
if sys.argv[7].strip() != "":
    vClasses = [s.strip() for s in sys.argv[7].split(",")]
else:
    vClasses = []
runSingle(1, float(sys.argv[2]), float(sys.argv[3]), lanes, opposite, vTypes, vClasses)
