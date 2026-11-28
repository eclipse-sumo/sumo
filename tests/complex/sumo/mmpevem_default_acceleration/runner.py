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
from pathlib import Path
import subprocess
import sys
import xml.etree.ElementTree as ET

sumo_home = Path(os.environ["SUMO_HOME"])
sys.path.append(str(sumo_home / "tools"))
import sumolib  # noqa: E402

network = Path(__file__).resolve().parents[3] / "sumo/cf_model/IDM/ticket11564/net.net.xml"
routes = Path("trip.rou.xml")
cases = (
    ("default", "", []),
    ("heavy", ' mass="2400" sigma="0" speedFactor="1"', []),
    ("action_step", ' actionStepLength="2" tau="2" sigma="0" speedFactor="1"', ["--step-method.ballistic"]),
)
for label, parameters, extra in cases:
    routes.write_text(f'<routes><vType id="car" emissionClass="MMPEVEM/default"{parameters}/>'
                      '<vehicle id="vehicle" type="car" depart="0" departSpeed="0">'
                      '<route edges="E1"/></vehicle></routes>\n', encoding="utf-8")
    subprocess.run([sumolib.checkBinary("sumo"), "-n", str(network), "-r", str(routes),
                    "--emission-output", "emissions.xml", "--device.battery.probability", "1",
                    "--battery-output", "battery.xml", "--no-step-log", "--no-duration-log", *extra],
                   check=True, stdout=subprocess.DEVNULL, timeout=30)
    for filename, name in (("emissions.xml", "electricity"), ("battery.xml", "energyConsumed")):
        records = [vehicle for timestep in ET.parse(filename).getroot() for vehicle in timestep]
        assert max(float(vehicle.get("speed")) for vehicle in records) > 28, (label, filename)
        assert all(math.isfinite(float(vehicle.get(name))) for vehicle in records), (label, filename)

print("MMPEVEM driving produced finite emissions and battery output.")
