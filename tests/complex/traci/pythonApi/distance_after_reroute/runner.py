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

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
if len(sys.argv) > 1:
    import libsumo as traci  # noqa
else:
    import traci  # noqa
import sumolib  # noqa


def main(args):
    traci.start([sumolib.checkBinary('sumo'), "-c", "data/hello.sumocfg",
                 "--netstate-dump", "rawdump.xml",
                 "--no-step-log"])

    for step in range(161):
        traci.simulationStep()
        if step == 120:
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRoute('Stapler_00', ('ed1', 'ed5'))
            print(traci.vehicle.getRoute('Stapler_00'))
            assert(traci.vehicle.getRoute('Stapler_00') == ('ed0', 'ed1', 'ed5'))
            print(traci.vehicle.getDistance('Stapler_00'))
        if step == 122:
            assert(traci.vehicle.getRoute('Stapler_00') == ('ed0', 'ed1', 'ed5'))
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRouteID('Stapler_00', "short")
            print(traci.vehicle.getRoute('Stapler_00'))
            print(traci.vehicle.getDistance('Stapler_00'))
    traci.close()


if __name__ == "__main__":
    main(sys.argv[1:])
