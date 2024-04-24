#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
import warnings
warnings.filterwarnings("ignore", category=ResourceWarning)  # pyscreeze leaves files open
import pyautogui  # noqa

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa


PLAY = os.path.join("data", "play.png")
STOP = os.path.join("data", "stop.png")


def findAndClick(obj):
    positionOnScreen = pyautogui.locateOnScreen(obj, minSearchTime=3, confidence=0.9)
    pyautogui.moveTo(positionOnScreen)
    pyautogui.click()


guisimBinary = sumolib.checkBinary('sumo-gui')
for run in range(3):
    p = subprocess.Popen([guisimBinary, "-Q", "-c", "sumo.sumocfg"])
    time.sleep(1)
    findAndClick(PLAY)
    time.sleep(10)
    for step in range(3):
        findAndClick(STOP)
        time.sleep(1)
        findAndClick(PLAY)
        time.sleep(1)
    p.wait()
