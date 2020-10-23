#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
import sumolib  # noqa
import traci.constants as tc  # noqa


traci.start([sumolib.checkBinary("sumo"),
             '-n', 'input_net2.net.xml',
             '--no-step-log', '-S', '-Q'])

red = (255, 0, 0)
traci.poi.add("poi0", 0, 0, red)
traci.polygon.add("poly0", ((0, 0), (1, 1)), red)

traci.polygon.subscribeContext("poly0", tc.CMD_GET_POI_VARIABLE, dist=20)

traci.simulationStep()
print("pois in range", traci.polygon.getContextSubscriptionResults("poly0"))
traci.poi.add("poi1", 1, 1, red)
print("added poi1")
traci.simulationStep()
print("pois in range", traci.polygon.getContextSubscriptionResults("poly0"))
traci.poi.remove("poi0")
print("removed poi0")
traci.simulationStep()
print("pois in range", traci.polygon.getContextSubscriptionResults("poly0"))

traci.poi.add("poi2", 0, 0, red)
traci.poi.subscribeContext("poi2", tc.CMD_GET_POLYGON_VARIABLE, dist=20)
traci.simulationStep()
print("polys in range", traci.poi.getContextSubscriptionResults("poi2"))
traci.polygon.add("poly1", ((0, 1), (1, 0)), red)
print("added poly1")
traci.simulationStep()
print("polys in range", sorted(traci.poi.getContextSubscriptionResults("poi2").items()))
traci.polygon.remove("poly1")
print("removed poly1")
traci.simulationStep()
print("polys in range", traci.poi.getContextSubscriptionResults("poi2"))

traci.close()
