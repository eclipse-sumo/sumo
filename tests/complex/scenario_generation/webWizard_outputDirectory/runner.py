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
# @author  Josh
# @date    2026-11-26

from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path
import re
from tempfile import TemporaryDirectory

import os
import sys

sys.path.insert(0, os.path.join(os.environ['SUMO_HOME'], 'tools'))
from osmWebWizard import Builder  # noqa: E402


with TemporaryDirectory() as tmp:
    root = Path(tmp).resolve()
    output = root / "scenario outputs"
    output.mkdir()
    with redirect_stdout(StringIO()):
        direct = Builder({"timestampScenarioDirectory": False}, True, str(output))
        assert Path(direct.tmp).samefile(output)
        marker = output / "existing.txt"
        marker.write_text("existing files are preserved", encoding="utf-8")
        try:
            Builder({"timestampScenarioDirectory": False}, True, str(output))
        except FileExistsError:
            assert marker.read_text(encoding="utf-8") == "existing files are preserved"
        else:
            raise AssertionError("expected a nonempty scenario directory to be rejected")
        dated = Path(Builder({}, True, str(output)).tmp)
        second = Path(Builder({}, True, str(output)).tmp)
        assert dated.parent.samefile(output) and dated.is_dir()
        assert re.match(r"^\d{4}(?:-\d{2}){5}(?:-.+)?$", dated.name)
        assert second.is_dir() and second != dated

print("Scenario output modes verified")
