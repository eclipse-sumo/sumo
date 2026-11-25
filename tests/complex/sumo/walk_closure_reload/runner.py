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

from pathlib import Path
import subprocess
from xml.etree import ElementTree as ET

import sumolib


def read_walk(path):
    positions = {}
    for step in ET.parse(path).findall("timestep"):
        person = step.find("person[@id='p']")
        if person is not None:
            positions[float(step.attrib["time"])] = (person.attrib["edge"], float(person.attrib["pos"]),
                                                     float(person.attrib["speed"]))
    return positions


fixture = (Path(__file__).resolve().parents[3] / "sumo" / "extended" / "rerouter" / "person"
           / "closing_sidewalk_late")
args = [sumolib.checkBinary("sumo"), "-n", str(fixture / "net.net.xml"),
        "-r", str(fixture / "input_routes.rou.xml"), "-a", str(fixture / "input_additional.add.xml"),
        "--default.speeddev", "0", "--no-step-log", "--no-duration-log", "--end", "40"]
for model in (("--pedestrian.model", "nonInteracting"), ("--mesosim",)):
    command = args + list(model)
    subprocess.check_call(command + ["--fcd-output", "uninterrupted.xml", "--save-state.times", "5",
                                     "--save-state.files", "approach.xml", "--save-state.transportables"])
    subprocess.check_call(command + ["--fcd-output", "resumed.xml", "--load-state", "approach.xml"])
    uninterrupted = read_walk("uninterrupted.xml")
    resumed = read_walk("resumed.xml")
    assert uninterrupted[20] == ("entry", 20, 0)
    assert uninterrupted[30][0] == "trigger"
    assert resumed == {step: position for step, position in uninterrupted.items() if step >= 5}
print("Reload preserves the approach and wait at the closed sidewalk.")
