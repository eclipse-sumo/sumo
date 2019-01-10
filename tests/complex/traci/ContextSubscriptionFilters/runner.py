#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-10-19
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa

DELTA_T = 1000

if sys.argv[1] == "sumo":
    sumoCall = [os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))]
else:
    sumoCall = [os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui')), '-S', '-Q']


def runSingle(traciEndTime, viewRange, module, objID, filterID):
    step = 0
    traci.start(sumoCall + ["-c", "sumo.sumocfg"])

    subscribed = False
    while not step > traciEndTime:
        responses = traci.simulationStep()
        near1 = set()
        if objID in module.getContextSubscriptionResults():
            for v in module.getContextSubscriptionResults()[objID]:
                print(objID, "context:", v)
                near1.add(v)

        if not subscribed:
            print("Subscribing to %s context of object '%s'" % (sys.argv[3], objID))
            module.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange, [
                                    traci.constants.VAR_POSITION])
            print("Adding subscription filter '%s'" % (filterID))
            sys.stdout.flush()
            if filterID == "lanes":
                module.addSubscriptionFilterLanes([-1, 0, 1, 100])
            elif filterID is "noOpposite":
                module.addSubscriptionFilterNoOpposite()
            elif filterID is "downstreamDistance":
                module.addSubscriptionFilterDownstreamDistance(25.)
            elif filterID is "upstreamDistance":
                module.addSubscriptionFilterUpstreamDistance(10.)
            elif filterID is "CFManeuver":
                module.addSubscriptionFilterCFManeuver()
            elif filterID is "LCManeuver":
                module.addSubscriptionFilterLCManeuver()
            elif filterID is "turnManeuver":
                module.addSubscriptionFilterTurnManeuver()
            elif filterID is "vClass":
                module.addSubscriptionFilterVClass()
            elif filterID is "vType":
                module.addSubscriptionFilterVType()
            else:
                print("Error: filterID '%s' not known" % filterID)
            subscribed = True
        step += 1

    module.unsubscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work")
    else:
        print("Ok: Unsubscribe successful")
    print("Print ended at step %s" %
          (traci.simulation.getCurrentTime() / DELTA_T))
    traci.close()
    sys.stdout.flush()


sys.stdout.flush()
if sys.argv[3] == "vehicle":
    runSingle(1000, float(sys.argv[2]), traci.vehicle, "ego", sys.argv[4])
elif sys.argv[3] == "edge":
    runSingle(1000, float(sys.argv[2]), traci.edge, "1fi", sys.argv[4])
elif sys.argv[3] == "lane":
    runSingle(1000, float(sys.argv[2]), traci.lane, "2si_0", sys.argv[4])
elif sys.argv[3] == "junction":
    runSingle(1000, float(sys.argv[2]), traci.junction, "0", sys.argv[4])
elif sys.argv[3] == "poi":
    runSingle(1000, float(sys.argv[2]), traci.poi, "poi", sys.argv[4])
elif sys.argv[3] == "polygon":
    runSingle(1000, float(sys.argv[2]), traci.polygon, "poly", sys.argv[4])
elif sys.argv[3] == "person":
    runSingle(1000, float(sys.argv[2]), traci.person, "p0", sys.argv[4])
