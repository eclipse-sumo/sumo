#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import time
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo-gui')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -S -Q -c sumo.sumocfg --window-size 500,500 --window-pos 50,50 --remote-port %s" %
    (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
time.sleep(1)  # give the gui a chance to draw itself
print("views", traci.gui.getIDList())
viewID = traci.gui.DEFAULT_VIEW
print("examining", viewID)
print("zoom", traci.gui.getZoom(viewID))
print("offset", traci.gui.getOffset(viewID))
print("schema", traci.gui.getSchema(viewID))
print("visible boundary", traci.gui.getBoundary(viewID))
print("has view", viewID, traci.gui.hasView(viewID))
print("has view 'foo'", traci.gui.hasView("foo"))

traci.gui.subscribe(viewID)
print(traci.gui.getSubscriptionResults(viewID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.gui.getSubscriptionResults(viewID))
traci.gui.screenshot(viewID, "out.png", 500, 500)
traci.gui.screenshot(viewID, "test.blub")
traci.gui.setBoundary(viewID, 0, 0, 500, 500)
traci.close()
sumoProcess.wait()
