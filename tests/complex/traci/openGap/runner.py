#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2012-10-19
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), "..", "..", "..", ".."))
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc

if sys.argv[1] == "sumo":
    sumoCall = [os.environ.get("SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))]
else:
    sumoCall = [os.environ.get("GUISIM_BINARY", os.path.join(sumoHome, 'bin', 'sumo-gui'))] #, '-S', '-Q']

# id of vehicle that is controlled
followerID = "follower"
# time to simulate after gap control is released
extraTime = 50.
# Offset to trigger phase of keeping enlarged headway
POSITIONAL_EPS = 0.1

def runSingle(targetHeadway, duration, changeRate, maxDecel):
    step = 0
    traci.start(sumoCall + ["-c", "sumo.sumocfg"])
    #~ traci.init(port=54321)
    dt = traci.simulation.getDeltaT()

    traci.simulationStep()
    #print(traci.vehicle.getIDList())
    print("Fix follower's lane.")
    traci.vehicle.changeLane(followerID, 0, 500)
    print("Subscribe to leader.")
    traci.vehicle.subscribeLeader(followerID, 500)
    traci.vehicle.subscribe(followerID, [tc.VAR_SPEED])
    
    gapControlActive = False
    targetGapEstablished = False
    remainingTime = duration
    testCompleted = False
    prevLeader = ""
    while not testCompleted:
        traci.simulationStep()
        results = traci.vehicle.getSubscriptionResults(followerID)
        leader = results[tc.VAR_LEADER][0]
        leaderDist = results[tc.VAR_LEADER][1]
        followerSpeed = results[tc.VAR_SPEED]
        currentHeadway = leaderDist/followerSpeed if followerSpeed > 0 else 10000
        currentTime = traci.simulation.getTime() 
        print("Time %s: Gap 'follower'->'%s' = %.3f (headway=%.3f)"%(currentTime, leader, leaderDist, currentHeadway))
        print("'follower' speed = %s"%followerSpeed)
        if (not gapControlActive and not targetGapEstablished and currentTime > 50.):
            print("## Starting to open gap.")
            print("(followerID, targetHeadway, duration, changeRate, maxDecel) = %s"%str((followerID, targetHeadway, duration, changeRate, maxDecel)))
            if maxDecel == -1:
                traci.vehicle.openGap(followerID, targetHeadway, duration, changeRate)
            else:
                traci.vehicle.openGap(followerID, targetHeadway, duration, changeRate, maxDecel)
            gapControlActive = True
        elif gapControlActive:
            print("Current/target headway: {0:.3f}/{1}".format(currentHeadway, targetHeadway))
            currentSpacing = currentHeadway*followerSpeed
            targetSpacing = targetHeadway*followerSpeed
            if not targetGapEstablished and (leader == "" or currentSpacing > targetSpacing - POSITIONAL_EPS or prevLeader != leader):
                if (leader != "" and prevLeader != leader):
                    traci.vehicle.deactivateGapControl(followerID)
                    print("## Deactivating gap control (leader has changed).")
                    remainingTime = 0.0
                else:
                    print("## Target Headway attained.")
                targetGapEstablished = True
        if targetGapEstablished:
            remainingTime -= dt
            print("Remaining: %s"%max(0.0,remainingTime))
            if gapControlActive and remainingTime < 0.:
                gapControlActive = False
                print("## Gap control expired.")
            if remainingTime <= -extraTime:
                testCompleted = True
        prevLeader = leader
        step += 1
        
    traci.close()
    sys.stdout.flush()

sys.stdout.flush()
targetHeadway = float(sys.argv[2])
duration = float(sys.argv[3])
changeRate = float(sys.argv[4])
if (len(sys.argv) > 5):
    maxDecel = float(sys.argv[5])
else:
    maxDecel = -1
runSingle(targetHeadway, duration, changeRate, maxDecel)

