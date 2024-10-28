#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa

if sys.argv[1] == "sumo":
    sumoCall = [sumolib.checkBinary('sumo')]
else:
    sumoCall = [sumolib.checkBinary('sumo-gui'), '-S', '-Q']


def runSingle(traciEndTime, viewRange, objID):
    step = 0
    traci.start(sumoCall + ["-c", "sumo.sumocfg"])

    subscribed = False
    while not step > traciEndTime:
        traci.simulationStep()

        if subscribed:
            # check if objID has arrived at destination
            arrivedList = traci.simulation.getArrivedIDList()
            if objID in arrivedList:
                print("[%03d] Vehicle '%s' has arrived at destination" % (step, objID))
                break

            print("[%03d] Context results for vehicle '%s':" % (step, objID))
            for v in sorted(traci.vehicle.getContextSubscriptionResults(objID)):
                print(v)

        if not subscribed:
            print("Subscribing to vehicle context of object '%s'" % (objID))
            traci.vehicle.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE,
                                           viewRange, [traci.constants.VAR_POSITION])
            sys.stdout.flush()

            traci.vehicle.addSubscriptionFilterLanes([-1, 0, 1])

            subscribed = True
        step += 1

    print("Print ended at %s" % traci.simulation.getTime())
    traci.close()
    sys.stdout.flush()


sys.stdout.flush()
runSingle(100, float(sys.argv[2]), "ego")
