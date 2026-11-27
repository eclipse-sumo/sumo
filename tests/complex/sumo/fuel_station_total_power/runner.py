#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2,
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-24

from pathlib import Path
import os
import subprocess
import sys
import xml.etree.ElementTree as ET

tests = Path(__file__).resolve().parents[3]
sys.path.append(str(Path(os.environ.get("SUMO_HOME", tests.parent)) / "tools"))
import sumolib  # noqa

fixtures = tests / "sumo/devices/battery"
fuel = fixtures / "trackFuel"
additional = ET.parse(fuel / "input_additional.add.xml")
for station in additional.findall("chargingStation"):
    station.set("totalPower", "1000")
additional.write("limited.add.xml", encoding="utf-8")

common = [sumolib.checkBinary("sumo"), "-n", str(fixtures / "net.net.xml"), "-r",
          str(fuel / "input_routes.rou.xml"), "--device.battery.probability", "1", "--device.battery.track-fuel",
          "--no-step-log", "--no-duration-log", "--battery-output.precision", "4"]


def measure(label, stations):
    battery, station_output = label + ".battery.xml", label + ".stations.xml"
    subprocess.check_call(common + ["-a", str(stations), "--battery-output", battery,
                                    "--chargingstations-output", station_output], stdout=subprocess.DEVNULL)
    return ET.tostring(ET.parse(battery).getroot()), ET.tostring(ET.parse(station_output).getroot())


control = measure("unlimited", fuel / "input_additional.add.xml")
assert b'energyCharged="9500.0000"' in control[0], "the fixture must refuel the vehicle"
assert measure("limited", "limited.add.xml") == control
print("Fuel output is identical with or without an aggregate power limit.")
