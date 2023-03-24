/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSLCHelper.cpp
/// @author  Jakob Erdmann
/// @date    Fri, 19.06.2020
///
// Common functions for lane change models
/****************************************************************************/

#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSLCHelper.h"

// ===========================================================================
// Debug flags
// ===========================================================================
//#define DEBUG_WANTS_CHANGE
//#define DEBUG_SAVE_BLOCKER_LENGTH

#define DEBUG_COND (veh.isSelected())
//#define DEBUG_COND (true)


// ===========================================================================
// member method definitions
// ===========================================================================

double
MSLCHelper::getRoundaboutDistBonus(const MSVehicle& veh,
                                   double bonusParam,
                                   const MSVehicle::LaneQ& curr,
                                   const MSVehicle::LaneQ& neigh,
                                   const MSVehicle::LaneQ& best) {
    if (veh.getLaneChangeModel().isOpposite()) {
        return 0;
    }
    const MSVehicle::LaneQ& inner = neigh.lane->getIndex() > curr.lane->getIndex() ? neigh : curr;
#ifdef DEBUG_WANTS_CHANGE
    const bool debugVehicle = veh.getLaneChangeModel().debugVehicle();
    if (debugVehicle) {
        std::cout << SIMTIME << " veh=" << veh.getID() << " getRoundaboutDistBonus bonusParam=" << bonusParam
                  << " curr=" << curr.lane->getID()
                  << " neigh=" << neigh.lane->getID()
                  << " inner=" << inner.lane->getID()
                  << " best=" << best.lane->getID()
                  << "\n   innerCont=" << toString(inner.bestContinuations)
                  << "\n   bestCont=" << toString(best.bestContinuations)
                  << "\n";
    }
#endif

    int roundaboutJunctionsAhead = 0;
    bool enteredRoundabout = false;
    double seen = -veh.getPositionOnLane();

    // first check using only normal lanes
    for (int i = 0; i < (int)best.bestContinuations.size(); i++) {
        const MSLane* lane = best.bestContinuations[i];
        if (lane == nullptr) {
            lane = veh.getLane();
        }
        if ((!enteredRoundabout || lane->getEdge().isRoundabout()) && i >= (int)inner.bestContinuations.size()) {
            // no bonus if we cannot continue on the inner lane until leaving the roundabout
#ifdef DEBUG_WANTS_CHANGE
            if (debugVehicle) {
                std::cout << "   noBonus: inner does not continue (lane=" << lane->getID() << ")\n";
            }
#endif
            return 0;
        }
        if (seen > 300) {
            // avoid long look-ahead
#ifdef DEBUG_WANTS_CHANGE
            if (debugVehicle) {
                std::cout << "   noBonus: seen=" << seen << " (lane=" << lane->getID() << ")\n";
            }
#endif
            return 0;
        }
        const MSJunction* junction = lane->getEdge().getToJunction();
        if (lane->getEdge().isRoundabout()) {
            enteredRoundabout = true;
            if (junction->getIncoming().size() + junction->getOutgoing().size() > 2) {
                roundaboutJunctionsAhead++;
            }
        } else if (enteredRoundabout) {
            // only check the first roundabout
            break;
        }
        seen += lane->getLength();
    }
    // no bonus if we want to take the next exit
    if (roundaboutJunctionsAhead < 2) {
        return 0;
    }

    // compute bonus value based on jamming and exact distances (taking into
    // account internal lanes)
    double occupancyOuter = 0;
    double occupancyInner = 0;
    double distanceInRoundabout = 0;
    MSLane* prevNormal = nullptr;
    MSLane* prevInner = nullptr;
    enteredRoundabout = false;
    for (int i = 0; i < (int)best.bestContinuations.size(); i++) {
        MSLane* lane = best.bestContinuations[i];
        if (lane == nullptr) {
            continue;
        }
        if (lane->getEdge().isRoundabout()) {
            enteredRoundabout = true;
        } else if (enteredRoundabout) {
            // only check the first roundabout
            break;
        }
        MSLane* via = nullptr;
        if (prevNormal != nullptr) {
            for (MSLink* link : prevNormal->getLinkCont()) {
                if (link->getLane() == lane) {
                    via = link->getViaLane();
                }
            }
        }
        if (enteredRoundabout) {
            distanceInRoundabout += lane->getLength();
            if (via != nullptr) {
                distanceInRoundabout += via->getLength();
            }
        }
        // discount vehicles that are upstream from ego
        const double upstreamDiscount = &lane->getEdge() == &veh.getLane()->getEdge()
                                        ? (lane->getLength() - veh.getPositionOnLane()) / lane->getLength() : 1;
        prevNormal = lane;
        occupancyOuter += upstreamDiscount * lane->getBruttoVehLenSum();
#ifdef DEBUG_WANTS_CHANGE
        if (debugVehicle) {
            std::cout << " lane=" << lane->getID() << " occ=" << lane->getBruttoVehLenSum() << " discount=" << upstreamDiscount << " outer=" << occupancyOuter << "\n";
        }
#endif
        if (via != nullptr) {
            occupancyOuter += via->getBruttoVehLenSum();
#ifdef DEBUG_WANTS_CHANGE
            if (debugVehicle) {
                std::cout << " via=" << via->getID() << " occ=" << via->getBruttoVehLenSum() << " outer=" << occupancyOuter << "\n";
            }
#endif
        }
        if (i < (int)inner.bestContinuations.size()) {
            MSLane* innerLane = inner.bestContinuations[i];
            occupancyInner += upstreamDiscount * innerLane->getBruttoVehLenSum();
#ifdef DEBUG_WANTS_CHANGE
            if (debugVehicle) {
                std::cout << " inner=" << innerLane->getID() << " occ=" << innerLane->getBruttoVehLenSum() << " discount=" << upstreamDiscount << " inner=" << occupancyInner << "\n";
            }
#endif
            if (prevInner != nullptr) {
                for (MSLink* link : prevInner->getLinkCont()) {
                    if (link->getLane() == innerLane && link->getViaLane() != nullptr) {
                        occupancyInner += link->getViaLane()->getBruttoVehLenSum();
#ifdef DEBUG_WANTS_CHANGE
                        if (debugVehicle) {
                            std::cout << " innerVia=" << link->getViaLane()->getID() << " occ=" << link->getViaLane()->getBruttoVehLenSum() << " inner=" << occupancyInner << "\n";
                        }
#endif
                    }
                }
            }
            prevInner = innerLane;
        }
    }

#ifdef DEBUG_WANTS_CHANGE
    if (debugVehicle) {
        std::cout << "   distanceInRoundabout=" << distanceInRoundabout
                  << " roundaboutJunctionsAhead=" << roundaboutJunctionsAhead
                  << " occupancyInner=" << occupancyInner
                  << " occupancyOuter=" << occupancyOuter
                  << "\n";
    }
#endif

    const double maxOccupancy = MAX2(occupancyInner, occupancyOuter);
    if (maxOccupancy == 0) {
        // no bonues if the roundabout is empty
        return 0;
    }
    // give some bonus for using the inside lane at equal occupancy
    const double bonus = roundaboutJunctionsAhead * 7.5;
    const double relativeJam = (occupancyOuter - occupancyInner + bonus) / (maxOccupancy + bonus);
    // no bonus if the inner lane or the left lane entering the roundabout is jammed
    const double jamFactor = MAX2(0.0, relativeJam);
    const double result = distanceInRoundabout * jamFactor * bonusParam * 9; // the 9 is abitrary and only there for backward compatibility
#ifdef DEBUG_WANTS_CHANGE
    if (debugVehicle) {
        std::cout << "   relativeJam=" << relativeJam
                  << " jamFactor=" << jamFactor
                  << " distanceBonus=" << result
                  << "\n";
    }
#endif
    return result;
}


