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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("junctions", traci.junction.getIDList())
print("junction count", traci.junction.getIDCount())
junctionID = "0"
print("examining", junctionID)
print("pos", traci.junction.getPosition(junctionID))
print("pos3D", traci.junction.getPosition(junctionID, True))
traci.junction.setParameter(junctionID, "foo", "42")
print("parameter", traci.junction.getParameter(junctionID, "foo"))
print("incomingEdges", traci.junction.getIncomingEdges(junctionID))
print("outgoingEdges", traci.junction.getOutgoingEdges(junctionID))

traci.junction.subscribe(junctionID)
print(traci.junction.getSubscriptionResults(junctionID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.junction.getSubscriptionResults(junctionID))
traci.close()
