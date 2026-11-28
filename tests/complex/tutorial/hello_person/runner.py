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
from tempfile import TemporaryDirectory
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa: E402


config = Path(__file__).resolve().parent / "data" / "hello_person.sumocfg"
with TemporaryDirectory() as temporary:
    trips = Path(temporary) / "trips.xml"
    subprocess.run([checkBinary("sumo"), "-c", str(config), "--tripinfo-output", str(trips)], check=True)
    trip_info = ET.parse(trips).getroot()
    person = trip_info.find("personinfo[@id='maya']")
    if person is None:
        raise RuntimeError("Maya did not complete the journey")
    stages = [stage.tag for stage in person]
    if stages != ["walk", "access", "ride", "access", "walk"]:
        raise RuntimeError("Unexpected person plan stages %s" % stages)
    if person.find("ride").get("vehicle") != "regional":
        raise RuntimeError("Maya did not ride the regional train")
    if float(person.findall("walk")[-1].get("arrival")) < 0:
        raise RuntimeError("Maya did not complete the final walk")
print("Maya walked to West Station, rode the train and walked to her destination.")
