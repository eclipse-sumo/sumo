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
# @author  Leonhard Luecken
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
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


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])

lead = "lead"
follow = "follow"

stopPos = 200
slowDownDist = 10

for i in range(10000):
    step()
    remDist = stopPos - traci.vehicle.getLanePosition(follow)
    if remDist < slowDownDist:
        # stop after distance remDist (which is the stop position for the leader
        # (mimics glosa situation in front of traffic light where this problem was observed))
        # d = v*v/(2*a) => a = v*v/(2*d) => tstop = v/a = 2*d/v
        speed = traci.vehicle.getSpeed(follow)
        tstop = 2*remDist / speed
        print("Commanding slow down for vehicle '%s' at time %s. Remaining distance: %s, speed: %s" %
              (follow, traci.simulation.getTime(), remDist, speed))
        traci.vehicle.slowDown(follow, 0, tstop)
        break

while traci.simulation.getMinExpectedNumber() > 0:
    step()
# done
traci.close()
