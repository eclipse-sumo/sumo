#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2020-2021 German Aerospace Center (DLR) and others.
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

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml",
             "--no-step-log",
             ] + sys.argv[1:])

pID = "person0"
firstStage = True
for i in range(0, traci.person.getRemainingStages(pID)):
    stage = traci.person.getStage(pID, i)
    extra = []
    if stage.type == tc.STAGE_WAITING:
        extra = [('actType', 'chew bubblegum')]
    print(stage.toXML(firstStage, extra=extra))
    firstStage = False

print("")
firstStage = True
for stage in traci.simulation.findIntermodalRoute("2/3to1/3", "1/3to0/3",
                                                  destStop="busStop0", departPos=12, modes="bicycle"):
    print(stage.toXML(firstStage))
    firstStage = False

traci.close()
