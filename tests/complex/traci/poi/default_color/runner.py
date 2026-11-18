#!/usr/bin/env python
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
# @author  Mark Colley
# @date    2026-11-18

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"] + sys.argv[1:])

traci.poi.add("p_default", 10.0, 20.0)
traci.poi.add("p_typed", 30.0, 40.0, poiType="shop")

print("p_default color:", traci.poi.getColor("p_default"))
print("p_default pos:", traci.poi.getPosition("p_default"))
print("p_typed color:", traci.poi.getColor("p_typed"))
print("p_typed type:", traci.poi.getType("p_typed"))

traci.close()
