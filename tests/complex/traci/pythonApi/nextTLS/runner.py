#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Mirko Barthauer (Technische Universitaet Braunschweig)
# @date    2018-09-27
# @version $Id$

from __future__ import print_function
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net2.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             ])

vehID = "v0"
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        print(traci.simulation.getTime(), traci.vehicle.getNextTLS(vehID))
    except traci.TraCIException as e:
        print(traci.simulation.getTime(), e)

traci.close()
