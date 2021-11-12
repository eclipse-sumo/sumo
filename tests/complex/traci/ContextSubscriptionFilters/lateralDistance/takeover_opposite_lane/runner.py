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

sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib  # noqa
import traci  # noqa

if sys.argv[1] == "sumo":
    sumoCall = [sumolib.checkBinary('sumo')]
else:
    sumoCall = [sumolib.checkBinary('sumo-gui')]  # , '-S', '-Q']

egoID = "ego"


def runSingle(traciEndTime, range, lateralDistance, downstreamDistance, upstreamDistance, testWithIncompatibleFilter):
    step = 0
    traci.start(sumoCall + ["-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
                            "--no-step-log", "true", '--lanechange.duration', str(3.0)])
    subscribed = False
    while not step > traciEndTime:
        responses = traci.simulationStep()
        near1 = set()
        if subscribed:
            print("[%02ds] Context results for veh '%s':" % (step, egoID))
            results = traci.vehicle.getContextSubscriptionResults(egoID)
            if results == {}:
                print("ERROR: results empty")
            elif 'ego' not in results:
                print("ERROR: 'ego' not in results")
            for v in sorted(results):
                print(v)
                near1.add(v)

        if not subscribed:
            print("Subscribing to context of vehicle '%s' (range=%s)" % (egoID, range))
            traci.vehicle.subscribeContext(egoID, traci.constants.CMD_GET_VEHICLE_VARIABLE,
                                           range, [traci.constants.VAR_POSITION])
            print("Adding lateral distance subscription filter ... " +
                  "(lateralDistance=%s, downstreamDistance=%s, upstreamDistance=%s)" %
                  (lateralDistance, downstreamDistance, upstreamDistance))
            sys.stdout.flush()
            if testWithIncompatibleFilter:
                traci.vehicle.addSubscriptionFilterFieldOfVision(90.0)
            traci.vehicle.addSubscriptionFilterLateralDistance(lateralDistance, downstreamDistance, upstreamDistance)
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


if len(sys.argv) != 7:
    print("Usage: runner <sumo/sumo-gui> <range> <lateralDistance> <downstreamDistance> <upstreamDistance> " +
          "<testWithIncompatibleFilter (0/1)>")
    sys.exit("")
sys.stdout.flush()
runSingle(15, float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5]), bool(int(sys.argv[6])))
