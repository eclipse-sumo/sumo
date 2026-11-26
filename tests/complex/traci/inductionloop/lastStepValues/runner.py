#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
# @author  Pranav Sateesh
# @date    2026-11-26


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
detID = traci.inductionloop.getIDList()[0]

for step in range(70):
    print("%s lO=%.2f lS=%.2f lL=%.2f lN=%s lI=%s tsd=%.2f" % (
        step,
        traci.inductionloop.getLastStepOccupancy(detID),
        traci.inductionloop.getLastStepMeanSpeed(detID),
        traci.inductionloop.getLastStepMeanLength(detID),
        traci.inductionloop.getLastStepVehicleNumber(detID),
        traci.inductionloop.getLastStepVehicleIDs(detID),
        traci.inductionloop.getTimeSinceDetection(detID)))
    traci.simulationStep()

traci.close()
