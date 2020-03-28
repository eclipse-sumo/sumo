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

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa


def main(args):
    traci.start([sumolib.checkBinary('sumo'), "-c", "data/hello.sumocfg",
                 "--netstate-dump", "rawdump.xml",
                 "--no-step-log"] + sys.argv[1:])

    for step in range(162):
        traci.simulationStep()
        if step == 120:
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRoute('Stapler_00', ('ed1', 'ed5'))
            print(traci.vehicle.getRoute('Stapler_00'))
            # assert(traci.vehicle.getRoute('Stapler_00') == ('ed0', 'ed1', 'ed5'))
            print(traci.vehicle.getDistance('Stapler_00'))
        if step == 122:
            # assert(traci.vehicle.getRoute('Stapler_00') == ('ed0', 'ed1', 'ed5'))
            print(traci.vehicle.getDistance('Stapler_00'))
            traci.vehicle.setRouteID('Stapler_00', "short")
            print(traci.vehicle.getRoute('Stapler_00'))
            print(traci.vehicle.getDistance('Stapler_00'))
            # We assume, that we reach an internal lane at step 130,
            # if distance calc is correct, there should be a
            # 10m distance difference between the
            # output of step 129 and 130
        if step in (129, 130):
            print(step, round(traci.vehicle.getDistance('Stapler_00'), 2))
    traci.close()


if __name__ == "__main__":
    main(sys.argv[1:])
