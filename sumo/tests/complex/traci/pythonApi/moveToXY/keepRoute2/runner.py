#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@date    2017-01-23
@version $Id: runner.py 24854 2017-06-22 13:21:09Z behrisch $


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-n', 'input_net.net.xml',
                                '--no-step-log',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)

vehID = "v0"
def check(x, y, comment):
    traci.vehicle.moveToXY(vehID, "", 0, x, y, keepRoute=2)
    traci.simulationStep()
    x2, y2 = traci.vehicle.getPosition(vehID)
    if x == x2 and y == y2:
        #print(comment, "success")
        pass
    else:
        print(comment, "failed: x=%s, x2=%s, y=%s, y2=%s" % (x, x2, y, y2))


traci.init(PORT)
traci.simulationStep()
traci.route.add("beg", ["beg"])
traci.vehicle.add(vehID, "beg")
check(-1, 0, "1m before the start of the edge")
check(-5, 0, "5m before the start of the edge")
traci.close()
sumoProcess.wait()
