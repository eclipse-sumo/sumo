#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @date    2010-10-26
# @version $Id$

from __future__ import absolute_import

import os
import subprocess
import sys
import time
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "lib"))
import testUtil

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
