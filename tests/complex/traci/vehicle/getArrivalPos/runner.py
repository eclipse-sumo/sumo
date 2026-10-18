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
traci.vehicle.add("v1", "r", depart=0, arrivalPos=20)
assert abs(traci.vehicle.getArrivalPos("v1") - 20.0) < 1e-9

traci.person.add("p1", "SC", pos=10, depart=0)
traci.person.appendWalkingStage("p1", ["NC"], 42.0)
assert abs(traci.person.getArrivalPos("p1") - 42.0) < 1e-9

print("vehicle arrivalPos: 20.0")
print("person arrivalPos: 42.0")
traci.close()