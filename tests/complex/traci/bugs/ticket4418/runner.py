#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Leonhard Luecken
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

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
