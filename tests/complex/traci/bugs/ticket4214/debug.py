#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    debug.py
# @author  Jakob Erdmann
# @date    2017

import os
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], '/home/kobo/dlr/sumo/tools'))
import traci  # noqa

traci.start(['sumo', '-c', 'test.sumocfg', '--no-step-log'])
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        print(
            traci.simulation.getCurrentTime() / 1000.0,
            traci.vehicle.getDistance('Linie'),
            traci.vehicle.getRoadID('Linie'))
    except traci.TraCIException:
        pass
traci.close()
