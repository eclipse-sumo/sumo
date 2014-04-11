#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2012-01-14
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os,subprocess,sys,time,threading,socket,difflib
toolDir = os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools")
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

sumoBinary = os.path.join(toolDir, '..', 'bin', 'sumo')
driveCycleBinary = os.path.join(toolDir, '..', 'bin', 'emissionsDrivingCycle')

# file output direct
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "--amitran-output", "trajectory.xml", "--emission-output", "emissions.xml"])
# filter trajectories and compare results
subprocess.call([driveCycleBinary, "--amitran", "trajectory.xml", "--emission-output", "emissionsCycle.xml"])
subprocess.call([driveCycleBinary, "--compute-a", "--amitran", "trajectory.xml", "--emission-output", "emissionsCycleNoA.xml"])

#for line in difflib.unified_diff(open('emissions.xml').readlines(), open('emissionsCycle.xml').readlines(), n=0):
#    sys.stdout.write(line)
