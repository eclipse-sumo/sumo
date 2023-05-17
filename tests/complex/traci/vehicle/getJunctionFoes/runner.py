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

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo-gui')
traci.start([sumoBinary, "-c", "sumo.sumocfg"] + sys.argv[1:])

egoID = "t_0"
monitoringDistance = 20

for step in range(30):
    traci.simulationStep()
    try:
        print(traci.simulation.getTime(), len(traci.vehicle.getJunctionFoes(egoID, monitoringDistance)))
    except traci.TraCIException as e:
        print(traci.simulation.getTime(), "Error:", e)

traci.close()
