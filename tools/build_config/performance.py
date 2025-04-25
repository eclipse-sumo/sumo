#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    performance.py
# @author  Michael Behrisch
# @date    2023-10-02

"""
This script builds all sumo versions in a certain revision range
and tries to eliminate duplicates afterwards.
"""

import os
import sys
import subprocess
import glob
import time
import psutil

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


arg_parser = sumolib.options.ArgumentParser()
arg_parser.add_argument("cfg", nargs="+", help="sumo config to run")
arg_parser.add_argument("-d", "--version-dir", default=".history", help="where to find sumo versions")
arg_parser.add_argument("-r", "--repeat", type=int, default=5, help="number of runs")
options = arg_parser.parse_args()

versions = []
for sumo in glob.glob(os.path.join(options.version_dir, "bin*", "sumo")):
    versions.append(([int(d) for d in sumo[len(options.version_dir) + 5:-5].split("_")], sumo))
versions.sort()

for c in options.cfg:
    for version, sumo in versions:
        times = []
        mems = []
        for _ in range(options.repeat):
            p = subprocess.Popen([sumo, c], stdout=subprocess.DEVNULL)
            ps = psutil.Process(p.pid)
            utime = None
            mem = 0
            while p.poll() is None:
                with ps.oneshot():
                    utime = ps.cpu_times().user
                    mem = max(mem, ps.memory_full_info().uss)
                time.sleep(0.1)
            times.append(utime)
            mems.append(mem)
        times.sort()
        mems.sort()
        num = options.repeat
        if num > 4:
            times = times[1:-1]
            mems = mems[1:-1]
            num -= 2
        print(".".join(map(str, version)), "%.2f" % (sum(times) / num), "%.2f" % (sum(mems) / num / 2 ** 20))
