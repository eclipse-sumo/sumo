#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Jakob Erdmann
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             '-c', 'sumo.sumocfg',
             '-n', 'net.net.xml',
             '-a', 'input_additional.add.xml,input_additional2.add.xml,input_additional3.add.xml',
             '--vehroute-output', 'vehroutes.xml',
             # '-S', '-Q',
             ])

egoID = "ego.0"


def check(vehID):
    print("time:", traci.simulation.getTime())
    for param in [
            "parking.rerouteCount",
            "parking.memory.IDList",
            "parking.memory.score",
            "parking.memory.blockedAtTime",
            "parking.memory.blockedAtTimeLocal",
            ]:
        print("  ", param, ":", traci.vehicle.getParameter(vehID, param))


traci.simulationStep()
check("dummy")

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    time = traci.simulation.getTime()
    if time in [10, 30]:
        check(egoID)
    if time == 20:
        try:
            traci.vehicle.getParameter(egoID, "parking.memory.foobar")
        except traci.TraCIException:
            pass

traci.close()
