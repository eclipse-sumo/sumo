#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    accelerations.py
# @author  Jakob Erdmann
# @date    2012-11-20
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import os
import sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.output import parse_fast  # noqa
from sumolib.miscutils import Statistics  # noqa


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
