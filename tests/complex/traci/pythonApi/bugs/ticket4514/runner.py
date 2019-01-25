#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

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

traci.close()
