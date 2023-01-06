#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa


sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes2.rou.xml,input_routes.rou.xml",
             "--no-step-log",
             "--stop-output", "stops.xml",
             "--device.taxi.dispatch-algorithm", "traci",
             ] + sys.argv[1:])

step = 10
while step <= 10000:
    traci.simulationStep(step)
    reservations = [x.id for x in traci.person.getTaxiReservations(0)]
    # check if some reservation not assigned
    if "5" in reservations:
        print("dispatch for 5")
        traci.vehicle.dispatchTaxi('taxiC', ['3', '4', '0', '5', '3', '1', '1', '5', '2', '4'])
        break
    elif "4" in reservations:
        print("dispatch for 4")
        traci.vehicle.dispatchTaxi('taxiC', ['0', '2', '3', '4', '0', '3', '1', '1', '2', '4'])
    elif "1" in reservations:
        print("dispatch for 1")
        traci.vehicle.dispatchTaxi('taxiC', ['0', '2', '3', '0', '3', '1', '1', '2'])
    elif "0" in reservations:
        print("dispatch for 0")
        traci.vehicle.dispatchTaxi('taxiC', ['0', '0'])
    step += 30


traci.close()
