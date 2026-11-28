#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
# @author  Josh
# @date    2026-11-26

import math
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402


def getPositions():
    traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--seed", "5", "--no-step-log"] + sys.argv[1:])
    try:
        edge = "3si"
        length = traci.lane.getLength(edge + "_0")
        randomIDs = [f"random{index}" for index in range(4)]
        departures = [(personID, "random") for personID in randomIDs] + [("negative", -2), ("number", "12.5")]
        for personID, position in departures:
            traci.person.add(personID, edge, position)
            traci.person.appendWaitingStage(personID, 10)
        traci.simulationStep()
        assert math.isclose(traci.person.getLanePosition("negative"), length - 2)
        assert math.isclose(traci.person.getLanePosition("number"), 12.5)
        positions = [traci.person.getLanePosition(personID) for personID in randomIDs]
        assert all(0 <= pos < length for pos in positions)
        assert len(set(positions)) == len(positions)
        return positions
    finally:
        traci.close()


assert getPositions() == getPositions()
print("Random person departure positions follow the simulation seed")
