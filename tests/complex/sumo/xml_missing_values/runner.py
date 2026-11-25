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

import os
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

FIXTURES = Path(__file__).resolve().parents[3] / "sumo"
BASE = ["--no-step-log", "--no-duration-log", "--xml-validation", "never",
        "--default.speeddev", "0", "--default.departspeed", "0", "--default.departlane", "first"]
ARRIVAL = {"arrival", "arrivalLane", "arrivalPos", "arrivalPosLat", "arrivalSpeed"}
SUMO = sumolib.checkBinary("sumo")


def simulate(folder, options, mode=None):
    command = [SUMO, *BASE, *map(str, options)]
    if mode is not None:
        command += ["--output.xml-missing-values", mode]
    subprocess.run(command, cwd=folder, stdout=subprocess.DEVNULL, check=True)


def compare(folder, options, filename, missing):
    simulate(folder, options, "legacy")
    path = folder / filename
    expected = path.read_bytes()
    simulate(folder, options)
    assert ET.tostring(ET.fromstring(path.read_bytes())) == ET.tostring(ET.fromstring(expected))
    for mode in ("empty", "omit"):
        simulate(folder, options, mode)
        tree = ET.fromstring(expected)
        count = 0
        for element in tree.iter():
            for key, value in list(element.attrib.items()):
                if missing(element, key, value):
                    count += 1
                    if mode == "empty":
                        element.attrib[key] = ""
                    else:
                        del element.attrib[key]
        assert count, filename
        assert ET.tostring(ET.fromstring(path.read_bytes())) == ET.tostring(tree), (filename, mode)
    print(filename, "passed")


with TemporaryDirectory() as temp:
    folder = Path(temp)
    (folder / "input.rou.xml").write_text(
        '<routes><vType id="mm" emissionClass="MMPEVEM/default">'
        '<param key="maximumTorque" value="0.1"/></vType>'
        '<route id="r" edges="beg middle end"/>'
        '<vehicle id="early" route="r" type="mm" depart="0"/>'
        '<vehicle id="late" route="r" depart="99"/></routes>\n')
    small = ["-n", FIXTURES / "three_split.net.xml", "-r", "input.rou.xml", "-e", 100]
    compare(folder, small + ["--tripinfo-output", "tripinfo.xml", "--tripinfo-output.write-unfinished"], "tripinfo.xml",
            lambda element, key, value: (key in ARRIVAL and element.get("id") == "late")
            or (key == "vaporized" and not value))
    compare(folder, small + ["--emission-output", "emissions.xml"], "emissions.xml",
            lambda element, key, value: value.lower().lstrip("-") == "nan")

    battery = FIXTURES / "devices/battery/actualBatteryCapacity"
    compare(folder, ["-n", FIXTURES / "devices/battery/net.net.xml", "-r", battery / "input_routes.rou.xml",
                     "-a", battery / "input_additional.add.xml", "--battery-output", "battery.xml",
                     "--device.battery.probability", 1], "battery.xml",
            lambda element, key, value: key == "chargingStationId" and value == "NULL")

    lc = FIXTURES / "lc_model/network_prohibits_changing"
    compare(folder, ["-n", lc / "noleft/net.net.xml", "-r", lc / "input_routes.rou.xml",
                     "--lanechange-output", "lanechanges.xml"], "lanechanges.xml",
            lambda element, key, value: value == "None")

    ssm = FIXTURES / "devices/ssm/multilane"
    routes = (ssm / "adjacent_merge_both_turn1/input_routes.rou.xml").read_text()
    (folder / "input.rou.xml").write_text(routes.replace('key="device.ssm.trajectories" value="false"',
                                                         'key="device.ssm.trajectories" value="true"'))
    compare(folder, ["-n", ssm.parent / "net3.net.xml", "-r", "input.rou.xml", "--step-length", 0.1,
                     "--step-method.ballistic"], "ssm.xml",
            lambda element, key, value: key != "values" and value == "NA")
