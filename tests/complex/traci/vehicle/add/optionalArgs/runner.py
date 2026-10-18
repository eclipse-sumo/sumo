#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
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
# @author  Dong Kemu
# @date    2026-10-18

from __future__ import print_function
import contextlib
import io
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


with contextlib.redirect_stdout(io.StringIO()):
    traci.start([sumolib.checkBinary("sumo"),
                 "-n", "input_net2.net.xml",
                 "--no-step-log"] + sys.argv[1:])

traci.route.add("r", ["SC", "NC"])
traci.vehicle.add("v2", "r", depart=0, departPosLat="1.0", arrivalPosLat="1.0",
                  speedFactor=1.2, insertionChecks="all")
assert abs(traci.vehicle.getSpeedFactor("v2") - 1.2) < 1e-9

traci.vehicle.add("v3", "r", depart=0)
assert traci.vehicle.getSpeedFactor("v3") > 0

print("speedFactor: 1.2")
print("default add ok")
traci.close()