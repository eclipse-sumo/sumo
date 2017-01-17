#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Jakob Erdmann
@date    2015-02-06
@version $Id$


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
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)


def step():
    s = traci.simulation.getCurrentTime() / 1000
    traci.simulationStep()
    return s


def setGetParam(objectType, object, objectID):
    print(objectType, 'foo="%s"' % object.getParameter(objectID, "foo"))
    object.setParameter(objectID, "foo", "42")
    print(objectType, 'foo="%s"' % object.getParameter(objectID, "foo"))

print("step", step())

# XXX test PoI, Polygon
objects = [
    ("vehicle", traci.vehicle, "veh0"),
    ("person", traci.person, "ped0"),
    ("edge", traci.edge, "1o"),
    ("lane", traci.lane, "1o_0"),
    ("vType", traci.vehicletype, "pType"),
    ("route", traci.route, "horizontal"),
    ("trafficlight", traci.trafficlights, "0"),
]

[setGetParam(*x) for x in objects]
print("step", step())

traci.close()
sumoProcess.wait()
