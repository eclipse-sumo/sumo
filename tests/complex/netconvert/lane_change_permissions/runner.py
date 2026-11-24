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
import os
import subprocess
import sys
import xml.etree.ElementTree as ET

home = Path(os.environ["SUMO_HOME"])
sys.path.append(str(home / "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402
from traci import constants as tc  # noqa: E402

network = home / "tests/complex/sumo/insertionCapacity_2lane/input_net.net.xml"
netconvert = sumolib.checkBinary("netconvert")
Path("restrictions.edg.xml").write_text(
    '<edges><edge id="E0"><lane index="0" changeLeft="ignoring" changeRight="emergency"/>'
    '<lane index="1" changeRight="ignoring"/></edge></edges>',
    encoding="utf-8",
)


def convert(*args):
    subprocess.run([netconvert, *args], check=True, capture_output=True)


convert("-s", str(network), "-e", "restrictions.edg.xml", "-o", "compiled.net.xml", "-p", "plain")
convert("-s", "compiled.net.xml", "-o", "reloaded.net.xml")

for filename, lane_key in (("plain.edg.xml", "index"), ("compiled.net.xml", "id"), ("reloaded.net.xml", "id")):
    lanes = ET.parse(filename).findall("./edge[@id='E0']/lane")
    lane0, lane1 = lanes
    assert lane0.get(lane_key) in ("0", "E0_0")
    assert lane0.get("changeLeft") == "ignoring", filename
    assert lane0.get("changeRight") == "emergency", filename
    assert lane1.get("changeRight") == "ignoring", filename
    assert lane1.get("changeLeft") is None, filename

traci.start([sumolib.checkBinary("sumo"), "-n", "reloaded.net.xml", "--xml-validation.net", "local", "--no-step-log"])
try:
    assert traci.lane.getChangePermissions("E0_0", tc.LANECHANGE_LEFT) == ()
    assert traci.lane.getChangePermissions("E0_0", tc.LANECHANGE_RIGHT) == ("emergency",)
    assert traci.lane.getChangePermissions("E0_1", tc.LANECHANGE_RIGHT) == ()
    assert "passenger" in traci.lane.getChangePermissions("E0_1", tc.LANECHANGE_LEFT)
finally:
    traci.close()
print("Lane change restrictions survived plain and network XML round-trips.")
