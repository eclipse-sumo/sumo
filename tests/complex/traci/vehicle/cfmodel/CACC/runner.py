#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2023 German Aerospace Center (DLR) and others.
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
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26

# @author  Robert Alms
# @date	   2023-02.23

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

# we need to import python modules from the $SUMO_HOME/tools directory
try:
    sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
        os.path.dirname(__file__), "..", "..", "..", "..")), "tools"))
except ImportError:
    sys.exit("please declare environment variable 'SUMO_HOME'")
import traci
from sumolib import checkBinary


def run(mode, fname):
    traci.start([sumoBinary,
                 "-n", "input_net.net.xml",
                 "-r", "input_routes.rou.xml",
                 "--fcd-output", fname,
                 "--fcd-output.attributes", "speed",
                 "--default.speeddev", "0",
                 "--step-length", "0.5",
                 "--no-step-log"])

    traci.vehicle.setParameter("ego", "carFollowModel.caccCommunicationsOverrideMode", str(mode))
    print("Set OverrideMode: %s , get OverrideMode: %s" % (
        mode,
        traci.vehicle.getParameter("ego", "carFollowModel.caccCommunicationsOverrideMode")))

    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
    traci.close()


sumoBinary = checkBinary('sumo')
for mode, fname in zip(range(0, 4), ['fcd.xml', 'fcd2.xml', 'fcd3.xml', 'fcd4.xml']):
    run(mode, fname)
