#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2013-2020 German Aerospace Center (DLR) and others.
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
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "tools"))  # noqa
from sumolib import checkBinary  # noqa

EDC = checkBinary("emissionsDrivingCycle", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', "bin"))
if len(sys.argv) > 2:
    PHEMLIGHTp = sys.argv[2]
else:
    PHEMLIGHTp = os.path.join(
        os.environ["SUMO_HOME"], "data", "emissions", "PHEMlight")
if not os.path.exists(PHEMLIGHTp):
    PHEMLIGHTp = os.path.join(os.path.dirname(
        sys.argv[0]), '..', '..', '..', "data", "emissions", "PHEMlight")

fd = open("classes.txt")
emissionClasses = fd.readlines()
fd.close()

fdo = open("results.csv", "w")
for i, ec in enumerate(emissionClasses):
    ec = ec.strip()
    if len(ec) == 0:
        continue
    drivingCycle = sys.argv[1]
    print("Running '%s'" % ec)
    sys.stdout.flush()
    sys.stderr.flush()
    call = [EDC, "-e", ec, "-t", drivingCycle, "-o", "tmp.csv",
            "--phemlight-path", PHEMLIGHTp, "--kmh", "--compute-a"]
    if drivingCycle[-4:] == ".dri":
        call += ["--timeline-file.skip", "3", "--timeline-file.separator", ","]
    retCode = subprocess.call(call)
    sys.stdout.flush()
    sys.stderr.flush()
    if retCode != 0:
        print("Error on building PHEMlight measurements")
        sys.exit(1)
    fd = open("tmp.csv")
    out = fd.readlines()
    fd.close()
    fdo.write("%s\n" % ec)
    for l in out:
        fdo.write(l)
    fdo.write("-----\n\n")
fdo.close()
