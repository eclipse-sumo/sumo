#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-11-05

"""Invalid replacement indices must leave the person plan and server usable."""

import os
import subprocess
import sys

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci


def plan():
    return [repr(traci.person.getStage("person", index))
            for index in range(traci.person.getRemainingStages("person"))]


def check_invalid_indices():
    before = plan()
    position = traci.person.getPosition("person")
    for index in (-1, -2, -2147483648, len(before), 2147483647):
        try:
            traci.person.replaceStage("person", index, traci.person.getStage("person", 0))
        except traci.TraCIException:
            pass
        else:
            raise AssertionError(f"Invalid stage index accepted: {index}")
        assert plan() == before
        assert traci.person.getPosition("person") == position
    traci.simulationStep()


traci.start([sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "--no-step-log"])
con = traci.getConnection()
try:
    traci.person.add("person", "edge", 20)
    traci.person.appendWaitingStage("person", 100)
    traci.simulationStep()
    check_invalid_indices()
    print("Invalid indices preserve a single stage plan")

    traci.person.appendWaitingStage("person", 50, "future")
    check_invalid_indices()
    print("Invalid indices preserve a multiple stage plan")

    replacement = traci.person.getStage("person", 1)
    replacement.description = "future replacement"
    replacement.travelTime = 40
    traci.person.replaceStage("person", 1, replacement)
    assert traci.person.getStage("person", 1).description == "waiting (future replacement)"
    assert traci.person.getStage("person", 1).travelTime == 40
    replacement = traci.person.getStage("person", 0)
    replacement.description = "current replacement"
    replacement.travelTime = 30
    traci.person.replaceStage("person", 0, replacement)
    assert traci.person.getStage("person", 0).description == "waiting (current replacement)"
    assert traci.person.getRemainingStages("person") == 2
    traci.simulationStep()
    print("Valid future and current replacements succeed")
finally:
    try:
        con.close(wait=False)
    except (traci.FatalTraCIError, OSError):
        if con._socket is not None:
            con._socket.close()
    try:
        con._process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        con._process.kill()
        con._process.wait(timeout=5)
