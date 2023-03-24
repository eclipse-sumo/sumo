#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
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
# @date    2020-10-15

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net2.net.xml",
             "--tripinfo-output", "tripinfo.xml",
             "--vehroute-output", "vehroutes.xml",
             "--no-step-log",
             ] + sys.argv[1:])
pID = "p0"
vID = "v0"
rID = "r0"

traci.route.add(rID, ['SC', 'CN'])
traci.vehicle.add(vID, rID)
traci.person.add(pID, 'SC', 0, depart=tc.DEPARTFLAG_TRIGGERED)
traci.person.appendDrivingStage(pID, 'CN', lines=vID)
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
print("simulation ends at time=%s" % traci.simulation.getTime())

traci.close()
