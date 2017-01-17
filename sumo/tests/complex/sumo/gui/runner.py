#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2010-10-26
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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
