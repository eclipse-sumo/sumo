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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-10-19

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa


if sys.argv[1] == "sumo":
    sumoCall = [os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))]
else:
    sumoCall = [os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui')), '-S', '-Q']


def runSingle(traciEndTime, latDist, vehID):
    step = 0
    traci.start(sumoCall + ["-c", "sumo.sumocfg",
                            "--lateral-resolution", "0.1"])

    # disable safety checks and set constant speed
    print("old lanechangemode", format(traci.vehicle.getLaneChangeMode(vehID), '012b'))
    print("old speedmode", traci.vehicle.getSpeedMode(vehID))
    for veh in ("collider", "left", "right"):
        traci.vehicle.setLaneChangeMode(veh, 0b010001010101)
        traci.vehicle.setSpeedMode(veh, 0)
        traci.vehicle.setSpeed(veh, 20)
    print("new lanechangemode", format(traci.vehicle.getLaneChangeMode(vehID), '012b'))
    print("new speedmode", traci.vehicle.getSpeedMode(vehID))

    while not step > traciEndTime:
        traci.simulationStep()

        # check if vehID has arrived at destination
        arrivedList = traci.simulation.getArrivedIDList()
        if vehID in arrivedList:
            print("[%03d] Vehicle '%s' has arrived at destination" % (step, vehID))
            break

        print("trying to change lateral position by %.2f..." % (latDist))
        traci.vehicle.changeSublane(vehID, latDist)
        print("[%03d] lane %d, lateral pos: %.2f" %
              (step, traci.vehicle.getLaneIndex(vehID), traci.vehicle.getLateralLanePosition(vehID)))
        sys.stdout.flush()

        step += 1

    print("Print ended at step %s" % traci.simulation.getTime())
    traci.close()
    sys.stdout.flush()


sys.stdout.flush()
sys.stderr.flush()
runSingle(50, float(sys.argv[2]), "collider")
