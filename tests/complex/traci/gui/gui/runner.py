#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import time
from matplotlib.pyplot import imread
REL_HOME = os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..")
SUMO_HOME = os.environ.get('SUMO_HOME', REL_HOME)
sys.path.append(os.path.join(SUMO_HOME, "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo-gui'),
             "-S", "-Q",
             "-c", "sumo.sumocfg",
             "--delay", "1",
             "--window-size", "500,500",
             "--window-pos", "50,50"],
            stdout=sys.stdout)
for step in range(3):
    print("step", step)
    traci.simulationStep()
time.sleep(1)  # give the gui a chance to draw itself
print("hasGUI", traci.hasGUI())
print("views", traci.gui.getIDList())
viewID = traci.gui.DEFAULT_VIEW
print("examining", viewID)
print("set zoom (80.0)")
traci.gui.setZoom(viewID, 80.0)
print("get zoom", traci.gui.getZoom(viewID))
print("set offset (400.0, 400.0)")
traci.gui.setOffset(viewID, 400.0, 400.0)
print("get offset", traci.gui.getOffset(viewID))
print("set schema ('real world')")
traci.gui.setSchema(viewID, "real world")
print("get schema", traci.gui.getSchema(viewID))
traci.gui.setSchema(viewID, "standard")  # reset schema to retain original screenshot
print("visible boundary", traci.gui.getBoundary(viewID))
print("has view", viewID, traci.gui.hasView(viewID))
print("has view 'foo'", traci.gui.hasView("foo"))
print("track vehicle 'horiz'")
traci.gui.trackVehicle(viewID, "horiz")
print("get tracked vehicle", traci.gui.getTrackedVehicle(viewID))

traci.gui.subscribe(viewID)
print(traci.gui.getSubscriptionResults(viewID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.gui.getSubscriptionResults(viewID))
traci.gui.setBoundary(viewID, 0, 0, 500, 500)
traci.gui.screenshot(viewID, "out.png", 500, 500)
traci.gui.screenshot(viewID, "test.blub")
print("veh selected:", traci.gui.isSelected("horiz"))
print("edge selected:", traci.gui.isSelected("2fi", "edge"))
traci.gui.toggleSelection("horiz")
traci.gui.toggleSelection("2fi", "edge")
print("veh selected:", traci.gui.isSelected("horiz"))
print("edge selected:", traci.gui.isSelected("2fi", "edge"))
traci.simulationStep()
pic = imread("out.png")
print("screenshot dimensions", pic.shape)
print("adding view")
traci.gui.addView("newView", "real world")
for i in range(100):
    traci.simulationStep()
print("views", traci.gui.getIDList())
print("removing view")
traci.gui.removeView("newView")
for i in range(100):
    traci.simulationStep()
print("views", traci.gui.getIDList())
traci.close()
