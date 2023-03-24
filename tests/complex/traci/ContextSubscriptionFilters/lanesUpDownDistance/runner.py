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
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


if sys.argv[1] == "sumo":
    sumoCall = [sumolib.checkBinary('sumo')]
else:
    sumoCall = [sumolib.checkBinary('sumo-gui'), '-S', '-Q']


def runSingle(traciEndTime, viewRange, laneList, upDist, downDist, vTypes, vClasses, objID):
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
            for v in sorted(traci.vehicle.getContextSubscriptionResults(objID) or []):
                print(v)

        if not subscribed:
            print("Subscribing to vehicle context of object '%s'" % (objID))
            traci.vehicle.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE,
                                           viewRange, [traci.constants.VAR_POSITION])
            sys.stdout.flush()

            traci.vehicle.addSubscriptionFilterLanes(laneList)
            traci.vehicle.addSubscriptionFilterUpstreamDistance(upDist)
            traci.vehicle.addSubscriptionFilterDownstreamDistance(downDist)

            if vTypes:
                traci.vehicle.addSubscriptionFilterVType(vTypes)
            if vClasses:
                traci.vehicle.addSubscriptionFilterVClass(vClasses)

            # advice all vehicle not to change lanes
            for vehID in traci.vehicle.getIDList():
                traci.vehicle.changeLane(vehID, traci.vehicle.getLaneIndex(vehID), 111)

            subscribed = True
        step += 1

    print("Print ended at step %s" % step)
    traci.close()
    sys.stdout.flush()


sys.stdout.flush()
viewRange = float(sys.argv[2])
laneList = laneList = list(map(int, sys.argv[3].strip('[]').split(',')))
upDist = float(sys.argv[4])
downDist = float(sys.argv[5])
vTypes = [x for x in sys.argv[6].split(',') if x] if len(sys.argv) > 6 else []
vClasses = [x for x in sys.argv[7].split(',') if x] if len(sys.argv) > 7 else []

runSingle(100, viewRange, laneList, upDist, downDist, vTypes, vClasses,  "ego")
