#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2014-03-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys,time,threading,socket,difflib
toolDir = os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools")
if 'SUMO_HOME' in os.environ:
    toolDir = os.path.join(os.environ['SUMO_HOME'], "tools")
sys.path.append(toolDir)
import sumolib

def connect(inPort, outPort):
    i = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    i.connect(("localhost", inPort))
    o = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    o.connect(("localhost", outPort))
    while 1:
        data = i.recv(1024)
        if not data: break
        o.sendall(data)
    o.close()
    i.close()

SUMO_PORT = 8089
IN_PORT = 8090
OUT_PORT = 8091
sumoBinary = sumolib.checkBinary('sumo')
xmlProtoPy = os.path.join(toolDir, 'xml', 'xml2protobuf.py')
protoXmlPy = os.path.join(toolDir, 'xml', 'protobuf2xml.py')
schema = os.path.join(toolDir, '..', 'data', 'xsd', 'amitran', 'trajectories.xsd')

# file output direct
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "--amitran-output", "direct.xml"])

# protobuf roundtrip
xPro = subprocess.Popen(['python', xmlProtoPy, '-x', schema, '-o', str(IN_PORT), str(SUMO_PORT)])
pPro = subprocess.Popen(['python', protoXmlPy, '-x', schema, str(OUT_PORT)])
time.sleep(1) # wait for all servers to start
sumoPro = subprocess.Popen([sumoBinary, "sumo.sumocfg"])
time.sleep(1) # wait for all servers to start
connect(IN_PORT, OUT_PORT)
sumoPro.wait()
pPro.wait()
xPro.wait()

for line in difflib.unified_diff(open('direct.xml').readlines(), open('%s.xml' % OUT_PORT).readlines(), n=0):
    sys.stdout.write(line)
