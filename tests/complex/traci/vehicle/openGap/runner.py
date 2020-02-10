#!/usr/bin/env python
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
# @author  Leonhard Luecken
# @date    2018-11-02

from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa
import traci.constants as tc  # noqa
sumoBinary = sumolib.checkBinary(sys.argv[1])

# id of vehicle that is controlled
followerID = "follower"
# id of vehicle that is followed
leaderID = "leader"
# time to simulate after gap control is released
extraTime = 50.
# Offset to trigger phase of keeping enlarged headway
POSITIONAL_EPS = 0.1


def runSingle(targetTimeHeadway, targetSpaceHeadway, duration, changeRate, maxDecel, refVehID):
    step = 0
    traci.start([sumoBinary, "-c", "sumo.sumocfg"])
    # traci.init(port=54321)
    dt = traci.simulation.getDeltaT()

    traci.simulationStep()
    # print(traci.vehicle.getIDList())
    print("Fix follower's lane.")
    traci.vehicle.changeLane(followerID, 0, 500)
    print("Subscribe to leader.")
    traci.vehicle.subscribeLeader(followerID, 500)
    traci.vehicle.subscribe(followerID, [tc.VAR_SPEED])
    traci.vehicle.subscribe(leaderID, [tc.VAR_SPEED])

    gapControlActive = False
    targetGapEstablished = False
    remainingTime = duration
    testCompleted = False
    prevLeader = ""
    leaderArrived = False
    while not testCompleted:
        traci.simulationStep()
        results = traci.vehicle.getSubscriptionResults(followerID)
        if refVehID == "":
            leader = results[tc.VAR_LEADER][0]
            leaderDist = results[tc.VAR_LEADER][1]
        elif not leaderArrived:
            leader = refVehID
            arrived = traci.simulation.getArrivedIDList()
            # print ("arrived vehicles: %s"%(str(arrived)))
            if leader in arrived:
                print("Reference vehicle has been removed.")
                leaderArrived = True
                targetGapEstablished = True
                gapControlActive = False
            else:
                traci.vehicle.subscribe(leaderID, [tc.VAR_SPEED])
                leaderEdgeID = traci.vehicle.getRoadID(leader)
                leaderPos = traci.vehicle.getLanePosition(leader)
                leaderDist = traci.vehicle.getDrivingDistance(followerID, leaderEdgeID, leaderPos)
        followerSpeed = results[tc.VAR_SPEED]
        leaderSpeed = traci.vehicle.getSubscriptionResults(leaderID)[tc.VAR_SPEED]
        currentTimeHeadway = leaderDist/followerSpeed if followerSpeed > 0 else 10000
        currentTime = traci.simulation.getTime()
        print("Time %s: Gap 'follower'->'%s' = %.3f (headway=%.3f)" %
              (currentTime, leader, leaderDist, currentTimeHeadway))
        print("'follower' speed = %s" % followerSpeed)
        sys.stdout.flush()
        if (not gapControlActive and not targetGapEstablished and currentTime > 50.):
            print("## Starting to open gap.")
            sys.stdout.flush()
            print("(followerID, targetTimeHeadway, targetSpaceHeadway, duration, changeRate) = %s" %
                  str((followerID, targetTimeHeadway, targetSpaceHeadway, duration, changeRate)))
            if refVehID == "":
                if maxDecel == -1:
                    traci.vehicle.openGap(followerID, targetTimeHeadway, targetSpaceHeadway, duration, changeRate)
                else:
                    print("maxDecel = %s" % maxDecel)
                    traci.vehicle.openGap(followerID, targetTimeHeadway, targetSpaceHeadway, duration, changeRate,
                                          maxDecel)
            else:
                print("(maxDecel, refVehID) = %s" % str((maxDecel, refVehID)))
                traci.vehicle.openGap(followerID, targetTimeHeadway, targetSpaceHeadway, duration, changeRate,
                                      maxDecel, refVehID)
                if maxDecel == -1:
                    # For test 'reference_vehicle_removed' set new route for merger
                    traci.vehicle.setRouteID('merger', 'route_merger')

            gapControlActive = True
        elif gapControlActive:
            print("Current/target headway: {0:.3f}/{1}".format(currentTimeHeadway, targetTimeHeadway))
            currentSpacing = currentTimeHeadway * followerSpeed
            targetSpacing = targetTimeHeadway * followerSpeed
            minSpacing = max(targetSpacing, targetSpaceHeadway) - POSITIONAL_EPS
            if not targetGapEstablished and (leader == "" or currentSpacing > minSpacing or prevLeader != leader):
                if (leader != "" and prevLeader != leader):
                    traci.vehicle.deactivateGapControl(followerID)
                    print("## Deactivating gap control (leader has changed).")
                    remainingTime = 0.0
                else:
                    print("## Target Headway attained.")
                targetGapEstablished = True
        if targetGapEstablished:
            remainingTime -= dt
            print("Remaining: %s" % max(0.0, remainingTime))
            if gapControlActive and remainingTime < 0.:
                gapControlActive = False
                print("## Gap control expired.")
            if remainingTime <= -extraTime:
                testCompleted = True
        if leaderSpeed == 0:
            # Let leader go on slowly, to test space gap
            traci.vehicle.setSpeed(leaderID, 1.)
        prevLeader = leader
        step += 1

    traci.close()
    sys.stdout.flush()


sys.stdout.flush()
targetTimeHeadway = float(sys.argv[2])
targetSpaceHeadway = float(sys.argv[3])
duration = float(sys.argv[4])
changeRate = float(sys.argv[5])
if (len(sys.argv) > 6):
    maxDecel = float(sys.argv[6])
    if (len(sys.argv) > 7):
        refVehID = sys.argv[7]
    else:
        refVehID = ""
else:
    maxDecel = -1
    refVehID = ""
runSingle(targetTimeHeadway, targetSpaceHeadway, duration, changeRate, maxDecel, refVehID)
