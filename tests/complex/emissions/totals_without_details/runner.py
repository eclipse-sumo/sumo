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
# @date    2026-11-23

import os
from pathlib import Path
import subprocess
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa


def run(*options):
    result = subprocess.run([checkBinary("emissionsDrivingCycle"), *options],
                            stdout=subprocess.PIPE, text=True, check=True)
    return result.stdout.splitlines()


for label, options in (
    ("timeline", ("-t", "data/cycle.csv")),
    ("netstate", ("-n", "data/netstate.xml", "--compute-a")),
    ("Amitran", ("-n", "data/amitran.xml")),
):
    actual = run(*options)
    detailed = run(*options, "-o", "detail.csv")
    expected = [line for line in detailed if ";" not in line]
    assert actual == expected, (actual, expected)
    assert any(float(line.split(":", 1)[1]) > 0 for line in actual if line.startswith("CO2:")), actual
    print("Aggregate output matches for %s input." % label)

run("-t", "data/cycle.csv", "--sum-output", "totals.csv", "--quiet")
run("-t", "data/cycle.csv", "--sum-output", "totals-with-details.csv", "-o", "detail.csv", "--quiet")
assert Path("totals.csv").read_bytes() == Path("totals-with-details.csv").read_bytes()
print("Normalized timeline totals match.")
