#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-23

import os
from pathlib import Path
import shutil
import subprocess
import sys

sumoHome = Path(os.environ["SUMO_HOME"])
sys.path.append(str(sumoHome / "tools"))
import sumolib  # noqa

subprocess.run([sumolib.checkBinary("netgenerate"), "--grid", "--no-internal-links", "-o", "generated.net.xml"],
               capture_output=True, check=True)
installedHome = Path("installation prefix").resolve()
shutil.copytree(sumoHome / "data" / "xsd", installedHome / "share" / "sumo" / "data" / "xsd")

cases = [
    ([sumolib.checkBinary("sumo"), "-n", "generated.net.xml", "-r", "input_routes.rou.xml",
      "--xml-validation.routes", "local", "--no-step-log"], "trip"),
    ([sumolib.checkBinary("netconvert"), "-s", "generated.net.xml", "--type-files", "input_types.typ.xml",
      "--xml-validation", "local", "-o", "out.net.xml"], "type"),
]
for name, home in (("Direct data directory", sumoHome), ("Installed data directory", installedHome)):
    for command, element in cases:
        result = subprocess.run(command, env=dict(os.environ, SUMO_HOME=str(home)), capture_output=True, text=True)
        diagnostics = result.stdout + result.stderr
        assert result.returncode != 0, diagnostics
        assert "attribute 'unexpected' is not declared for element '%s'" % element in diagnostics, diagnostics
        assert "SUMO_HOME is not set properly" not in diagnostics, diagnostics
    print(name + " validates routes and types")
