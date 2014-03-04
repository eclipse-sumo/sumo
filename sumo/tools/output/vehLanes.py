#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    vehlanes.py
@author  Jakob Erdmann
@date    2012-11-20
@version $Id: vehlanes.py 15692 2014-02-22 09:17:02Z behrisch $

Compute a vehroute-like output for lanes based on a sumo netstate-dump

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os,sys
from collections import defaultdict
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse

def trackLanes(netstate, out):
    # veh_id -> values
    laneTimes = defaultdict(list)
    arrivals = {}
    running = set()

    with open(out, 'w') as f:
        f.write("<vehlanes>\n")

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
                                        laneTimes[vehicle.id].append((timestep.time, lane.id))
                                        running.add(vehicle.id)
            for veh_id in running:
                if not veh_id in seen:
                    arrivals[veh_id] = timestep.time
            running = running - set(arrivals.keys())

        for veh_id, lt in laneTimes.items():
            f.write('    <vehicle id="%s" laneTimes="%s" arrival="%s"/>\n' % (
                veh_id, ' '.join(['%s,%s' % (t,l) for t,l in lt]), arrivals.get(veh_id)))
        f.write("</routeDiff>\n")

if __name__ == "__main__":
    trackLanes(*sys.argv[1:])
