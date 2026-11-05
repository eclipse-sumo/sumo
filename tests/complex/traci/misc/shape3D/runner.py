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
             "-n", "input_net3d.net.xml",
             "--no-step-log",
             "--begin", "0",
             "--end", "1"])


def shape2D(shape):
    return [(round(p[0], 2), round(p[1], 2)) for p in shape]


def shape3D(shape):
    return [(round(p[0], 2), round(p[1], 2), round(p[2], 2)) for p in shape]


print("junction A shape", shape2D(traci.junction.getShape("A")))
print("junction A shape3D", shape3D(traci.junction.getShape3D("A")))
print("junction B shape3D", shape3D(traci.junction.getShape3D("B")))
print("junction B position3D", tuple(round(v, 2) for v in traci.junction.getPosition3D("B")))
print("lane shape", shape2D(traci.lane.getShape("E_0")))
print("lane shape3D", shape3D(traci.lane.getShape3D("E_0")))
traci.lane.subscribe("E_0", [tc.VAR_SHAPE3D])
traci.simulationStep()
print("lane subscription3D", shape3D(traci.lane.getSubscriptionResults("E_0")[tc.VAR_SHAPE3D]))

traci.close()