bool
MSLCHelper::saveBlockerLength(const MSVehicle& veh,  MSVehicle* blocker, int lcaCounter, double leftSpace, bool reliefConnection, double& leadingBlockerLength) {
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " veh=" << veh.getID()
                  << " saveBlockerLength blocker=" << Named::getIDSecure(blocker)
                  << " bState=" << (blocker == 0 ? "None" : toString((LaneChangeAction)blocker->getLaneChangeModel().getOwnState()))
                  << "\n";
    }
#endif
    if (blocker != nullptr && (blocker->getLaneChangeModel().getOwnState() & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const double potential = leftSpace - veh.getCarFollowModel().brakeGap(
                                     veh.getSpeed(), veh.getCarFollowModel().getMaxDecel(), 0);
        if (blocker->getVehicleType().getLengthWithGap() <= potential) {
            // save at least his length in myLeadingBlockerLength
            leadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), leadingBlockerLength);
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " veh=" << veh.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " saving myLeadingBlockerLength=" << leadingBlockerLength
                          << "\n";
            }
#endif
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
            const bool canReserve = blocker->getLaneChangeModel().saveBlockerLength(veh.getVehicleType().getLengthWithGap(), leftSpace);
            //reliefConnection ? std::numeric_limits<double>::max() : leftSpace);
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " veh=" << veh.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " cannot save space=" << blocker->getVehicleType().getLengthWithGap()
                          << " potential=" << potential
                          << " myReserved=" << leadingBlockerLength
                          << " canReserve=" << canReserve
                          << " reliefConnection=" << reliefConnection
                          << "\n";
            }
