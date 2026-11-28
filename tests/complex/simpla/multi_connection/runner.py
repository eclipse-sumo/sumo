#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-11-26

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa
import simpla  # noqa

binary = sumolib.checkBinary("sumo" if "nogui" in sys.argv else "sumo-gui")

traci.start([binary, "-c", "sumo.sumocfg", "--step-length", "0.5"], label="simA")
traci.start([binary, "-n", "input_net.net.xml", "-r", "input_routes2.rou.xml",
             "--no-step-log", "--step-length", "0.5"], label="simB")

simpla.load("simpla.cfg.xml", connectionLabel="simA")
simpla.load("simpla.cfg.xml", connectionLabel="simB")

for _ in range(50):
    traci.switch("simA")
    traci.simulationStep()
    traci.switch("simB")
    assert traci.getLabel() == "simB"
    traci.simulationStep()

leaders_a = sorted(simpla.getPlatoonLeaderIDList("simA"))
leaders_b = sorted(simpla.getPlatoonLeaderIDList("simB"))
print("simA leaders:", leaders_a)
print("simB leaders:", leaders_b)
assert any(v.startswith("v.") for v in leaders_a), leaders_a
assert any(v.startswith("b.") for v in leaders_b), leaders_b

simpla.stop("simA")
traci.switch("simB")
for _ in range(10):
    traci.simulationStep()
leaders_b_after = sorted(simpla.getPlatoonLeaderIDList("simB"))
print("simB leaders after stopping simA:", leaders_b_after)
assert any(v.startswith("b.") for v in leaders_b_after), leaders_b_after

simpla.stop("simB")
traci.switch("simA")
traci.close()
traci.switch("simB")
traci.close()
