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
import subprocess
import sys
from xml.etree import ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


def read_trips(sumo, network, threads):
    output = "tripinfo_%s.xml" % threads
    subprocess.run(
        [sumo, "-n", str(network), "-r", "input_routes.rou.xml",
         "--threads", str(threads), "--step-length", "0.2", "--precision", "9",
         "--no-step-log", "--no-duration-log", "--tripinfo-output", output],
        capture_output=True, check=True, timeout=30,
    )
    trips = [trip.attrib for trip in ET.parse(output).findall("tripinfo")]
    assert len(trips) == 126
    return trips


# Use the existing priority-to-right crossing from the junction tests.
network = Path(__file__).resolve().parents[3] / "sumo/junction_model/junction_type/right_before_left/net.net.xml"
sumo = sumolib.checkBinary("sumo")
serial = read_trips(sumo, network, 1)
for repeat in range(8):
    assert read_trips(sumo, network, 4) == serial, "four-thread output differs on run %s" % (repeat + 1)
print("Compared 126 trips with one and four threads.")