#endif
            if (!canReserve && !reliefConnection) {
                // reserve anyway and try to avoid deadlock with emergency deceleration
                leadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), leadingBlockerLength);
            }
            return canReserve;
        }
    }
    return true;
}


bool
MSLCHelper::canSaveBlockerLength(const MSVehicle& veh, double requested, double leftSpace) {
    const double potential = leftSpace - veh.getCarFollowModel().brakeGap(veh.getSpeed(), veh.getCarFollowModel().getMaxDecel(), veh.getActionStepLengthSecs());
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
    if (DEBUG_COND) {
        std::cout << SIMTIME << " canSaveBlockerLength veh=" << veh.getID() << " requested=" << requested << " leftSpace=" << leftSpace << " potential=" << potential << "\n";
    }
#endif
    return potential >= requested;
}


bool
MSLCHelper::divergentRoute(const MSVehicle& v1, const MSVehicle& v2) {
    // a sufficient, but not necessary condition for divergence
    return (v1.getLane()->isInternal() && v2.getLane()->isInternal()
            && v1.getLane()->getEdge().getFromJunction() == v2.getLane()->getEdge().getFromJunction()
            && &v1.getLane()->getEdge() != &v2.getLane()->getEdge());
}


double
MSLCHelper::getSpeedPreservingSecureGap(const MSVehicle& leader, const MSVehicle& follower, double currentGap, double leaderPlannedSpeed) {
    // whatever speed the follower choses in the next step, it will change both
    // the secureGap and the required followSpeed.
    // Let's assume the leader maintains speed
    const double nextGap = currentGap + SPEED2DIST(leaderPlannedSpeed - follower.getSpeed());
    double sGap = follower.getCarFollowModel().getSecureGap(&follower, &leader, follower.getSpeed(), leaderPlannedSpeed, leader.getCarFollowModel().getMaxDecel());
    if (nextGap >= sGap) {
        // follower may still accelerate
        const double nextGapMin = currentGap + SPEED2DIST(leaderPlannedSpeed - follower.getCarFollowModel().maxNextSpeed(follower.getSpeed(), &follower));
        const double vSafe = follower.getCarFollowModel().followSpeed(
                                 &follower, follower.getSpeed(), nextGapMin, leaderPlannedSpeed, leader.getCarFollowModel().getMaxDecel());
        return MAX2(vSafe, follower.getSpeed());
    } else {
        // follower must brake. The following brakes conservatively since the actual gap will be lower due to braking.
        const double vSafe = follower.getCarFollowModel().followSpeed(
                                 &follower, follower.getSpeed(), nextGap, leaderPlannedSpeed, leader.getCarFollowModel().getMaxDecel());
        // avoid emergency deceleration
        return MAX2(vSafe, follower.getCarFollowModel().minNextSpeed(follower.getSpeed(), &follower));
    }
}

/****************************************************************************/
