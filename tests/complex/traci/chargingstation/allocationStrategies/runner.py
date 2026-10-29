#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
"""Check the three charging-station allocations against known vehicle requests."""

import os
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402


EXPECTED_KW = {
    "max-min": {"t1": 11.0, "t2": 11.0, "t3": 8.0},
    "proportional": {"t1": 15.0, "t2": 9.0, "t3": 6.0},
    "flat": {"t1": 50.0 / 3.0, "t2": 26.0 / 3.0, "t3": 14.0 / 3.0},
}


with tempfile.TemporaryDirectory() as directory:
    for strategy, expected in EXPECTED_KW.items():
        output = Path(directory) / (strategy + ".xml")
        cmd = [sumolib.checkBinary("sumo"), "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
               "-a", {"max-min": "input_additional.add.xml",
                       "proportional": "input_additional2.add.xml",
                       "flat": "input_additional3.add.xml"}[strategy], "--end", "40", "--step-length", "1",
               "--device.battery.probability", "1", "--battery-output", str(output),
               "--battery-output.precision", "6", "--no-step-log", "true", "--no-duration-log", "true"]
        subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        samples = []
        for step in ET.parse(output).getroot():
            readings = {vehicle.get("id"): float(vehicle.get("energyCharged", "0")) * 3.6
                        for vehicle in step if vehicle.get("id") in expected}
            if all(readings.get(vehicle, 0) > 0 for vehicle in expected):
                samples.append((float(step.get("time")), readings))
        assert len(samples) >= 3, (strategy, samples)
        for time, readings in samples:
            assert sum(readings.values()) <= 30.01, (strategy, time, readings)
            for vehicle, target in expected.items():
                assert abs(readings[vehicle] - target) < 0.01, (strategy, time, vehicle, readings[vehicle])
        print(strategy, "allocation OK")
