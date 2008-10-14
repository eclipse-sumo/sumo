"""
@file    constants.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Defining constants for the CityMobil parking lot.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os

INFINITY = 1e400

PREFIX = "park" 
DOUBLE_ROWS = 3
ROW_DIST = 35
SLOTS_PER_ROW = 8
SLOT_WIDTH = 5
SLOT_LENGTH = 9
SLOT_FOOT_LENGTH = 5
CAR_CAPACITY = 4
CYBER_CAPACITY = 10
CYBER_SPEED = 5
WAIT_PER_PERSON = 5

PORT = 8813
if os.name == "posix":
    SUMO = "../../../src/sumo"
    SUMOGUI = "../../../src/sumo-guisim"
else:
    SUMO = "..\\..\\..\\bin\\sumo"
    SUMOGUI = "..\\..\\..\\bin\\guisim"
