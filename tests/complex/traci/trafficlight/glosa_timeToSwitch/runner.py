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
# @author  Mark Colley
# @date    2026-04-12

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             '-n', 'input_net2.net.xml',
             '-r', 'input_routes.rou.xml',
             '--no-step-log'] + [a for a in sys.argv[1:] if not a.startswith("--meso")])

# In input_net2.net.xml, SC->CN is red during the first 45s phase (33+3+6+3 = 45s)
traci.simulationStep()
print("default timeToSwitch:", traci.vehicle.getParameter("v0", "device.glosa.timeToSwitch"))
print("initial speedFactor:", round(traci.vehicle.getSpeedFactor("v0"), 3))

# Override predicted switch time to 12s (causing GLOSA to slow v0 down to arrive at t=13s instead of t=46s)
traci.vehicle.setParameter("v0", "device.glosa.timeToSwitch", "12.0")
print("override timeToSwitch:", traci.vehicle.getParameter("v0", "device.glosa.timeToSwitch"))

traci.simulationStep()
print("next step override timeToSwitch:", traci.vehicle.getParameter("v0", "device.glosa.timeToSwitch"))
print("adapted speedFactor:", round(traci.vehicle.getSpeedFactor("v0"), 3))

# Reset override back to static signal plan
traci.vehicle.setParameter("v0", "device.glosa.timeToSwitch", "-1")
print("reset timeToSwitch:", traci.vehicle.getParameter("v0", "device.glosa.timeToSwitch"))

traci.close()
