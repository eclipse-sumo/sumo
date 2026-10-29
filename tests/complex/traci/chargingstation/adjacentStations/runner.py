#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
"""Check charging and battery accounting across adjacent limited stations."""

import os
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402


def run(directory, with_stations):
    suffix = "charging" if with_stations else "baseline"
    battery_output = directory / (suffix + ".battery.xml")
    station_output = directory / (suffix + ".stations.xml")
    command = [sumolib.checkBinary("sumo"), "-n", "input_net.net.xml",
               "-r", "input_routes.rou.xml", "--end", "5", "--step-length", "1",
               "--battery-output", str(battery_output), "--battery-output.precision", "6",
               "--no-step-log", "true", "--no-duration-log", "true"]
    if with_stations:
        command += ["-a", "input_additional.add.xml", "--chargingstations-output",
                    str(station_output), "--precision", "6"]
    subprocess.run(command, check=True, capture_output=True, text=True)
    battery = {}
    for timestep in ET.parse(battery_output).getroot():
        vehicle = timestep.find("vehicle")
        if vehicle is not None:
            battery[float(timestep.get("time"))] = vehicle
    return battery, station_output


with tempfile.TemporaryDirectory() as temporary_directory:
    directory = Path(temporary_directory)
    baseline, _ = run(directory, False)
    charging, station_output = run(directory, True)

    stations = {station.get("id"): station for station in ET.parse(station_output).getroot()}
    assert set(stations) == {"first", "second"}
    delivered = {}
    for station_id, station in stations.items():
        steps = station.findall("vehicle/step")
        assert steps, (station_id, "did not charge")
        for step in steps:
            time = float(step.get("time"))
            energy = float(step.get("energyCharged"))
            assert 0 < energy <= 1000.0 / 3600.0 + 1e-5, (station_id, time, energy)
            delivered[time] = delivered.get(time, 0.0) + energy
        assert abs(sum(float(step.get("energyCharged")) for step in steps)
                   - float(station.get("totalEnergyCharged"))) < 1e-5

    first_times = {float(step.get("time")) for step in stations["first"].findall("vehicle/step")}
    second_times = {float(step.get("time")) for step in stations["second"].findall("vehicle/step")}
    assert first_times == {1.0, 2.0}, first_times
    assert second_times == {3.0}, second_times

    cumulative_charge = 0.0
    for time in sorted(charging):
        cumulative_charge += delivered.get(time, 0.0)
        actual_gain = (float(charging[time].get("actualBatteryCapacity"))
                       - float(baseline[time].get("actualBatteryCapacity")))
        assert abs(actual_gain - cumulative_charge) < 1e-4, (time, actual_gain, cumulative_charge)

    print("adjacent charging stations: limits and battery accounting OK")
