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
# @date    2026-11-23

from pathlib import Path
import os
import shutil
import subprocess
from xml.etree import ElementTree

import sumolib


fixture = Path(os.environ["SUMO_HOME"]) / "tests/sumo/devices/ssm/recurrent_net"
routes = "input_routes.rou.xml"
# Keep the SSM output, which is relative to the route input, in the test directory.
shutil.copyfile(fixture / "ego_major_recurrent_longer_loop1_longer_range2" / routes, routes)
subprocess.check_call([sumolib.checkBinary("sumo"), "-n", str(fixture / "net2.net.xml"), "-r", routes,
                       "--step-length", "0.1", "--step-method.ballistic", "--no-step-log"],
                      stdout=subprocess.DEVNULL)

conflicts = ElementTree.parse("ssm.xml").findall("conflict")
assert len(conflicts) == 2
assert all((conflict.get("ego"), conflict.get("foe")) == ("ego1", "foe1") for conflict in conflicts)
# Encounter type 12 is EGO_ENTERED_CONFLICT_AREA.
assert conflicts[1].find("typeSpan").get("values").split()[0] == "12"
first, second = [conflict.find("PET") for conflict in conflicts]
assert 0 < float(first.get("value")) < 10
assert 0 < float(second.get("value")) < 10
assert float(first.get("time")) < float(second.get("time"))
assert first.get("position") != second.get("position")

Path("ssm.xml").replace("recurrent-pet.xml")
print("Recorded two PET values across the recurrent crossings.")
