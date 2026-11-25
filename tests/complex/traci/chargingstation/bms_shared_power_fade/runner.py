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

from argparse import ArgumentParser
from copy import deepcopy
from itertools import product
from math import isclose
import os
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa


def check_allocation(net, initial_health, initial_charge, fade, ramped, export=True, discharger=False):
    routes = ET.parse("input_routes.rou.xml")
    for key, value in (("stateOfHealth", initial_health), ("chargeLevel", initial_charge),
                       ("capacityLossPerCycle", fade)):
        ET.SubElement(routes.find("vType"), "param", key="device.battery." + key, value=str(value))
    if discharger:
        donor = deepcopy(routes.find("vType"))
        donor.set("id", "donor")
        ET.SubElement(donor, "param", key="device.battery.maximumDischargeRate", value="3600")
        routes.getroot().insert(1, donor)
        routes.find("vehicle[@id='B']").set("type", "donor")
    routes.write("probe.rou.xml")

    station = ET.Element("additional")
    charger = ET.SubElement(station, "chargingStation", id="station", lane="D2_0",
                            startPos="50", endPos="70", power="36000", totalPower="1800", efficiency="1")
    if ramped:
        charger.set("powerRampUp", "72000")
        charger.set("powerRampDown", "72000")
    if discharger:
        charger.set("dischargePower", "3600")
    ET.ElementTree(station).write("probe.add.xml")

    args = [sumolib.checkBinary("sumo"), "-n", net, "-r", "probe.rou.xml",
            "-a", "probe.add.xml", "--no-step-log", "--no-duration-log", "--end", "2",
            "--device.battery.probability", "1", "--precision", "8"]
    if export:
        args += ["--chargingstations-output", "recorded-charging.xml"]
    traci.start(args)
    delivered = 0.5 if discharger else 0.25
    expected_health = max(0.05, initial_health - fade * delivered / 200)
    expected_charge = min(initial_charge + delivered, 100 * expected_health)
    try:
        traci.simulationStep()
        traci.simulationStep()
        for vid in (("A",) if discharger else ("A", "B")):
            for key, expected in (("chargeLevel", expected_charge), ("stateOfHealth", expected_health),
                                  ("totalEnergyCharged", delivered)):
                actual = traci.vehicle.getParameter(vid, "device.battery." + key)
                assert isclose(float(actual), expected), (vid, key, actual, expected)
        if discharger:
            assert isclose(float(traci.vehicle.getParameter("B", "device.battery.energyDischarged")), 1)
            assert isclose(float(traci.vehicle.getParameter("B", "device.battery.chargeLevel")), initial_charge - 1)
    finally:
        traci.close()

    if not export:
        return
    steps = [step for step in ET.parse("recorded-charging.xml").findall(".//vehicle/step")
             if float(step.get("energyCharged")) > 0]
    assert len(steps) == (1 if discharger else 2)
    for step in steps:
        assert isclose(float(step.get("energyCharged")), delivered)
        assert isclose(float(step.get("actualBatteryCapacity")), expected_charge)


parser = ArgumentParser()
parser.add_argument("--net-file", required=True)
options, _ = parser.parse_known_args()
for ramped, (initial_health, initial_charge, fade) in product(
        (False, True), ((0.5, 49, 0), (0.5, 49, 1), (0.0525, 4.25, 1))):
    check_allocation(options.net_file, initial_health, initial_charge, fade, ramped)

check_allocation(options.net_file, 0.5, 49, 1, False, export=False)
for export in (False, True):
    check_allocation(options.net_file, 0.5, 49, 1, False, export=export, discharger=True)

print("Shared power preserves the delivered charge and final battery health.")
