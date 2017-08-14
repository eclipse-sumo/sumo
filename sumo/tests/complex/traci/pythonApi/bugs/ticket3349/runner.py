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
import traci.constants as tc

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-n', 'input_net.net.xml',
                                '-r', 'input_routes.rou.xml',
                                '--no-step-log',
                                '--begin', '5',
                                #'-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)


traci.init(PORT)
vehID = "v0"
traci.vehicle.add(vehID, "r0")
traci.vehicle.subscribeContext(vehID, tc.CMD_GET_VEHICLE_VARIABLE,
        dist=20, begin=0, end=99999999)
traci.simulationStep(10000)
traci.close()
sumoProcess.wait()
