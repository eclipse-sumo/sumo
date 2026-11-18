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
             '--device.ssm.probability', '1',
             '--device.ssm.measures', 'TTC DRAC BR SGAP TGAP',
             '--device.ssm.thresholds', '3.0 3.0 0.0 50.0 5.0',
             '--device.ssm.file', 'NUL',
             '--no-step-log'] + [a for a in sys.argv[1:] if not a.startswith("--meso")])

for step in range(10):
    traci.simulationStep()
    if step in (3, 8):
        print("step=%s maxBR=%s minSGAP=%s minTGAP=%s encounters='%s' collisions='%s'" % (
            step,
            traci.vehicle.getParameter("ego", "device.ssm.maxBR"),
            traci.vehicle.getParameter("ego", "device.ssm.minSGAP"),
            traci.vehicle.getParameter("ego", "device.ssm.minTGAP"),
            traci.vehicle.getParameter("ego", "device.ssm.activeEncounters"),
            traci.vehicle.getParameter("ego", "device.ssm.collisions"),
        ))

traci.close()
