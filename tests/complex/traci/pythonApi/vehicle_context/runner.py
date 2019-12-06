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
import subprocess
import sys
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
sumoCmd = [sumoBinary,
        '-n', 'input_net.net.xml',
        '-r', 'input_routes.rou.xml',
        '--no-step-log',
        '--begin', '0',
        # '-S', '-Q',
        ]
        

vehID = "v1"
traci.start(sumoCmd)
sr = traci.simulationStep(1.)
traci.vehicle.subscribeContext(vehID, tc.CMD_GET_VEHICLE_VARIABLE,
                               dist=50,
                               varIDs=(tc.VAR_SPEED,))
sr = traci.simulationStep()
for vehID, response in sr:
    print("t=%s subscriptionResult=%s" % (traci.simulation.getTime(),
        traci.vehicle.getContextSubscriptionResults(vehID)))

traci.close()
