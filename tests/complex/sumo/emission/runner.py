#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @date    2012-01-14
# @version $Id$

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
import sumolib  # noqa

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
