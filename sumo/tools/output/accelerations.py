#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    accelerations.py
@author  Jakob Erdmann
@date    2012-11-20
@version $Id$

Compute acceleration statistics from a sumo netstate-dump

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse_fast
from sumolib.miscutils import Statistics


def accelStats(netstate):
    lastSpeed = {}
    stats = Statistics(
        "Accelerations", histogram=True, printMin=True, scale=0.2)
    for vehicle in parse_fast(netstate, 'vehicle', ['id', 'speed']):
        speed = float(vehicle.speed)
        prevSpeed = lastSpeed.get(vehicle.id, speed)
        stats.add(speed - prevSpeed, (vehicle.id, vehicle.speed))
        lastSpeed[vehicle.id] = speed
    print(stats)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("call %s <netstate-dump>" % sys.argv[0])
    accelStats(*sys.argv[1:])
