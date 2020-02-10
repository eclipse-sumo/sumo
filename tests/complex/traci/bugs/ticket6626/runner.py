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
# @author  Jakob Erdmann
# @date    2018-09-27

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--default.speeddev", "0",
             "--tripinfo-output", "tripinfos.xml",
             "--no-step-log",
             ])
vehID = "v0"
traci.vehicle.add(vehID, "r0", departLane="1", departSpeed="14")
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()

traci.close()
