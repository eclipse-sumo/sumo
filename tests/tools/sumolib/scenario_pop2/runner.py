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
# @date    2026-11-22

import io
import os
import random
import sys
import types
from types import SimpleNamespace
from unittest.mock import patch
import xml.etree.ElementTree as ET

DEFAULT_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")
TOOLS = os.path.join(os.environ.get("SUMO_HOME", DEFAULT_HOME), "tools")
sys.path.insert(0, TOOLS)

try:
    import matplotlib.pyplot  # noqa
except ImportError:
    sys.modules.setdefault("matplotlib", types.ModuleType("matplotlib"))
    sys.modules.setdefault("matplotlib.pyplot", types.ModuleType("matplotlib.pyplot"))

import devel.scalability as scalability  # noqa
import sumolib.scenario.pop2 as pop2  # noqa


class _MemFile(io.StringIO):
    def close(self):
        pass


buf = io.StringIO()
scalability.write_flows(buf, 2, 50)
flows = ET.fromstring(buf.getvalue()).findall("flow")
assert len(flows) == 2 and all("departSpeed" not in f.attrib for f in flows)

mem1 = _MemFile()
random.seed(42)
with patch("builtins.open", return_value=mem1):
    pop2.ScenarioSet_SinSinDemand({}).genDemand(SimpleNamespace(demandName="mem1.rou.xml"), 20, 0.0, 1.0)
vehs1 = ET.fromstring(mem1.getvalue()).findall("vehicle")
assert len(vehs1) > 0 and all("departSpeed" not in v.attrib for v in vehs1)

mem2 = _MemFile()
random.seed(42)
with patch("builtins.open", return_value=mem2):
    pop2.ScenarioSet_OneSinDemand({}).genDemand(SimpleNamespace(demandName="mem2.rou.xml"), 20, 0.0, 1.0)
vehs2 = ET.fromstring(mem2.getvalue()).findall("vehicle")
assert len(vehs2) > 0 and all("departSpeed" not in v.attrib for v in vehs2)

print("OK flows=%d vehicles=%d,%d without departSpeed" % (len(flows), len(vehs1), len(vehs2)))
