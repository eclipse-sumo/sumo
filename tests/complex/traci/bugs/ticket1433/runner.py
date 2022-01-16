#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2022 German Aerospace Center (DLR) and others.
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
# @author  Laura Bieker-Walz
# @date    2020-09-04

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

from sumolib import checkBinary  # noqa
import traci  # noqa


def run():
    """execute the TraCI control loop"""
    step = 0
    while traci.simulation.getMinExpectedNumber() > 0:
        traci.simulationStep()
        step += 1
        ids = traci.simulation.getDepartedIDList()
        # print(ids)
        if "1" in ids:
            traci.vehicle.setRoute("1", ["4/1to3/1", "3/1to2/1", "2/1to1/1"])
    traci.close()
    sys.stdout.flush()


# this is the main entry point of this script
if __name__ == "__main__":

    sumoBinary = checkBinary('sumo')
    # prepare state
    subprocess.call([sumoBinary, "-c", "sumo.sumocfg"] + sys.argv[1:])

    args = [sumoBinary, "-n", "input_net.net.xml",
            "-a", "input_routes.rou.xml",
            "--load-state", "input_state.xml",
            "--no-step-log",
            "--begin", "9"]

    args += sys.argv[1:]

    traci.start(args)
    run()
