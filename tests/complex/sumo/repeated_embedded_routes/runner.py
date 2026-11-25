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

from pathlib import Path
import os
import subprocess
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


def read_stops(output):
    result = {}
    for stop in ET.parse(output).findall("stopinfo"):
        edge = stop.get("edge") or stop.get("lane").rsplit("_", 1)[0]
        result.setdefault(stop.get("id"), []).append((edge, stop.get("started"), stop.get("ended")))
    return result


for mode, options in (("micro", []), ("meso", ["--mesosim"])):
    args = [sumolib.checkBinary("sumo"), "-n", sys.argv[1], "-r", "input_routes.rou.xml",
            "--no-step-log", "--no-duration-log", "--default.speeddev", "0"] + options
    subprocess.check_call(args + ["--stop-output", "actual-stops.xml", "--vehroute-output", "schedule.xml",
                                 "--save-state.times", "15", "--save-state.files", "checkpoint.xml",
                                 "--save-state.rng"])
    stopped = read_stops("actual-stops.xml")
    expected_edges = {"flow.0": ["CS"] * 3, "flow.1": ["CS"] * 3,
                      "one": ["CN", "CS"] * 3 + ["SC"]}
    assert {vid: [stop[0] for stop in stops] for vid, stops in stopped.items()} == expected_edges
    plans = {v.get("id"): v for v in ET.parse("schedule.xml").findall("vehicle")}
    for vid, until in (("flow.0", [30, 150, 270]), ("flow.1", [90, 210, 330]),
                       ("one", [420, 440, 520, 540, 620, 640])):
        scheduled = [float(stop.get("until")) for stop in plans[vid].findall("stop") if "until" in stop.attrib]
        assert scheduled == until, (mode, vid, scheduled)
    subprocess.check_call(args + ["--stop-output", "restored-stops.xml", "--load-state", "checkpoint.xml"])
    assert read_stops("restored-stops.xml") == stopped
    print(mode, "repeats only the selected route's stops before and after reloading state.")
