#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    debug.py
# @author  Jakob Erdmann
# @date    2017
# @version $Id$

import os,sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], '/home/kobo/dlr/sumo/tools'))
import traci

traci.start(['sumo', '-c', 'test.sumocfg', '--no-step-log'])
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        print(traci.simulation.getCurrentTime() / 1000.0,
                traci.vehicle.getDistance('Linie'),
                traci.vehicle.getRoadID('Linie'))
    except traci.TraCIException:
        pass
traci.close()

