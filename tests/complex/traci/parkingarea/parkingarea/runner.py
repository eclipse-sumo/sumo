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
# @author  Jakob Erdmann
# @date    2020-03-16


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net2.net.xml',
             '-a', 'input_additional2.add.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log',
             ])

print("parkingareas", traci.parkingarea.getIDList())
print("parkingarea count", traci.parkingarea.getIDCount())
print("stop attributes:")
for stop in traci.parkingarea.getIDList():
    print("  stop=%s lane=%s startPos=%s endPos=%s name=%s" % (
        stop,
        traci.parkingarea.getLaneID(stop),
        traci.parkingarea.getStartPos(stop),
        traci.parkingarea.getEndPos(stop),
        traci.parkingarea.getName(stop)))

for step in range(50):
    if step % 5 == 0:
        print("time:", traci.simulation.getTime())
        for stop in traci.parkingarea.getIDList():
            print("  stop=%s vC=%s vIDs=%s" % (
                stop,
                traci.parkingarea.getVehicleCount(stop),
                traci.parkingarea.getVehicleIDs(stop)))
    traci.simulationStep()

traci.close()
