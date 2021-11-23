#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa

if sys.argv[1] == "sumo":
    sumoCall = [os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo')), ]
else:
    sumoCall = [os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui')), '-S', '-Q']

egoID = "ego"


def runSingle(traciEndTime, downstreamDist, foeDistToJunction):
    step = 0
    traci.start(sumoCall + ["-n", "input_net3.net.xml", "-r", "input_routes3.rou.xml", "--no-step-log", "true"])
    subscribed = False
    while not step > traciEndTime:
        responses = traci.simulationStep()
        near1 = set()
        if subscribed:
            print("Step %s:\nContext results for veh '%s':" % (step, egoID))
            for v in sorted(traci.vehicle.getContextSubscriptionResults(egoID) or []):
                print(v)
                near1.add(v)
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


if len(sys.argv) < 4:
    print("Usage: runner <sumo/sumo-gui> <downstreamDist> <foeDistToJunction>")
    sys.exit("")
sys.stdout.flush()
runSingle(100, float(sys.argv[2]), float(sys.argv[3]))
