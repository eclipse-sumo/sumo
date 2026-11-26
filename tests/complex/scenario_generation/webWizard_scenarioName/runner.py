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
import io
import os
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
from unittest.mock import patch
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
from osmWebWizard import Builder  # noqa: E402


with TemporaryDirectory() as tmp, redirect_stdout(io.StringIO()):
    output = Path(tmp) / "output"
    data = {
        "outputDir": str(output),
        "scenarioName": "M\u00fcnchen / West",
        "decal": False,
        "publicTransport": False,
    }
    builder = Builder(data, True)
    assert Path(builder.tmp) == output
    builder.filename("net", ".net.xml.gz")
    builder.additionalFiles = []
    builder.routenames = []
    orig_call = subprocess.call
    recorded = []

    def quiet_call(cmd, **kwargs):
        recorded.append((cmd, kwargs))
        return orig_call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, **kwargs)

    with patch("osmWebWizard.subprocess.call", side_effect=quiet_call):
        builder.makeConfigFile()
    cmd, kwargs = recorded[0]
    assert kwargs == {"cwd": str(output)}
    assert cmd[cmd.index("-n") + 1] == "Munchen-West.net.xml.gz"
    assert cmd[cmd.index("--save-configuration") + 1] == "Munchen-West.sumocfg"
    assert (output / "Munchen-West.view.xml").is_file()
    assert (output / "Munchen-West.sumocfg").is_file()
    cfg_root = ET.parse(output / "Munchen-West.sumocfg").getroot()
    assert cfg_root.find("./input/net-file").get("value") == "Munchen-West.net.xml.gz"
    builder.createBatch()
    assert (output / "run.bat").read_text() == "sumo-gui -c Munchen-West.sumocfg"

    for name, expected in [(None, "osm"), ("", "osm"), ("../", "osm"), ("AUX", "AUX-scenario")]:
        with patch("osmWebWizard.tempfile.mkdtemp", return_value=tmp):
            assert Builder({"scenarioName": name}, False).prefix == expected

print("Scenario name used for generated files and configuration")
