#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-09-25
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function


import os
import subprocess
import sys
import shutil
sys.path.append(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(
    os.path.dirname(__file__), "..", "..", "..")), "tools"))
from sumolib import checkBinary  # noqa


netconvertBinary = checkBinary('netconvert')
sumoBinary = checkBinary('sumo')
# build/check network
retcode = subprocess.call(
    [netconvertBinary, "-c", "data/quickstart.netccfg"], stdout=sys.stdout, stderr=sys.stderr)
try:
    shutil.copy("data/quickstart.net.xml", "net.net.xml")
except IOError:
    print("Missing 'quickstart.net.xml'")
print(">> Netbuilding closed with status %s" % retcode)
sys.stdout.flush()
# run simulation
retcode = subprocess.call(
    [sumoBinary, "-c", "data/quickstart.sumocfg", "--no-step-log"], stdout=sys.stdout, stderr=sys.stderr)
print(">> Simulation closed with status %s" % retcode)
sys.stdout.flush()
