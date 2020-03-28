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
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import math

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import sumolib  # noqa
import traci  # noqa


def dist2(v, w):
    return (v[0] - w[0]) ** 2 + (v[1] - w[1]) ** 2


def distToSegmentSquared(p, v, w):
    l2 = dist2(v, w)
    if l2 == 0:
        return dist2(p, v)
    t = ((p[0] - v[0]) * (w[0] - v[0]) + (p[1] - v[1]) * (w[1] - v[1])) / l2
    if t < 0:
        return dist2(p, v)
    if t > 1:
        return dist2(p, w)
    return dist2(p, (v[0] + t * (w[0] - v[0]), v[1] + t * (w[1] - v[1])))


def runSingle(traciEndTime, viewRange, module, objID):
    seen1 = 0
    seen2 = 0
    step = 0
    traci.start([sumolib.checkBinary(sys.argv[1]), '-Q', "-c", "sumo.sumocfg"])
    traci.poi.add("poi", 400, 500, (1, 0, 0, 0))
    traci.polygon.add("poly", ((400, 400), (450, 400), (450, 400)), (1, 0, 0, 0))
    subscribed = False
    while not step > traciEndTime:
        # print(step)
        responses = traci.simulationStep()
        near1 = set()
        if objID in module.getAllContextSubscriptionResults():
            for v in module.getContextSubscriptionResults(objID):
                near1.add(v)
            # print(objID, "context:", sorted(near1))
        vehs = traci.vehicle.getIDList()
        persons = traci.person.getIDList()
        pos = {}
        for v in vehs:
            pos[v] = traci.vehicle.getPosition(v)
        for p in persons:
            pos[p] = traci.person.getPosition(p)
        shape = None
        egoPos = None
        if hasattr(module, "getPosition"):
            egoPos = module.getPosition(objID)
        elif hasattr(module, "getShape"):
            shape = module.getShape(objID)
        elif module == traci.edge:
            # it's a hack, I know,  but do we really need to introduce
            # edge.getShape?
            shape = traci.lane.getShape(objID + "_0")
        near2 = set()
        for v in pos:
            if egoPos:
                if math.sqrt(dist2(egoPos, pos[v])) < viewRange:
                    near2.add(v)
            if shape:
                lastP = shape[0]
                for p in shape[1:]:
                    if math.sqrt(distToSegmentSquared(pos[v], lastP, p)) < viewRange:
                        near2.add(v)
                    lastP = p

        if not subscribed:
            module.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange,
                                    [traci.constants.VAR_POSITION])
            module.subscribeContext(objID, traci.constants.CMD_GET_PERSON_VARIABLE, viewRange,
                                    [traci.constants.VAR_POSITION])
            subscribed = True
        else:
            seen1 += len(near1)
            seen2 += len(near2)
            for v in near1:
                if v not in near2:
                    print("timestep %s: %s is missing in surrounding objects" % (step, v))
            for v in near2:
                if v not in near1:
                    print("timestep %s: %s is missing in subscription results" % (step, v))

        step += 1
    module.unsubscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange)
    responses = traci.simulationStep()
    print([r[0] for r in responses])  # person subscription should still be active
    module.unsubscribeContext(objID, traci.constants.CMD_GET_PERSON_VARIABLE, viewRange)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work", responses)
    else:
        print("Ok: Unsubscribe successful")
    print("Print ended at step %s" % traci.simulation.getTime())
    traci.close()
    sys.stdout.flush()
    print("uncheck: seen %s vehicles via subscription, %s in surrounding" %
          (seen1, seen2))
    if seen1 == seen2:
        print("Ok: Subscription and computed are same")
    else:
        print("Error: subscribed number and computed number differ")


sys.stdout.flush()
if sys.argv[3] == "vehicle":
    runSingle(1000, float(sys.argv[2]), traci.vehicle, "ego")
elif sys.argv[3] == "edge":
    runSingle(1000, float(sys.argv[2]), traci.edge, "1fi")
elif sys.argv[3] == "lane":
    runSingle(1000, float(sys.argv[2]), traci.lane, "2si_0")
elif sys.argv[3] == "junction":
    runSingle(1000, float(sys.argv[2]), traci.junction, "0")
elif sys.argv[3] == "poi":
    runSingle(1000, float(sys.argv[2]), traci.poi, "poi")
elif sys.argv[3] == "polygon":
    runSingle(1000, float(sys.argv[2]), traci.polygon, "poly")
elif sys.argv[3] == "person":
    runSingle(1000, float(sys.argv[2]), traci.person, "p0")
