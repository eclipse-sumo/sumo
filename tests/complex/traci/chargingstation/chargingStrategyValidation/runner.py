#!/usr/bin/env python3
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
"""Check invalid XML values and chargingStrategy TraCI get/set."""

import contextlib
import io
import os
import subprocess
import sys
import tempfile
from pathlib import Path

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa: E402
import traci  # noqa: E402


binary = sumolib.checkBinary("sumo")
with tempfile.TemporaryDirectory() as directory:
    additional = Path(directory) / "invalid.add.xml"
    additional.write_text('<additional><chargingStation id="cs" lane="WC_0" startPos="25" endPos="45" '
                          'chargingStrategy="unsupported"/></additional>')
    result = subprocess.run([binary, "-n", "input_net2.net.xml", "-a", str(additional), "--end", "1",
                             "--no-step-log", "true"], capture_output=True, text=True)
    assert result.returncode != 0, "invalid XML strategy was accepted"
    assert "Invalid charging" in result.stderr and "unsupported" in result.stderr, result.stderr
    print("invalid XML strategy rejected")

with contextlib.redirect_stdout(io.StringIO()):
    traci.start([binary, "-n", "input_net2.net.xml", "-a", "input_additional2.add.xml",
                 "--no-step-log", "true", "--no-duration-log", "true"])
try:
    station = "stop1"
    assert traci.chargingstation.getChargingStrategy(station) == "proportional"
    for strategy in ("max-min", "flat", "proportional"):
        traci.chargingstation.setChargingStrategy(station, strategy)
        assert traci.chargingstation.getChargingStrategy(station) == strategy
        print("TraCI", strategy, "round-trip OK")
    try:
        traci.chargingstation.setChargingStrategy(station, "unsupported")
    except traci.TraCIException:
        pass
    else:
        raise AssertionError("invalid TraCI strategy was accepted")
    assert traci.chargingstation.getChargingStrategy(station) == "proportional"
    print("invalid TraCI strategy rejected")
finally:
    traci.close()
