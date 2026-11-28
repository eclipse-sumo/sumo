#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
# @author  Michael Behrisch
# @date    2026-11-19

import importlib.metadata
import os
import plistlib
import sys
import tempfile
from unittest import mock

SUMO_HOME = os.environ.get(
    "SUMO_HOME", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "..", "..")
)
sys.path.insert(0, os.path.join(SUMO_HOME, "tools"))
import sumolib  # noqa

# 1. Build-tree src/version.h fallback
with tempfile.TemporaryDirectory() as tmp:
    os.makedirs(os.path.join(tmp, "src"))
    with open(os.path.join(tmp, "src", "version.h"), "w") as f:
        f.write('#define VERSION_STRING "v1_28_0+0123-abcdef01234"\n')
    print("src_version_h:", sumolib.version.fromVersionHeader(rootDir=tmp, curVersion="0.0.0"))

# 2. macOS framework Resources/Info.plist fallback
with tempfile.TemporaryDirectory() as tmp:
    share_sumo = os.path.join(tmp, "Versions", "1.28.0", "EclipseSUMO", "share", "sumo")
    res_dir = os.path.join(tmp, "Versions", "1.28.0", "Resources")
    os.makedirs(share_sumo)
    os.makedirs(res_dir)
    with open(os.path.join(res_dir, "Info.plist"), "wb") as pf:
        plistlib.dump({"CFBundleShortVersionString": "1.28.0"}, pf)
    print("macos_plist:", sumolib.version.fromVersionHeader(rootDir=share_sumo, curVersion="0.0.0"))

# 3. Sibling binary fallback (mocked subprocess)
with tempfile.TemporaryDirectory() as tmp:
    bin_dir = os.path.join(tmp, "bin")
    os.makedirs(bin_dir)
    with open(os.path.join(bin_dir, "sumo"), "w") as bf:
        bf.write("")
    sumolib.version._cached_bin_version.clear()
    with mock.patch("subprocess.check_output", return_value="Eclipse SUMO sumo Version v1_28_0+0456-123456789ab\n"):
        print("sibling_bin:", sumolib.version.fromVersionHeader(rootDir=tmp, curVersion="0.0.0"))


# 4. eclipse-sumo wheel metadata fallback when sumolib dist is absent
def fake_meta_version(pkg):
    if pkg == "sumolib":
        raise importlib.metadata.PackageNotFoundError(pkg)
    if pkg == "eclipse-sumo":
        return "1.28.0.post42"
    raise importlib.metadata.PackageNotFoundError(pkg)


with mock.patch("importlib.metadata.version", side_effect=fake_meta_version):
    det = sumolib.version._detect_metadata_version()
    print("eclipse_sumo_wheel:", sumolib.version.fromVersionHeader(rootDir="/nonexistent_sumo_dir", curVersion=det))
