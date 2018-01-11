#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Laura Bieker
# @author  Michael Behrisch
# @date    2014-08-28
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess


if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path += [tools, os.path.join(tools, 'assign')]
    import sumolib  # noqa
    import traci
    import traci.constants as tc

else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


# the port used for communicating with your sumo instance
PORT_TRACI = 8873


def init_traci():

    sumoBinary = sumolib.checkBinary('sumo')
#    sumoBinary = sumolib.checkBinary('sumo-gui')

    sumo_call = [sumoBinary, "-c", "data/hello.sumocfg",
                 "--remote-port", str(PORT_TRACI),
                 "--netstate-dump", "rawdump.xml",
                 "--no-step-log",
                 "-v",
                 ]

    sumoProcess = subprocess.Popen(
        sumo_call, stdout=sys.stdout, stderr=sys.stderr)

    traci.init(PORT_TRACI)
    return sumoProcess


def main(args):
    sumoProcess = init_traci()
    step = -1
    while True:
        step += 1

        traci.simulationStep()

        if step == 140:
            traci.vehicle.resume('Stapler_00')

        if step == 160:
            traci.close()
            sumoProcess.wait()
            break

if __name__ == "__main__":
    main(sys.argv[1:])
