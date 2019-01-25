#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    vehLanes.py
# @author  Jakob Erdmann
# @date    2012-11-20
# @version $Id$

from __future__ import absolute_import
import os
import sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse  # noqa


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
                                    if vehicle.id not in running or laneTimes[vehicle.id][-1][1] != lane.id:
                                        laneTimes[vehicle.id].append(
                                            (timestep.time, lane.id))
                                        running.add(vehicle.id)
                                        if lastEdge[vehicle.id] == edge.id:
                                            laneChanges[vehicle.id] += 1
                                        lastEdge[vehicle.id] = edge.id
            for veh_id in running:
                if veh_id not in seen:
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
