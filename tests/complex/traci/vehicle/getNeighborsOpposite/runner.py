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
# @author  Jakob Erdmann
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


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


egoID = "ego"


def printNeighInfo():
    print("Neigh infos for ego:")
    rightFollowers = traci.vehicle.getRightFollowers(egoID)
    print("  rightFollowers:\n  %s" % str(rightFollowers))
    rightLeaders = traci.vehicle.getRightLeaders(egoID)
    print("  rightLeaders:\n  %s" % str(rightLeaders))
    leftFollowers = traci.vehicle.getLeftFollowers(egoID)
    print("  leftFollowers:\n  %s" % str(leftFollowers))
    leftLeaders = traci.vehicle.getLeftLeaders(egoID)
    print("  leftLeaders:\n  %s" % str(leftLeaders))
    rightFollowersBlocking = traci.vehicle.getRightFollowers(egoID, True)
    print("  rightFollowers (only blocking):\n  %s" % str(rightFollowersBlocking))
    rightLeadersBlocking = traci.vehicle.getRightLeaders(egoID, True)
    print("  rightLeaders (only blocking):\n  %s" % str(rightLeadersBlocking))
    leftFollowersBlocking = traci.vehicle.getLeftFollowers(egoID, True)
    print("  leftFollowers (only blocking):\n  %s" % str(leftFollowersBlocking))
    leftLeadersBlocking = traci.vehicle.getLeftLeaders(egoID, True)
    print("  leftLeaders (only blocking):\n  %s" % str(leftLeadersBlocking))
    sys.stdout.flush()


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])

for i in range(1):
    print("step", step())

while egoID in traci.vehicle.getIDList():
    printNeighInfo()
    print("step", step())
step()
# done
traci.close()
