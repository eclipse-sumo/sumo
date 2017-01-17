#!/usr/bin/env python
"""
@file    runner.py
@author  Michael Behrisch
@date    2012-01-14
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
import threading
import socket
import difflib
toolDir = os.path.join(
    os.path.dirname(__file__), '..', '..', '..', '..', "tools")
sys.path.append(toolDir)
import sumolib

sumoBinary = sumolib.checkBinary('sumo')
driveCycleBinary = sumolib.checkBinary('emissionsDrivingCycle')

# file output direct
subprocess.call([sumoBinary, "-c", "sumo.sumocfg", "--amitran-output",
                 "trajectory.xml", "--emission-output", "emissions.xml"])
# filter trajectories and compare results
subprocess.call([driveCycleBinary, "--amitran",
                 "trajectory.xml", "--emission-output", "emissionsCycle.xml"])
subprocess.call([driveCycleBinary, "--compute-a", "--amitran",
                 "trajectory.xml", "--emission-output", "emissionsCycleNoA.xml"])

# for line in difflib.unified_diff(open('emissions.xml').readlines(), open('emissionsCycle.xml').readlines(), n=0):
#    sys.stdout.write(line)
