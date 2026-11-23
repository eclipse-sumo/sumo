#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @date    2026-11-23

import os
import subprocess
import sys
import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


binary = [sumolib.checkBinary("sumo"), "--net-file", "net.net.xml",
          "--additional-files", "input_additional.add.xml", "--no-step-log", "--no-duration-log"]
if "--mesosim" in sys.argv:
    binary.append("--mesosim")

subprocess.check_call(binary + ["--route-files", "input_routes.rou.xml",
                                "--save-state.transportables", "--save-state.times", "250,400",
                                "--save-state.files", "access-in-progress.xml,access-completed.xml",
                                "--tripinfo-output", "original-tripinfo.xml"])
original = ET.parse("original-tripinfo.xml").getroot()
assert len(original.findall("./personinfo/access")) == 2
for name in ("in-progress", "completed"):
    filename = "restored-%s.xml" % name
    subprocess.check_call(binary + ["--load-state", "access-%s.xml" % name, "--tripinfo-output", filename])
    restored = ET.parse(filename).getroot()
    assert ET.tostring(original) == ET.tostring(restored), name

print("Tripinfo matched after restoring during the second access and after both accesses.")
