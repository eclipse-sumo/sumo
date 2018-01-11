#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2015-02-06
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
cmd = [sumoBinary,
    "-n", "input_net2.net.xml",
    "-r", "input_routes.rou.xml",
    "--fcd-output", "fcd.xml",
    "--no-step-log"] 
traci.start(cmd)


def step():
    s = traci.simulation.getCurrentTime() / 1000
    traci.simulationStep()
    return s

p = "p0"
s = step()
x, y = traci.person.getPosition(p)
print("s=%s x=%s y=%s" % (s, x, y))
traci.person.moveToXY(p, "", x, y-10)
s = step()
x, y = traci.person.getPosition(p)
print("s=%s x=%s y=%s" % (s, x, y))

traci.close()
