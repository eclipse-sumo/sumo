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

PREFIX = "park" 
DOUBLE_ROWS = 3
ROW_DIST = 35
SLOTS_PER_ROW = 5
SLOT_WIDTH = 5
SLOT_LENGTH = 9
SLOT_FOOT_LENGTH = 5
PORT = 8813
if os.name == "posix":
#    SUMO = "../../../src/sumo"
    SUMO = "../../../src/sumo-guisim"
else:
    SUMO = "..\\..\\..\\bin\\sumoD"
#    SUMO = "..\\..\\..\\bin\\guisim"

CMD_SIMSTEP = 0x01
CMD_STOP = 0x12
CMD_CHANGETARGET = 0x31
CMD_MOVENODE = 0x80

POSITION_ROADMAP = 0x04

RESULTS = {0x00: "OK", 0x01: "Not implemented", 0xFF: "Error"}
