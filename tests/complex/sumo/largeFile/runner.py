#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @date    2021-01-13

import os
import subprocess
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


subprocess.call([sumolib.checkBinary("netgenerate"), "--grid", "-o", "test.net.xml"])
large = 2**20 * " "
with open("test.net.xml", "a") as net:
    for _ in range(4 * 2**10):
        net.write(large)
subprocess.call([sumolib.checkBinary('sumo'), "-n", "test.net.xml", "--no-step-log"])
os.remove("test.net.xml")
