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
# @author  Mark Colley
# @date    2026-11-24

"""Check that replay-created vehicles leave after their final FCD sample."""

from pathlib import Path
import os
import subprocess
import sys
import xml.etree.ElementTree as ET

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa
import traci  # noqa


NET = Path(__file__).resolve().parents[4] / "sumo/junction_model/junction_type/zipper/net.net.xml"
COMMON = [checkBinary("sumo"), "-n", str(NET), "--step-length", "0.1",
          "--no-step-log", "--no-duration-log", "--no-warnings", "--time-to-teleport", "-1"]


def await_lane(prefix):
    for _ in range(100):
        traci.simulationStep()
        if traci.vehicle.getLaneID("ego").startswith(prefix):
            return
    raise AssertionError("recording never reached lane " + prefix)


def record_samples():
    traci.start(COMMON + ["--fcd-output", "recorded.xml"])
    try:
        traci.route.add("trip", ["middle", "end"])
        traci.vehicle.add("ego", "trip", departPos="30", departSpeed="10")
        traci.simulationStep()
        traci.vehicle.setSpeedMode("ego", 0)
        traci.vehicle.setSpeed("ego", 10)
        for lane in (":gneJ1_", "end_"):
            await_lane(lane)
            traci.vehicle.setSpeed("ego", 0)
            traci.simulationStep()
            traci.vehicle.setSpeed("ego", 10)
        x, y = traci.lane.getShape("end_0")[-1]
        traci.vehicle.moveToXY("ego", "end", 0, x - 0.01, y, 90, 7)
        traci.vehicle.setSpeed("ego", 0)
        traci.simulationStep()
    finally:
        traci.close()
    return [step for step in ET.parse("recorded.xml").getroot() if step.find("vehicle") is not None]


def sample_at_stop(samples, lane_prefix):
    return next(i for i, step in enumerate(samples)
                if step.find("vehicle").get("lane").startswith(lane_prefix)
                and float(step.find("vehicle").get("speed")) == 0)


def write_recording(filename, samples):
    root = ET.Element("fcd-export")
    root.extend(samples)
    ET.ElementTree(root).write(filename)


samples = record_samples()
internal = sample_at_stop(samples, ":gneJ1_")

# A recording can start and end on a junction lane.
internal_vehicle = samples[internal].find("vehicle")
single = ET.Element("timestep", {"time": "0.00"})
single.append(internal_vehicle)
write_recording("single.xml", [single])
subprocess.run(COMMON + ["--device.fcd-replay.files", "single.xml", "--fcd-output", "single-replay.xml"],
               check=True, timeout=10)
replayed, = ET.parse("single-replay.xml").getroot().iter("vehicle")
assert replayed.get("lane") == internal_vehicle.get("lane")
assert replayed.get("pos") == internal_vehicle.get("pos")

write_recording("internal.xml", samples[:internal + 1])
traci.start(COMMON + ["--device.fcd-replay.files", "internal.xml"])
try:
    traci.simulationStep()
    # Retain the outgoing edge so the internal lane belongs to the replay route.
    traci.vehicle.setRoute("ego", ["middle", "end"])
    last_time = float(samples[internal].get("time"))
    traci.simulationStep(round(last_time + 0.1, 2))
    assert traci.vehicle.getLaneID("ego") == samples[internal].find("vehicle").get("lane")
    assert traci.vehicle.getSpeed("ego") == 0
    traci.simulationStep(round(last_time + 0.2, 2))
    assert traci.simulation.getArrivedIDList() == ("ego",)
    assert traci.simulation.getMinExpectedNumber() == 0
    print("Vehicle left after its final junction sample")
finally:
    traci.close()

normal = sample_at_stop(samples, "end_")
streamed = samples[:normal + 1]
for tick in range(round(float(streamed[-1].get("time")) * 10) + 1, 200):
    streamed.append(ET.Element("timestep", {"time": "%.2f" % (tick / 10)}))
samples[-1].set("time", "20.00")
streamed.append(samples[-1])
write_recording("streamed.xml", streamed)
traci.start(COMMON + ["--device.fcd-replay.files", "streamed.xml", "--route-steps", "1"])
try:
    traci.simulationStep(10)
    assert traci.vehicle.getLaneID("ego") == "end_0"
    assert traci.vehicle.getSpeed("ego") == 0
    traci.simulationStep(20.1)
    assert traci.vehicle.getLanePosition("ego") > 95
    traci.simulationStep(20.2)
    assert traci.simulation.getArrivedIDList() == ("ego",)
    assert traci.simulation.getMinExpectedNumber() == 0
    print("Vehicle stayed until the reader reached the later sample")
finally:
    traci.close()
