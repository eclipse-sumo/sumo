#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2023-2023 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer
# @date    2023-01-12


from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
tlsID = "C"
phases = []
phases.append(traci.trafficlight.Phase(5, "GGGggrrrrrGGGggrrrrr", 0, 0))
phases.append(traci.trafficlight.Phase(25, "rrrrrrrrrrrrrrrrrrrr", 0, 0))
logic = traci.trafficlight.Logic("custom", 0, 0, phases)
step = 0
while traci.simulation.getMinExpectedNumber() > 0 and step < 90:
    if step % 30 == 0:
        traci.trafficlight.setProgramLogic(tlsID, logic)
        traci.trafficlight.setPhase(tlsID, 0)
    traci.simulationStep()
    step += 1
traci.close()
