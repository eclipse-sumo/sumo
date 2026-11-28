#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2024-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-19

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

traci.start([
    sumolib.checkBinary("sumo"),
    "-n", "input_net.net.xml",
    "-r", "input_routes.rou.xml",
    "--no-step-log",
] + sys.argv[1:])

for _ in range(15):
    traci.simulationStep()
traci.simulation.saveState("saved_state.xml")

for _ in range(25):
    traci.simulationStep()

traces = []
for run in range(5):
    traci.simulation.loadState("saved_state.xml")
    for _ in range(20):
        traci.simulationStep()
    snap = (
        traci.simulation.getTime(),
        tuple((v, round(traci.vehicle.getLanePosition(v), 2), traci.vehicle.getRoadID(v))
              for v in traci.vehicle.getIDList()),
        tuple((s, traci.trafficlight.getRedYellowGreenState(s))
              for s in sorted(traci.trafficlight.getIDList())),
        tuple(sorted(traci.vehicletype.getIDList())),
    )
    print("reload %d: t=%s vehs=%s tls=%s" % (run, snap[0], snap[1], snap[2]))
    traces.append(snap)

assert len(set(traces)) == 1
traci.close()
