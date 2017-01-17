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
    sys.path += [tools]
    import sumolib
    import traci
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


# the port used for communicating with your sumo instance
PORT_TRACI = sumolib.miscutils.getFreeSocketPort()


def main(args):
    sumoBinary = sumolib.checkBinary('sumo')
    sumo_call = [sumoBinary, "-c", "data/hello.sumocfg",
                 "--remote-port", str(PORT_TRACI),
                 "--netstate-dump", "rawdump.xml",
                 "--no-step-log"]
    sumoProcess = subprocess.Popen(
        sumo_call, stdout=sys.stdout, stderr=sys.stderr)
    traci.init(PORT_TRACI)

    for step in range(161):
        traci.simulationStep()
        if step == 120:
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRoute('Stapler_00', ('ed1', 'ed5'))
            print(traci.vehicle.getRoute('Stapler_00'))
            assert(traci.vehicle.getRoute('Stapler_00')
                   == ['ed0', 'ed1', 'ed5'])
            print(traci.vehicle.getDistance('Stapler_00'))
        if step == 122:
            assert(traci.vehicle.getRoute('Stapler_00')
                   == ['ed0', 'ed1', 'ed5'])
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRouteID('Stapler_00', "short")
            print(traci.vehicle.getRoute('Stapler_00'))
            print(traci.vehicle.getDistance('Stapler_00'))
    traci.close()
    sumoProcess.wait()

if __name__ == "__main__":
    main(sys.argv[1:])
