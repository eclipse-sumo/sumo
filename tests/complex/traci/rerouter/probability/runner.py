#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-04-12

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])

print("initial prob:", traci.rerouter.getParameter("rerouter_0", "probability"))

traci.rerouter.setParameter("rerouter_0", "probability", "0.0")
print("prob after set 0.0:", traci.rerouter.getParameter("rerouter_0", "probability"))
traci.simulationStep()
print("v0 route:", traci.vehicle.getRoute("v0"))

for _ in range(4):
    traci.simulationStep()

traci.rerouter.setParameter("rerouter_0", "probability", "1.0")
print("prob after set 1.0:", traci.rerouter.getParameter("rerouter_0", "probability"))
traci.simulationStep()
print("v1 route:", traci.vehicle.getRoute("v1"))

traci.rerouter.setParameter("rerouter_0", "probability", "")
print("prob after reset:", traci.rerouter.getParameter("rerouter_0", "probability"))

for bad_val in ("1.5", "-0.1", "abc"):
    try:
        traci.rerouter.setParameter("rerouter_0", "probability", bad_val)
    except traci.TraCIException:
        print("caught invalid value:", bad_val)

try:
    traci.rerouter.setParameter("rerouter_0", "unknownParam", "1")
except traci.TraCIException:
    print("caught unsupported param")

try:
    traci.rerouter.getParameter("unknownRerouter", "probability")
except traci.TraCIException:
    print("caught unknown rerouter")

traci.close()
