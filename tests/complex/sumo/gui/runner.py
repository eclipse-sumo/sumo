#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
# @date    2010-10-26

from __future__ import absolute_import

import os
import subprocess
import sys
import time
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "lib"))
import testUtil  # noqa

guisimBinary = testUtil.checkBinary('sumo-gui')
for run in range(20):
    p = subprocess.Popen([guisimBinary, "-Q", "-N", "-c", "sumo.sumocfg"])
    time.sleep(1)
    testUtil.findAndClick(testUtil.PLAY)
    time.sleep(10)
    for step in range(3):
        testUtil.findAndClick(testUtil.STOP)
        time.sleep(1)
        testUtil.findAndClick(testUtil.PLAY)
        time.sleep(1)
    p.wait()
    subprocess.call(["diff", "v.xml", "vehroutes.xml"])
