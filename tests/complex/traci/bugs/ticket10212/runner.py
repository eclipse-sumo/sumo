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

cmd = [sumolib.checkBinary("sumo"),
       '-n', 'net.net.xml',
       '-r', 'input_routes.rou.xml',
       '--lateral-resolution', '0.8',
       '--step-length', '0.1',
       '--seed', '1',
       '--no-step-log']

vehID = "car"

traci.start(cmd)
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    if vehID in traci.vehicle.getIDList():
        print("%s right=%s left=%s center=%s" % (
            traci.simulation.getTime(),
            traci.vehicle.getLaneChangeStatePretty(vehID, -1),
            traci.vehicle.getLaneChangeStatePretty(vehID, 1),
            traci.vehicle.getLaneChangeStatePretty(vehID, 0)))

traci.close()
