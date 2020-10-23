#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @date    2012-01-14

from __future__ import absolute_import

import os
import subprocess
import sys
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
