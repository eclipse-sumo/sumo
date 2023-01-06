#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

ANGLE_UNDEF = traci.constants.INVALID_DOUBLE_VALUE
INVALID = traci.constants.INVALID_DOUBLE_VALUE

victim = "victim"
collider = "collider"

traci.start([sumolib.checkBinary("sumo"),
             '-n', 'input_net2.net.xml',
             '--collision.action', 'warn',
             '--collision.mingap-factor', '0',
             '--step-length', '0.1',
             '--no-step-log'])

traci.simulationStep()
traci.route.add("r0", ["SC", "CN"])
traci.vehicle.add(victim, "r0", departPos="80")
traci.vehicle.add(collider, "r0", departPos="10")
traci.simulationStep()

x, y = traci.vehicle.getPosition(victim)

traci.vehicle.moveToXY(collider, "", -1, x, y - 3)
traci.simulationStep()

traci.close()
