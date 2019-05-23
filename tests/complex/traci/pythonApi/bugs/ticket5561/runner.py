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
# @date    2017-01-23
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary("sumo"), '-c', 'sumo.sumocfg'])

step = 0
done = False
bus_id = "bus"

while traci.simulation.getMinExpectedNumber() > 0:
    next_stops = traci.vehicle.getNextStops(bus_id)
    if len(next_stops) > 0:
        stop = next_stops[0]
        bus_stop_pos = stop[1]
        if stop[2] == "busStop2" and traci.vehicle.getLaneID(bus_id) == stop[0]:
            bus_pos = traci.vehicle.getLanePosition(bus_id)
            if bus_stop_pos - bus_pos < 10:
                traci.vehicle.setBusStop(bus_id, "busStop2", 0, 0, 0)
                print("abort stop at busStop2 at t=%s" % traci.simulation.getTime())
    traci.simulationStep()
traci.close()
