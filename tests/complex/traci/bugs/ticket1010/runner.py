#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @author  Laura Bieker
# @date    2013-10-07

from __future__ import absolute_import

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


cmd = [sumolib.checkBinary(sys.argv[1]),
       "-n", "input_net.net.xml",
       "-r", "input_routes.rou.xml",
       "-a", "input_additional.add.xml",
       "--no-step-log", "-S", "-Q"]


def run():
    """execute the TraCI control loop"""
    traci.start(cmd)
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0 and step < 100:
        traci.simulationStep()
        step += 1
        if step == 4:
            traci.trafficlight.setProgram("center", "0")
    traci.close()
    sys.stdout.flush()


run()
