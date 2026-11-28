#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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

import os
from pathlib import Path
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.append(str(Path(os.environ["SUMO_HOME"]) / "tools"))
import sumolib  # noqa: E402

sumo = sumolib.checkBinary("sumo")
network = Path(__file__).resolve().parents[2] / "sumo/insertionCapacity_2lane/input_net.net.xml"


def run(prefix, *options):
    subprocess.run([sumo, "-n", str(network), "-r", "input_routes.rou.xml",
                    "--no-step-log", "--duration-log.disable", "--thread-rngs", "4",
                    "--fcd-output", prefix + "-fcd.xml", "--tripinfo-output", prefix + "-tripinfos.xml",
                    *options], check=True)
    return [Path(prefix + "-" + output + ".xml").read_text(encoding="utf-8") for output in ("fcd", "tripinfos")]


random_outputs = run("random", "--seed", "17", "--random")
seeds = [re.search(r'<seed value="(-?\d+)"/>', output) for output in random_outputs]
assert all(seeds) and seeds[0][1] == seeds[1][1], "Missing or inconsistent run seed in the output headers"
seed = seeds[0][1]
assert seed != "17", "The header contains the supplied seed rather than the effective seed"
replayed_outputs = run("replay", "--seed", seed)
for original, replayed in zip(random_outputs, replayed_outputs):
    assert ET.tostring(ET.fromstring(original)) == ET.tostring(ET.fromstring(replayed))
assert len(ET.fromstring(random_outputs[1])) > 0
print("The recorded seed reproduced floating car data and trip information.")
