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
    sys.path += [tools]
    import sumolib  # noqa
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
