#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2026 German Aerospace Center (DLR) and others.
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
# @date    2022-10-27

from __future__ import absolute_import
from __future__ import print_function

import os
import glob
import sys
import subprocess
import datetime

TOOLS = os.path.join(os.environ["SUMO_HOME"], "tools")

# the following two lines are basically a hack to save the osm data on the test server for later inspection
report = os.environ.get("SUMO_REPORT", "")
osm_history = os.path.join(report, datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S"))
out_dir = osm_history if os.environ.get("FILEPREFIX") == "gcc" else "test"
subprocess.call([sys.executable,
                 os.path.join(TOOLS, "osmWebWizard.py"), "-b=7:0:0", "-e=8:0:0",
                 "--bbox=13.381507,52.511801,13.417790,52.527748", "--test-output", out_dir,
                 "-n=--aggregate-warnings=0", "--remote"])
net_file = os.path.join(out_dir, "osm.net.xml.gz")
if not os.path.exists(net_file):
    # download failed, fall back to the newest saved download which is at most 10 days old
    limit = datetime.datetime.now() - datetime.timedelta(days=10)
    for d in sorted(glob.glob(os.path.join(report, "????-??-??-??-??-??")), reverse=True):
        try:
            stamp = datetime.datetime.strptime(os.path.basename(d), "%Y-%m-%d-%H-%M-%S")
        except ValueError:
            continue
        old_net = os.path.join(d, "osm.net.xml.gz")
        if stamp >= limit and os.path.exists(old_net):
            print("Download failed, using", old_net, file=sys.stderr)
            net_file = old_net
            break
subprocess.call([sys.executable,
                 os.path.join(TOOLS, "import", "gtfs", "gtfs2pt.py"),
                 "-n", net_file] + sys.argv[1:])
