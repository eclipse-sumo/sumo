#!/usr/bin/env python
"""
@file    runner.py
@author  Laura Bieker
@author  Michael Behrisch
@date    2014-08-28
@version $Id$

Python resume test.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess


if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path += [tools, os.path.join(tools, 'assign')]
    import sumolib
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
