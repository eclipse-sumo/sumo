#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2026 German Aerospace Center (DLR) and others.
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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2013-01-14

from __future__ import absolute_import
from __future__ import print_function

import sys
import os
import subprocess
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib import checkBinary  # noqa

EDC = checkBinary("emissionsMap", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', "bin"))

fdo = open("results.csv", "w")
ec = sys.argv[1].strip()
print("Running '%s'" % ec)
sys.stdout.flush()
sys.stderr.flush()
call = [EDC, "-e", ec, "-o", "tmp.csv",
        "--v-max", "30",
        "--v-step", "5",
        "--a-step", "1",
        "--s-min", "0",
        "--s-max", "0",
        ]
call += sys.argv[2:]
retCode = subprocess.call(call)
sys.stdout.flush()
sys.stderr.flush()
if retCode != 0:
    print("Error on building PHEMlight measurements")
    sys.exit(1)
fdo.write("%s\n" % ec)
with open("tmp.csv") as fd:
    fdo.write(fd.read())
fdo.write("-----\n\n")
fdo.close()
