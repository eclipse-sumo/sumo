#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import traci.constants as tc  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
sumoCmd = [sumoBinary,
           '-n', 'input_net.net.xml',
           '-r', 'input_routes.rou.xml',
           '--no-step-log',
           '--begin', '0',
           # '-S', '-Q',
           ] + sys.argv[1:]


vehID = "v1"
traci.start(sumoCmd)
sr = traci.simulationStep(1.)
traci.vehicle.subscribeContext(vehID, tc.CMD_GET_VEHICLE_VARIABLE,
                               dist=50,
                               varIDs=(tc.VAR_SPEED, tc.VAR_EMISSIONCLASS))
sr = traci.simulationStep()
for vehID, response in sr:
    print("t=%s subscriptionResult=%s" % (traci.simulation.getTime(),
                                          sorted(traci.vehicle.getContextSubscriptionResults(vehID).items())))

traci.close()
