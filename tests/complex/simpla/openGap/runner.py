#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci  # noqa
import sumolib  # noqa
import simpla  # noqa
from simpla import SimplaException  # noqa

traci.start([sumolib.checkBinary('sumo'), '-c', 'sumo.sumocfg'])
simpla._utils.DEBUG_GAP_CONTROL = True


def step():
    s = traci.simulation.getTime()
    traci.simulationStep()
    return s


def check(vehID):
    print("examining", vehID)
    print("speed", traci.vehicle.getSpeed(vehID))
    print("speed w/o traci", traci.vehicle.getSpeedWithoutTraCI(vehID))
    print("acceleration", traci.vehicle.getAcceleration(vehID))
    print("lane", traci.vehicle.getLaneID(vehID))
    print("lanePos", traci.vehicle.getLanePosition(vehID))


followerID = "follower"
leaderID = "leader"


def runSteps(nSteps):
    for i in range(nSteps):
        print("\n# step", step())
        print("# Follower:")
        check(followerID)
        print("# Leader:")
        check(leaderID)
        leaderInfo = traci.vehicle.getLeader(followerID, 1000)
        if leaderInfo is not None:
            print("\n# gap = %s" % leaderInfo[1])
            speedDiff = traci.vehicle.getSpeed(leaderID) - traci.vehicle.getSpeed(followerID)
            print("# speedDiff = %s\n" % speedDiff)
        else:
            print("\n# No follow-lead relation!\n")


runSteps(100)

# params for next openGap call
gap = 50
speedDiff = 1.0
maxDecel = 0.1
duration = 100

print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
print("\n")

runSteps(200)

# params for next openGap call
speedDiff = 10
maxDecel = 10.  # Don't exceed vehicle's decel
duration = 50

print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
print("\n")

runSteps(100)

# params for next openGap call
gap = 75
speedDiff = 4.0
maxDecel = 2.

print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
print("\n")

# params for next openGap call
gap = -75
print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
try:
    simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
except SimplaException as e:
    print(e)
print("\n")

gap = 75
speedDiff = -4
print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
try:
    simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
except SimplaException as e:
    print(e)
print("\n")

speedDiff = 4
maxDecel = -2
print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
try:
    simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
except SimplaException as e:
    print(e)
print("\n")

maxDecel = 2
duration = -100
print("\n# Calling simpla.openGap(%s, %s, %s, %s)\n" % (gap, speedDiff, maxDecel, duration))
try:
    simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
except SimplaException as e:
    print(e)
print("\n")

duration = 51
gap = 10000
print("\n# Calling simpla.openGap() various times\n")
simpla.openGap(followerID, gap, speedDiff, maxDecel, duration)
simpla.openGap(followerID, gap * 3, speedDiff, maxDecel / 2., duration)
simpla.openGap(followerID, gap, speedDiff, maxDecel, duration / 2.)
print("\n")

runSteps(100)

# done
traci.close()
