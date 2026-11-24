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
# @date    2026-11-26

import os
from pathlib import Path
import subprocess
import sys

tests = Path(__file__).resolve().parents[4]
sumoHome = Path(os.environ.get("SUMO_HOME", tests.parent))
sys.path.append(str(sumoHome / "tools"))
import sumolib  # noqa

net = tests / "sumo/meso/bugs/ticket16014/net.net.xml"
cmd = [sumolib.checkBinary("sumo"), "-n", str(net), "-r", "input_routes.rou.xml",
       "--mesosim", "--no-step-log", "--duration-log.disable"]


def count_warnings(recheck):
    result = subprocess.run(cmd + ["--meso-recheck", recheck], text=True, capture_output=True, check=True)
    return result.stderr.count("Option --meso-recheck delays checking entry to short edge '-E0'")


assert count_warnings("0") == 0
assert count_warnings("5") == 1
print("Short-edge recheck warning was emitted once.")
