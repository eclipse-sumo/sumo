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
# @author  Laura Bieker
# @author  Michael Behrisch
# @date    2014-08-28


from __future__ import print_function
from __future__ import absolute_import
import os
import sys


if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path += [tools, os.path.join(tools, 'assign')]
    import sumolib  # noqa
    import traci  # noqa
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def main(args):
    sumo_call = [sumolib.checkBinary('sumo'), "-c", "data/hello.sumocfg",
                 "--netstate-dump", "rawdump.xml",
                 "--no-step-log",
                 "-v",
                 ] + sys.argv[1:]
    traci.start(sumo_call)
    step = -1
    while True:
        step += 1
        traci.simulationStep()
        if step == 140:
            traci.vehicle.resume('Stapler_00')
        if step == 161:
            traci.close()
            break


if __name__ == "__main__":
    main(sys.argv[1:])
