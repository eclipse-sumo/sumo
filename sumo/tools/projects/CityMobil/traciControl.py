#!/usr/bin/env python
"""
@file    traciControl.py
@author  Michael.Behrisch@dlr.de
@date    2008-07-21
@version $Id$

Control the CityMobil parking lot via TraCI.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import subprocess, socket, time, struct
from constants import *

sumoProcess = subprocess.Popen("%s -c %s.sumo.cfg" % (SUMO, PREFIX))
time.sleep(1)
sock = socket.socket()
sock.connect(("localhost", PORT))

command = struct.pack("!BBdB", 1+1+8+1, CMD_SIMSTEP, 1., POSITION_ROADMAP)
length = struct.pack("!i", len(command)+4)
sock.send(length)
sock.send(command)
result = sock.recv(256)
time.sleep(5)
prefixFmt = "!iBBBi"
prefix = struct.unpack(prefixFmt, result[:struct.calcsize(prefixFmt)])
print prefix
print struct.unpack("%ss" % prefix[4], result[struct.calcsize(prefixFmt):])
time.sleep(100)
