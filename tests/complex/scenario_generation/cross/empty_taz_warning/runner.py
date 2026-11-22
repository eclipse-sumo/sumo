#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-21

import os
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa

visum_in = os.path.join(
    os.environ["SUMO_HOME"], "tests", "netconvert", "import", "visum", "orinoko_kp577", "visum.net"
)
proc = subprocess.run(
    [
        sumolib.checkBinary("netconvert"),
        "--visum-file", visum_in,
        "--taz-output", "taz.add.xml",
        "-o", "out.net.xml",
    ],
    capture_output=True, text=True, check=True,
)
for ln in proc.stderr.splitlines():
    if "TAZ" in ln:
        print(ln.strip())
