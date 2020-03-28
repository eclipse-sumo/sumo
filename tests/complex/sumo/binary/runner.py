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
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2010-03-02

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
sys.path.append(
    os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
import sumolib  # noqa

netconvertBinary = sumolib.checkBinary('netconvert')
duarouterBinary = sumolib.checkBinary('duarouter')
sumoBinary = sumolib.checkBinary('sumo')

print(">>> Building the xml network")
sys.stdout.flush()
subprocess.call([netconvertBinary, "-c", "netconvert.netccfg"],
                stdout=sys.stdout, stderr=sys.stderr)
print(">>> Building the binary network")
sys.stdout.flush()
subprocess.call([netconvertBinary, "-c", "netconvert.netccfg",
                 "-o", "circular.net.xml.gz"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print(">>> Converting the routes to binary")
subprocess.call([duarouterBinary, "-c", "duarouter.duarcfg"],
                stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
print(">>> Running Simulation with binary input")
subprocess.call(
    [sumoBinary, "-c", "sumo.sumocfg"], stdout=sys.stdout, stderr=sys.stderr)
