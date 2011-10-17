# -*- coding: utf-8 -*-
"""
@file    constants.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Defining constants for the CityMobil parking lot.

Copyright (C) 2008-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""
import os

INFINITY = 1e400

PREFIX = "park"
DOUBLE_ROWS = 8
ROW_DIST = 35
STOP_POS = ROW_DIST-12
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

PORT = 8813
BIN_DIR = os.path.join(os.path.dirname(__file__), "..", "..", "..", "bin")
NETCONVERT = os.path.join(BIN_DIR, "netconvert")
SUMO = os.path.join(BIN_DIR, "sumo")
SUMOGUI = os.path.join(BIN_DIR, "sumo-gui")
