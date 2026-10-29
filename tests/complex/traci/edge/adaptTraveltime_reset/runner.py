#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import traci.constants as tc  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net2.net.xml",
             "--no-step-log"] + sys.argv[1:])

print("initial", traci.edge.getAdaptedTraveltime("SC", 0))

traci.edge.adaptTraveltime("SC", 42)
print("set", traci.edge.getAdaptedTraveltime("SC", 0))

traci.edge.adaptTraveltime("SC", tc.INVALID_DOUBLE_VALUE)
print("reset", traci.edge.getAdaptedTraveltime("SC", 0))

traci.edge.adaptTraveltime("SC", 7, 10, 20)
print("interval", traci.edge.getAdaptedTraveltime("SC", 15))

traci.edge.adaptTraveltime("SC", tc.INVALID_DOUBLE_VALUE)
print("reset interval", traci.edge.getAdaptedTraveltime("SC", 15))

traci.close()
