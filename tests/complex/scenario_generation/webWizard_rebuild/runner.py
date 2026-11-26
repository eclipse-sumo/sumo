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

import base64
from contextlib import redirect_stdout
import io
import json
import os
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
from zipfile import ZipFile

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
from osmWebWizard import Builder  # noqa: E402


with TemporaryDirectory() as temp, redirect_stdout(io.StringIO()):
    root = Path(temp)
    scenario = root / "original scenario"
    tools = root / "SUMO installation" / "tools"
    tools.mkdir(parents=True)
    stub = ("import json, pathlib, sys\n"
            "with open('calls.jsonl', 'a', encoding='utf-8') as output:\n"
            "    output.write(json.dumps([pathlib.Path(__file__).name] + sys.argv[1:]) + '\\n')\n")
    for name in ("ptlines2flows.py", "randomTrips.py"):
        (tools / name).write_text(stub, encoding="utf-8")

    builder = Builder({"outputDir": str(scenario), "publicTransport": True, "vehicles": {}, "poly": False}, True)
    for artifact in ("net", "guisettings", "config", "run.bat"):
        builder.filename(artifact, artifact, False)
        Path(builder.files[artifact]).touch()
    options = ["--trip-attributes", 'departLane="best"', "--seed", 43]
    builder.writeBuildScripts(["-n", str(scenario / "network with space.net.xml")], [options])
    archive = ZipFile(io.BytesIO(base64.b64decode(builder.createZip())))
    with archive:
        assert {"build.py", "build.bat"} <= {Path(name).name for name in archive.namelist()}

    moved = root / "moved scenario"
    scenario.rename(moved)
    env = dict(os.environ, SUMO_HOME=str(tools.parent))
    subprocess.run([sys.executable, str(moved / "build.py")], cwd=root, env=env, check=True)
    records = [json.loads(row) for row in (moved / "calls.jsonl").read_text(encoding="utf-8").splitlines()]
    assert records == [
        ["ptlines2flows.py", "-n", "network with space.net.xml"],
        ["randomTrips.py", "--trip-attributes", 'departLane="best"', "--seed", "43"],
    ]

print("The portable build script preserves options and works after relocation")
