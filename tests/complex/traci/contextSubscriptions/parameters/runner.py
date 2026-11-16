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
# @date    2026-11-05

import os
import pprint
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa

traci.start([sumolib.checkBinary("sumo"), "-c", "sumo.sumocfg"] + sys.argv[1:])
try:
    traci.simulationStep()
    traci.vehicle.setParameter("ego", "foo", "42")
    traci.vehicle.setParameter("ego", "bar", "baz")
    traci.vehicle.subscribeContext(
        "ego", tc.CMD_GET_VEHICLE_VARIABLE, 50.,
        [tc.VAR_PARAMETER_WITH_KEY, tc.VAR_ROAD_ID, tc.VAR_PARAMETER],
        parameters={tc.VAR_PARAMETER: "foo", tc.VAR_PARAMETER_WITH_KEY: "bar"},
    )
    pprint.pprint(traci.vehicle.getContextSubscriptionResults("ego"))
    traci.vehicle.setParameter("ego", "foo", "99")
    traci.simulationStep()
    pprint.pprint(traci.vehicle.getContextSubscriptionResults("ego"))
    traci.vehicle.unsubscribeContext("ego", tc.CMD_GET_VEHICLE_VARIABLE, 50.)
    traci.simulationStep()
    pprint.pprint(traci.vehicle.getAllContextSubscriptionResults())
finally:
    traci.close()
