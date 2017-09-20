#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@date    2017-01-23
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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
                                '-c', 'sumo.sumocfg',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)


firstPos = None
vehID = "v0"
traci.init(PORT)
traci.simulationStep()
traci.route.add("r0", ["SC", "CN"])
traci.vehicle.add(vehID, "r0")
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        if firstPos is None:
            firstPos = traci.vehicle.getPosition(vehID)
            print("distToInternal=%s" % traci.vehicle.getDrivingDistance(vehID, ":C_10", 13.80))
        currPos = traci.vehicle.getPosition(vehID)
        print("step=%s lane=%s pos=%.2f dist=%.2f simDist=%.2f" % (
            traci.simulation.getCurrentTime() / 1000,
            traci.vehicle.getLaneID(vehID),
            traci.vehicle.getLanePosition(vehID),
            traci.vehicle.getDistance(vehID),
            traci.simulation.getDistance2D(firstPos[0], firstPos[1], currPos[0],
                                           currPos[1], isDriving=True)))

    except traci.TraCIException:
        pass

traci.close()
sumoProcess.wait()
