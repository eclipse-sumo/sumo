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

from pathlib import Path
import subprocess

import sumolib


Path("cycle.csv").write_text("0;0\n1;5\n2;10\n3;10\n", encoding="utf-8")
Path("types.add.xml").write_text(
    '<additional><vType id="other" emissionClass="HBEFA4/default"/>'
    '<vType id="electric" emissionClass="Energy/unknown"/></additional>',
    encoding="utf-8")
command = [sumolib.checkBinary("emissionsDrivingCycle"), "--timeline-file", "cycle.csv",
           "--compute-a", "--quiet", "--additional-files", "types.add.xml", "--vtype", "electric"]


def run(name, *options):
    output = Path(name + ".csv")
    subprocess.check_call(command + ["--output-file", str(output), *options], stdout=subprocess.DEVNULL)
    return output.read_text(encoding="utf-8").splitlines()


inherited = run("inherited")
electric = run("electric", "--emission-class", "Energy/unknown")
override = run("override", "--emission-class", "HBEFA4/default")
assert len(inherited) >= 3
assert inherited == electric, "The inherited vType class must match the explicitly selected electric class"
assert inherited != override, "An explicit class must override the vType class"
print("The vType supplies the emission class, and an explicit class overrides it.")
