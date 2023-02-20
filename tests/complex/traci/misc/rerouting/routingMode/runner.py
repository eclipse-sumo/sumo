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
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), '--device.rerouting.adaptation-steps', '20', '-c', 'sumo.sumocfg'])
traci.simulationStep()
while traci.simulation.getMinExpectedNumber() > 0:
    timeS = traci.simulation.getTime()
    for vehID in traci.simulation.getDepartedIDList():
        if "custom" in sys.argv:
            traci.vehicle.setAdaptedTraveltime(vehID, "detour", 1)
        if "raw" in sys.argv:
            traci.vehicle.rerouteTraveltime(vehID, False)
        elif "aggregated" in sys.argv:
            traci.vehicle.setRoutingMode(vehID, traci.constants.ROUTING_MODE_AGGREGATED)
            traci.vehicle.rerouteTraveltime(vehID)
        else:
            traci.vehicle.rerouteTraveltime(vehID)
        print("step=%s rerouted=%s" % (timeS, vehID))
    # print("step=%s", timeS)
    # for edgeID in traci.edge.getIDList():
    #    print("   edge=%s tt=%s" % (edgeID,
    #        traci.edge.getAdaptedTraveltime(edgeID, timeS)))
    traci.simulationStep()
traci.close()
