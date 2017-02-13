#!/usr/bin/env python
"""
@file    runner.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2012-10-19
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import time
import math

sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib
import traci

DELTA_T = 1000

if sys.argv[1] == "sumo":
    sumoCall = [os.environ.get(
        "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))]
else:
    sumoCall = [os.environ.get(
        "GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui')), '-S', '-Q']


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
    traci.start(sumoCall + ["-c", "sumo.sumocfg"])
    traci.poi.add("poi", 400, 500, (1, 0, 0, 0))
    traci.polygon.add(
        "poly", ((400, 400), (450, 400), (450, 400)), (1, 0, 0, 0))
    subscribed = False
    while not step > traciEndTime:
        responses = traci.simulationStep()
        near1 = set()
        if objID in module.getContextSubscriptionResults():
            for v in module.getContextSubscriptionResults()[objID]:
                near1.add(v)
        vehs = traci.vehicle.getIDList()
        pos = {}
        for v in vehs:
            pos[v] = traci.vehicle.getPosition(v)
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
            module.subscribeContext(objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange, [
                                    traci.constants.VAR_POSITION])
            subscribed = True
        else:
            seen1 += len(near1)
            seen2 += len(near2)
            for v in near1:
                if v not in near2:
                    print(
                        "timestep %s: %s is missing in subscribed vehicles" % (step, v))
            for v in near2:
                if v not in near1:
                    print(
                        "timestep %s: %s is missing in surrounding vehicles" % (step, v))

        step += 1
    module.unsubscribeContext(
        objID, traci.constants.CMD_GET_VEHICLE_VARIABLE, viewRange)
    responses = traci.simulationStep()
    if responses:
        print("Error: Unsubscribe did not work")
    else:
        print("Ok: Unsubscribe successful")
    print("Print ended at step %s" %
          (traci.simulation.getCurrentTime() / DELTA_T))
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
