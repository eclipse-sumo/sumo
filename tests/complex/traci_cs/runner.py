#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2019-2026 German Aerospace Center (DLR) and others.
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
# @date    2019-05-01

from __future__ import absolute_import
from __future__ import print_function


import os
import shutil
import subprocess
import sys
import glob
if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa

# use latest version
useLibsumo = 'LIBSUMO_AS_TRACI' in os.environ
sources = os.path.join(os.environ['SUMO_HOME'], "bin",
                       "libsumocs-sources.zip" if useLibsumo else "libtracics-sources.zip")
assert os.path.exists(sources)

shutil.unpack_archive(sources, "data")
files = []
for f in sys.argv[1:]:
    fname = "data/%s.cs" % f
    if useLibsumo:
        with open(fname, encoding="utf8") as fin:
            filedata = fin.read()
        with open(fname, 'w', encoding="utf8") as fob:
            fob.write(filedata.replace('Libtraci', 'Libsumo'))
    files.append(f + ".cs")
files += [f[5:].replace("\\", "/") for f in glob.glob("data/*/*.cs")]
os.environ["PATH"] += os.pathsep + os.path.join(os.environ['SUMO_HOME'], "bin")
if os.name == "nt":
    with open("data/CMakeLists.txt", "w") as cmakelists:
        print("""cmake_minimum_required(VERSION 3.8)
    project(TraCITestCS LANGUAGES CSharp)
    add_executable(TraCITestCS
        %s)""" % "\n        ".join(files), file=cmakelists)
    subprocess.check_call(["cmake", "-B", "build", "data"])
    subprocess.check_call(["cmake", "--build", "build", "--config", "Release"])
    subprocess.check_call([os.path.join("build", "Release", "TraCITestCS.exe")])
else:
    subprocess.check_call(["mcs"] + files, cwd="data")
    os.environ["LD_LIBRARY_PATH"] = os.path.join(os.environ['SUMO_HOME'], "bin")
    subprocess.check_call(["mono", os.path.join("data", "Program.exe")])
