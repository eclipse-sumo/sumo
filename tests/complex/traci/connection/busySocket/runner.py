#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @date    2011-03-02

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
import shutil
import socket

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

sumoBinary = sumolib.checkBinary(sys.argv[1])
if sys.argv[1] == "sumo":
    addOption = ["-c", "sumo.sumocfg"]
else:
    addOption = ["-S", "-Q", "-c", "sumo_log.sumocfg"]
PORT = sumolib.miscutils.getFreeSocketPort()
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', PORT))

sumoProc = subprocess.Popen([sumoBinary, "--remote-port", str(PORT)] + addOption,
                            stdout=sys.stdout)
try:
    traci.init(PORT)
    traci.close()
except traci.FatalTraCIError as e:
    print(e, file=sys.stderr)
sumoProc.wait()
sys.stdout.flush()
if os.path.exists("lastrun.stderr"):
    f = open("lastrun.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
s.close()
