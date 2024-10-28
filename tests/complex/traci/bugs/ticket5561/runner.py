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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary("sumo"), '-c', 'sumo.sumocfg'])

step = 0
done = False
bus_id = "bus"

while traci.simulation.getMinExpectedNumber() > 0:
    next_stops = traci.vehicle.getStops(bus_id)
    if len(next_stops) > 0:
        stop = next_stops[0]
        bus_stop_pos = stop.endPos
        if stop.stoppingPlaceID == "busStop2" and traci.vehicle.getLaneID(bus_id) == stop.lane:
            bus_pos = traci.vehicle.getLanePosition(bus_id)
            if bus_stop_pos - bus_pos < 10:
                traci.vehicle.setBusStop(bus_id, "busStop2", 0)
                print("abort stop at busStop2 at t=%s" % traci.simulation.getTime())
    traci.simulationStep()
traci.close()
