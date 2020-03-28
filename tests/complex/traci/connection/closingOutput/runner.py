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
# @date    2011-07-01

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import time

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

sumoBinary = sumolib.checkBinary(sys.argv[1])
if sys.argv[1] == "sumo":
    addOption = []
else:
    addOption = ["-S", "-Q"]

traci.start([sumoBinary, "-c", "sumo.sumocfg"] + addOption)
time.sleep(10)
step = 0
while step <= 100:
    traci.simulationStep()
    vehs = traci.vehicle.getIDList()
    if vehs.index("horiz") < 0 or len(vehs) > 1:
        print("Something is false")
    step += 1
traci.simulationStep()
traci.close()
sys.stdout.flush()
