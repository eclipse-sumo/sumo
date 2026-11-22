#!/usr/bin/env python
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
# @date    2026-11-21

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa

traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg"])
for name, var in [
    ("POSITION_CONVERSION", tc.POSITION_CONVERSION),
    ("DISTANCE_REQUEST", tc.DISTANCE_REQUEST),
    ("FIND_ROUTE", tc.FIND_ROUTE),
    ("FIND_INTERMODAL_ROUTE", tc.FIND_INTERMODAL_ROUTE),
    ("VAR_BUS_STOP_WAITING", tc.VAR_BUS_STOP_WAITING),
    ("VAR_BUS_STOP_WAITING_IDS", tc.VAR_BUS_STOP_WAITING_IDS),
    ("VAR_OPTION", tc.VAR_OPTION),
]:
    try:
        traci.simulation.subscribe([var])
    except traci.exceptions.TraCIException as e:
        print(name, "->", e)
traci.close()
