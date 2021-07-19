#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    stateReplay.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @date    2021-05-20

"""
Synchronizes saved state files from a (remote) simulation and replays them in a
local sumo-gui instance to observe the remote simulation

requirements: rsync
"""

import os
import sys
import socket
import time
from subprocess import call
import glob
sys.path.append(os.path.join(os.environ["SUMO_HOME"], 'tools'))
import sumolib
import traci

sumoBinary = sumolib.checkBinary("sumo-gui")

def main():
    parser = sumolib.options.ArgumentParser()
    parser.add_argument("--sumo-config", default="sumo.sumocfg", help="sumo config file")
    parser.add_argument("--state-prefix", dest="statePrefix", default="state", help="prefix for synchronized state files")
    parser.add_argument("--src", help="the remote directory to sync")
    parser.add_argument("--dst", default="states", help="the subdirectory for the synced files")
    parser.add_argument("--delay", default=1, type=float, help="the delay between simulation states")
    options = parser.parse_args()

    traci.start([sumoBinary, "-c", options.sumo_config, "-S"])
    while True:
        call(['rsync', '-a', options.src, options.dst])
        files = glob.glob(options.dst + options.statePrefix + "*")
        fileSteps = [(int(os.path.basename(f).split('.')[0].split('_')[1]), f) for f in files]
        fileSteps.sort()
        lastState = fileSteps[-2][1]
        print(os.path.basename(lastState))
        traci.simulation.loadState(lastState)
        # a phantom step makes the client respond to gui-close but adds invalid
        # info (as long as the traffic lights are not in sync)
        # traci.simulationStep()
        time.sleep(options.delay)


if __name__ == "__main__":
    main()
