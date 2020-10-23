#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("routes", traci.route.getIDList())
print("route count", traci.route.getIDCount())
routeID = "horizontal"
print("examining", routeID)
print("edges", traci.route.getEdges(routeID))
traci.route.subscribe(routeID)
print(traci.route.getSubscriptionResults(routeID))
for step in range(3, 6):
    print("step", step)
    # XXX: This stepping is a bit strange (intentional?)
    traci.simulationStep(step)
    print(traci.route.getSubscriptionResults(routeID))
traci.route.add("h2", ["1o"])
traci.route.add("withTaz", ["taz12-source", "taz34-sink"])
try:
    traci.route.add("empty", [])
except traci.TraCIException as e:
    if traci.isLibsumo():
        print(e, file=sys.stderr)
print("routes", traci.route.getIDList())
print("edges", traci.route.getEdges("h2"))
print("edges", traci.route.getEdges("withTaz"))
traci.close()
