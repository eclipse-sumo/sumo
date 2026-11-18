#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
# @date    2020-01-01

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log"] + [a for a in sys.argv[1:] if not a.startswith("--meso")])

for _ in range(3):
    traci.simulationStep()

keys = ["cc%d" % i for i in range(10)]
print("initial v0:", [traci.vehicle.getParameter("v0", "carFollowModel." + k) for k in keys])

updates = {
    "cc0": "3.2",
    "cc1": "1.8",
    "cc2": "6.5",
    "cc3": "-10.0",
    "cc4": "-0.3",
    "cc5": "0.4",
    "cc6": "5.5",
    "cc7": "0.3",
    "cc8": "2.6",
    "cc9": "1.9",
}
for k, val in sorted(updates.items()):
    traci.vehicle.setParameter("v0", "carFollowModel." + k, val)

print("updated v0:", [traci.vehicle.getParameter("v0", "carFollowModel." + k) for k in keys])
print("v0 minGap/tau/accel:",
      traci.vehicle.getMinGap("v0"),
      traci.vehicle.getTau("v0"),
      traci.vehicle.getAccel("v0"))
print("unchanged v1:", [traci.vehicle.getParameter("v1", "carFollowModel." + k) for k in keys])

try:
    traci.vehicle.setParameter("v0", "carFollowModel.cc10", "1.0")
except traci.TraCIException as e:
    print("caught unknown:", e)

traci.simulationStep()
traci.close()
