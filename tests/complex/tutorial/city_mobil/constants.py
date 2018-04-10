# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    constants.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2008-07-21
# @version $Id$

from __future__ import absolute_import
import os
import sys

INFINITY = 1e400

PREFIX = "park"
DOUBLE_ROWS = 8
ROW_DIST = 35
STOP_POS = ROW_DIST - 12
SLOTS_PER_ROW = 10
SLOT_WIDTH = 5
SLOT_LENGTH = 9
SLOT_FOOT_LENGTH = 5
CAR_CAPACITY = 3
CYBER_CAPACITY = 20
BUS_CAPACITY = 30
TOTAL_CAPACITY = 60
CYBER_SPEED = 5
CYBER_LENGTH = 9
WAIT_PER_PERSON = 5
OCCUPATION_PROBABILITY = 0.5
BREAK_DELAY = 1200

PORT = 8883
SUMO_HOME = os.path.realpath(os.environ.get(
    "SUMO_HOME", os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")))
sys.path.append(os.path.join(SUMO_HOME, "tools"))
try:
    from sumolib import checkBinary  # noqa
except ImportError:
    def checkBinary(name):
        return name
NETCONVERT = checkBinary("netconvert")
SUMO = checkBinary("sumo")
SUMOGUI = checkBinary("sumo-gui")
