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

sumoBinary = os.environ["SUMO_BINARY"]

cmd = [
    sumoBinary,
    '-n', 'input_net2.net.xml',
    '-r', 'input_routes.rou.xml',
    '--no-step-log', ]

traci.start(cmd)
traci.simulationStep()  # insert vehicles
vehs = ['stratLeft', 'stratRight']
for vehID in vehs:
    traci.vehicle.setLaneChangeMode(vehID, 0)

for i in range(8):
    t = traci.simulation.getTime()
    for vehID in vehs:
        for dir in [-1, 1]:
            s, sTraci = traci.vehicle.getLaneChangeStatePretty(vehID, dir)
            print("%s v=%s d=%s s=%s sTraci=%s" % (t, vehID, dir, s, sTraci))
    traci.simulationStep()
    print()
traci.close()
