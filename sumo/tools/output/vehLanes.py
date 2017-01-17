#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    vehLanes.py
@author  Jakob Erdmann
@date    2012-11-20
@version $Id$

Compute a vehroute-like output for lanes based on a sumo netstate-dump

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse


def trackLanes(netstate, out):
    # veh_id -> values
    laneTimes = defaultdict(list)
    laneChanges = defaultdict(lambda: 0)
    lastEdge = defaultdict(lambda: None)
    arrivals = {}
    running = set()

    with open(out, 'w') as f:
        f.write("<vehLanes>\n")

        for timestep in parse(netstate, 'timestep'):
            seen = set()
            if timestep.edge is not None:
                for edge in timestep.edge:
                    if edge.lane is not None:
                        for lane in edge.lane:
                            if lane.vehicle is not None:
                                for vehicle in lane.vehicle:
                                    seen.add(vehicle.id)
                                    if not vehicle.id in running or laneTimes[vehicle.id][-1][1] != lane.id:
                                        laneTimes[vehicle.id].append(
                                            (timestep.time, lane.id))
                                        running.add(vehicle.id)
                                        if lastEdge[vehicle.id] == edge.id:
                                            laneChanges[vehicle.id] += 1
                                        lastEdge[vehicle.id] = edge.id
            for veh_id in running:
                if not veh_id in seen:
                    arrivals[veh_id] = timestep.time
            running = running - set(arrivals.keys())

        for veh_id, lt in laneTimes.items():
            f.write('    <vehicle id="%s" laneTimes="%s" arrival="%s" laneChanges="%s"/>\n' % (
                veh_id,
                ' '.join(['%s,%s' % (t, l) for t, l in lt]),
                arrivals.get(veh_id),
                laneChanges[veh_id]))
        f.write("</vehLanes>\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("call %s <netstate-dump> <output-file>" % sys.argv[0])
    trackLanes(*sys.argv[1:])
