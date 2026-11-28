#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the
# Eclipse Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import traci.constants as tc  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net.net.xml",
             "--no-step-log",
             "--begin", "0",
             "--end", "1"])


def roundShape(shape):
    return [tuple(round(value, 2) for value in point) for point in shape]


print("junction A shape", roundShape(traci.junction.getShape("A")))
print("junction A shape3D", roundShape(traci.junction.getShape3D("A")))
print("junction B shape3D", roundShape(traci.junction.getShape3D("B")))
print("junction C shape3D", roundShape(traci.junction.getShape3D("C")))
position = traci.junction.getPosition("B", True)
if not (traci.isLibsumo() or traci.isLibtraci()):
    assert traci.junction.getPosition3D("B") == position
print("junction B position3D", tuple(round(v, 2) for v in position))
print("lane shape", roundShape(traci.lane.getShape("E_0")))
print("lane shape3D", roundShape(traci.lane.getShape3D("E_0")))
traci.lane.subscribe("E_0", [tc.VAR_SHAPE3D])
traci.junction.subscribe("B", [tc.VAR_SHAPE3D])
traci.junction.subscribe("C", [tc.VAR_SHAPE3D])
traci.simulationStep()
print("lane subscription3D", roundShape(traci.lane.getSubscriptionResults("E_0")[tc.VAR_SHAPE3D]))
for junction in ("B", "C"):
    print("junction %s subscription3D" % junction,
          roundShape(traci.junction.getSubscriptionResults(junction)[tc.VAR_SHAPE3D]))

traci.close()
