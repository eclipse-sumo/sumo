#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
detID = traci.lanearea.getIDList()[0]

for step in range(120):
    print("%s iO=%.2f iS=%.2f iN=%s iI=%s lO=%.2f lS=%.2f lN=%s lI=%s" % (
        step,
        traci.lanearea.getIntervalOccupancy(detID),
        traci.lanearea.getIntervalMeanSpeed(detID),
        traci.lanearea.getIntervalVehicleNumber(detID),
        traci.lanearea.getIntervalMaxJamLengthInMeters(detID),
        traci.lanearea.getLastIntervalOccupancy(detID),
        traci.lanearea.getLastIntervalMeanSpeed(detID),
        traci.lanearea.getLastIntervalVehicleNumber(detID),
        traci.lanearea.getLastIntervalMaxJamLengthInMeters(detID)))
    traci.simulationStep()

traci.close()
