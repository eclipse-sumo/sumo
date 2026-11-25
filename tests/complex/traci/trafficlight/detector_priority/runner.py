#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Mark Colley
# @date    2026-11-25

import os
from pathlib import Path
import sys
from xml.etree import ElementTree

sumo_home = Path(os.environ["SUMO_HOME"])
sys.path.append(str(sumo_home / "tools"))
import sumolib  # noqa
import traci  # noqa

network = sumo_home / "tests/sumo/tls/actuated/multiNext/dualring_simple/net.net.xml"
full_key = "priority:C_1_D1.0"
short_key = "priority:D1.0"


def next_phase(params, runtime=False):
    additional = ElementTree.parse("input_additional.add.xml")
    program = additional.find("tlLogic")
    for key, value in params:
        ElementTree.SubElement(program, "param", {"key": key, "value": value})
    additional.write("configured.add.xml", encoding="utf-8")

    traci.start([sumolib.checkBinary("sumo"), "-n", str(network), "-a", "configured.add.xml",
                 "--no-step-log", "--no-warnings", "--end", "30"])
    try:
        traci.inductionloop.overrideTimeSinceDetection("C_1_D0.0", 0)
        traci.inductionloop.overrideTimeSinceDetection("C_1_D1.0", 0)
        if runtime:
            traci.trafficlight.setParameter("C", short_key, "1.5")
            for key, value in ((short_key, "-1"), (short_key, "bad"), ("priority:unknown", "2")):
                try:
                    traci.trafficlight.setParameter("C", key, value)
                except traci.TraCIException:
                    continue
                raise AssertionError("Invalid priority accepted: " + key)
        for _ in range(6):
            traci.simulationStep()
        return traci.trafficlight.getPhase("C")
    finally:
        traci.close()


for case, settings, runtime, expected in (
    ("default", (), False, 1),
    ("full ID", ((full_key, "1.5"),), False, 3),
    ("short ID", ((short_key, "1.5"),), False, 3),
    ("full precedence", ((full_key, "1.5"), (short_key, "0")), False, 3),
    ("lower weight", ((full_key, "0.5"),), False, 1),
    ("runtime", (), True, 3),
):
    selected = next_phase(settings, runtime)
    print(case, selected)
    assert selected == expected, (case, selected, expected)
