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
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary('sumo'),
             '--no-step-log',
             '-n', 'input_net.net.xml',
             '-r', 'input_routes.rou.xml',
             '-a', 'input_additional.add.xml',
             ])

while traci.simulation.getMinExpectedNumber() > 0:

    traci.simulationStep()
    if traci.simulation.getTime() == 10:
        traci.lane.setAllowed("4_1", "authority")
        traci.lane.setAllowed("4_2", "authority")

    if traci.simulation.getTime() == 300:
        traci.lane.setAllowed("4_1", "passenger")
        traci.lane.setAllowed("4_2", "passenger")
        for veh in traci.edge.getLastStepVehicleIDs("3"):
            # print("updateBestLanes %s" % veh)
            traci.vehicle.updateBestLanes(veh)

traci.simulationStep()
traci.close()
