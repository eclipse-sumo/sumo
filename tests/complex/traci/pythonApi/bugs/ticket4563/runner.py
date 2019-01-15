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


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             "--fcd-output", "fcd.xml",
             "--fcd-output.signals",
             "--step-length", "0.2",
             "--default.speeddev", "0",
             ])

lead = "lead"
follow = "follow"

changeDist = 10
tryChangeTime = None
tryChangeDuration = 5

for i in range(10000):
    step()
    dist = traci.vehicle.getLanePosition(lead) - traci.vehicle.getLanePosition(follow)
    if dist < changeDist:
        print("requesting changeLane down for vehicle '%s' at time %s. Remaining distance: %s" %
              (follow, traci.simulation.getTime(), dist))
        traci.vehicle.changeLane(follow, 0, tryChangeDuration)
        tryChangeTime = traci.simulation.getTime()
        break

while traci.simulation.getMinExpectedNumber() > 0 and traci.vehicle.getLaneIndex(follow) > 0:
    t = step()
    if (t - tryChangeTime) > tryChangeDuration:
        # failed. try again
        print("requesting changeLane down for vehicle '%s' at time %s. Remaining distance: %s" %
              (follow, traci.simulation.getTime(), dist))
        traci.vehicle.changeLane(follow, 0, tryChangeDuration)
        tryChangeTime = traci.simulation.getTime()

# done
traci.close()
