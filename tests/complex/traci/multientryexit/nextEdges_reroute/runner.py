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

from contextlib import redirect_stdout
from pathlib import Path
import os
import sys
import xml.etree.ElementTree as ET

sumo_home = Path(os.environ["SUMO_HOME"])
sys.path.append(str(sumo_home / "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402

network = sumo_home / "tests/sumo/output/net4.net.xml"
routes = Path("routes.rou.xml")


def run_case(initial_edge, later_edge, expected_count):
    routes.write_text(f'<routes><vehicle id="v" depart="0" departSpeed="5">'
                      f'<route edges="SC {initial_edge}"/></vehicle></routes>\n', encoding="utf-8")
    changed = False
    with open(os.devnull, "w") as quiet, redirect_stdout(quiet):
        traci.start([sumolib.checkBinary("sumo"), "-n", str(network), "-r", str(routes),
                     "-a", "input_additional.add.xml", "--no-step-log", "--no-duration-log"])
        try:
            for _ in range(90):
                traci.simulationStep()
                if not changed and "v" in traci.vehicle.getIDList() and traci.vehicle.getLanePosition("v") > 14:
                    assert bool(traci.multientryexit.getLastStepVehicleIDs("filtered")) == bool(expected_count)
                    traci.vehicle.setRoute("v", ["SC", later_edge])
                    changed = True
                if traci.simulation.getMinExpectedNumber() == 0:
                    break
        finally:
            traci.close()
    assert changed, (initial_edge, later_edge)
    for detector, expected in (("filtered", expected_count), ("all", 1)):
        entries = ET.parse(f"{detector}.xml").getroot()
        veh_sum = sum(int(interval.get("vehicleSum")) for interval in entries)
        assert veh_sum == expected, (initial_edge, later_edge, detector)


run_case("CN", "CN", 1)
run_case("CN", "CE", 1)
run_case("CE", "CN", 0)
print("Entry-exit filtering remains consistent after rerouting.")
