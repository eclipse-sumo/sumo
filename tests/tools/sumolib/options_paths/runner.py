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

import os
from pathlib import Path
import sys
from tempfile import TemporaryDirectory

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.options import ArgumentParser  # noqa: E402


with TemporaryDirectory() as directory:
    cfg = Path(directory, "example.toolcfg")
    cfg.write_text("""<configuration>
    <net-file value="maps/city map.net.xml"/>
    <routes value="morning.rou.xml,https://example.invalid/remote.rou.xml,evening.rou.xml"/>
    <empty-file value=""/>
    <literal-file value="literal.xml"/>
    <explicit-path value="explicit.xml"/>
    <label value="unchanged"/>
</configuration>
""", encoding="utf-8")
    parser = ArgumentParser()
    parser.add_argument("--net-file", type=parser.net_file)
    parser.add_argument("--routes", type=parser.route_file_list)
    parser.add_argument("--empty-file", type=parser.file)
    parser.add_argument("--literal-file", type=parser.data_file, fix_path=False)
    parser.add_argument("--explicit-path", fix_path=True)
    parser.add_argument("--label")
    options = parser.parse_args(["-c", str(cfg)])
    assert Path(options.net_file) == cfg.parent / "maps" / "city map.net.xml"
    morning, remote, evening = options.routes.split(",")
    assert Path(morning) == cfg.parent / "morning.rou.xml"
    assert remote == "https://example.invalid/remote.rou.xml"
    assert Path(evening) == cfg.parent / "evening.rou.xml"
    assert options.empty_file == ""
    assert options.literal_file == "literal.xml"
    assert Path(options.explicit_path) == cfg.parent / "explicit.xml"
    assert options.label == "unchanged"
    overridden = parser.parse_args(["-c", str(cfg), "--net-file", "working.net.xml"])
    assert overridden.net_file == "working.net.xml"

print("Tool configuration files resolve their relative file options.")
