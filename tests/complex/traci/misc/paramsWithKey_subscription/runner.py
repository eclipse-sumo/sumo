#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Jakob Erdmann
# @date    2015-02-06


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


def subscribeParamWithKey(objectType, object, objectID):
    print("subscribing to %s param \'simTime\'" % (objectType))
    object.subscribeParameterWithKey(objectID, "simTime")


def setParam(objectType, object, objectID):
    object.setParameter(objectID, "simTime", str(traci.simulation.getTime()))


def getParamSubscriptionResult(objectType, object, objectID):
    print("%s result: %s" %
          (objectType, object.getSubscriptionResults(objectID)[traci.constants.VAR_PARAMETER_WITH_KEY]))


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
print("step", step())

# XXX test PoI, Polygon
objects = [
    ("vehicle", traci.vehicle, "veh0"),
    ("person", traci.person, "ped0"),
    ("edge", traci.edge, "1o"),
    ("lane", traci.lane, "1o_0"),
    ("vType", traci.vehicletype, "pType"),
    ("route", traci.route, "horizontal"),
    ("trafficlight", traci.trafficlight, "0"),
]

[setParam(*x) for x in objects]
[subscribeParamWithKey(*x) for x in objects]
for i in range(2):
    print("step", step())
    [setParam(*x) for x in objects]
    [getParamSubscriptionResult(*x) for x in objects]

traci.close()
