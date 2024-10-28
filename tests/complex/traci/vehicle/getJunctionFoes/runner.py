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
# @author  Benjamin Coueraud
# @date    2023-05-17

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary, "-c", "sumo.sumocfg"] + sys.argv[1:])

egoID = "t_0"
monitoringDistance = 20

for step in range(30):
    try:
        traci.simulationStep()
        print('='*100)
        print("Time: %.2f" % traci.simulation.getTime())
        for foe in traci.vehicle.getJunctionFoes(egoID, monitoringDistance):
            print(foe[0])
            for item in foe[1:]:
                if isinstance(item, float):
                    print("\t%.2f" % item)
                else:
                    print("\t%s" % item)
    except traci.TraCIException as e:
        print(traci.simulation.getTime(), "Error:", e)

traci.close()
