#!/usr/bin/env python
"""
@file    testUtil.py
@author  Michael.Behrisch@dlr.de
@date    2010-10-26
@version $Id$

This library wraps useful functions for the complex tests
and automatic GUI control
from AutoPy (http://github.com/msanders/autopy).

Copyright (C) 2010-2011 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, time
_libdir = os.path.abspath(os.path.dirname(__file__))
_bindir = os.path.abspath(os.path.join(_libdir, '..', '..', 'bin'))

try:
    import autopy

    #PLAY = autopy.bitmap.Bitmap.from_string()
    PLAY = autopy.bitmap.Bitmap.open(os.path.join(_libdir, "play.png"))
    STOP = autopy.bitmap.Bitmap.open(os.path.join(_libdir, "stop.png"))

    def findAndClick(obj):
        screen = autopy.bitmap.capture_screen()
        pos = screen.find_bitmap(obj)
        autopy.mouse.move(*pos)
        autopy.mouse.click()
except ImportError:
    pass

def checkBinary(name, bindir=_bindir):
    if name == "sumo-gui":
        envName = "GUISIM_BINARY"
    else:
        envName = name.upper() + "_BINARY"
    binary = os.environ.get(envName, os.path.join(bindir, name))
    if os.name == "nt" and binary[-4:] != ".exe":
        binary += ".exe"
    if not os.path.exists(binary):
        return name
    return binary

if __name__ == "__main__":
    findAndClick(PLAY)
    time.sleep(10)
    findAndClick(STOP)
