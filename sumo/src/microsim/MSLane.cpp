/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Christoph Sommer
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include "MSAbstractLaneChangeModel.h"
#include "MSNet.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSEdgeControl.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include "MSVehicleControl.h"
#include "MSInsertionControl.h"
#include "MSVehicleControl.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/HelpersHarmonoise.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
MSLane::DictType MSLane::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
MSLane::MSLane(const std::string& id, SUMOReal maxSpeed, SUMOReal length, MSEdge* const edge,
               unsigned int numericalID, const PositionVector& shape, SUMOReal width,
               SVCPermissions permissions) :
    Named(id),
    myShape(shape), myNumericalID(numericalID),
    myVehicles(), myLength(length), myWidth(width), myEdge(edge), myMaxSpeed(maxSpeed),
    myPermissions(permissions),
    myLogicalPredecessorLane(0),
    myBruttoVehicleLengthSum(0), myNettoVehicleLengthSum(0), myInlappingVehicleEnd(10000), myInlappingVehicle(0),
    myLengthGeometryFactor(myShape.length() / myLength) {}


MSLane::~MSLane() {
    for (MSLinkCont::iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        delete *i;
    }
}


void
MSLane::addLink(MSLink* link) {
    myLinks.push_back(link);
}


// ------ interaction with MSMoveReminder ------
void
MSLane::addMoveReminder(MSMoveReminder* rem) {
    myMoveReminders.push_back(rem);
    for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end(); ++veh) {
        (*veh)->addReminder(rem);
    }
}



// ------ Vehicle emission ------
void
MSLane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed, const MSLane::VehCont::iterator& at, MSMoveReminder::Notification notification) {
    assert(pos <= myLength);
    bool wasInactive = myVehicles.size() == 0;
    veh->enterLaneAtInsertion(this, pos, speed, notification);
    if (at == myVehicles.end()) {
        // vehicle will be the first on the lane
        myVehicles.push_back(veh);
    } else {
        myVehicles.insert(at, veh);
    }
    myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum += veh->getVehicleType().getLength();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
}


bool
MSLane::pWagGenericInsertion(MSVehicle& veh, SUMOReal mspeed, SUMOReal maxPos, SUMOReal minPos) {
    SUMOReal xIn = maxPos;
    SUMOReal vIn = mspeed;
    SUMOReal leaderDecel;
    veh.getBestLanes(true, this);
    if (myVehicles.size() != 0) {
        MSVehicle* leader = myVehicles.front();
        xIn = leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap();
        vIn = leader->getSpeed();
        leaderDecel = leader->getCarFollowModel().getMaxDecel();
    } else {
        SUMOReal brakeGap = veh.getCarFollowModel().brakeGap(mspeed);
        std::pair<MSVehicle* const, SUMOReal> leader = getLeaderOnConsecutive(brakeGap, 0, mspeed, veh, veh.getBestLanesContinuation(this));
        if (leader.first != 0) {
            xIn = getLength() + leader.second;
            vIn = leader.first->getSpeed();
            leaderDecel = leader.first->getCarFollowModel().getMaxDecel();
        } else {
            incorporateVehicle(&veh, maxPos, mspeed, myVehicles.end());
            return true;
        }
    }
    const SUMOReal vHlp = 0.5 * (vIn + mspeed);
    SUMOReal x2 = xIn;// have seen leader length already - skCar::lCar;
    SUMOReal x1 = x2 - 100.0;
    SUMOReal x = 0;
    for (int i = 0; i <= 10; i++) {
        x = 0.5 * (x1 + x2);
        SUMOReal vSafe = veh.getCarFollowModel().followSpeed(&veh, vHlp, xIn - x, vIn, leaderDecel);
        if (vSafe < vHlp) {
            x2 = x;
        } else {
            x1 = x;
        }
    }
    if (x < minPos) {
        return false;
    } else if (x > maxPos) {
        x = maxPos;
    }
    incorporateVehicle(&veh, x, vHlp, myVehicles.begin());
    return true;
}


bool
MSLane::pWagSimpleInsertion(MSVehicle& veh, SUMOReal mspeed, SUMOReal maxPos, SUMOReal minPos) {
    SUMOReal xIn = maxPos;
    SUMOReal vIn = mspeed;
    veh.getBestLanes(true, this);
    if (myVehicles.size() != 0) {
        MSVehicle* leader = myVehicles.front();
        xIn = leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap();
        vIn = leader->getSpeed();
    } else {
        SUMOReal brakeGap = veh.getCarFollowModel().brakeGap(mspeed);
        std::pair<MSVehicle* const, SUMOReal> leader = getLeaderOnConsecutive(brakeGap, 0, mspeed, veh, veh.getBestLanesContinuation(this));
        if (leader.first != 0) {
            xIn = getLength() + leader.second;
            vIn = leader.first->getSpeed();
        } else {
            incorporateVehicle(&veh, maxPos, mspeed, myVehicles.end());
            return true;
        }
    }
    const SUMOReal vHlp = 0.5 * (mspeed + vIn);
    xIn = xIn - vHlp * veh.getCarFollowModel().getHeadwayTime() - veh.getVehicleType().getMinGap();
    if (xIn < minPos) {
        return false;
    } else if (xIn > maxPos) {
        xIn = maxPos;
    }
    incorporateVehicle(&veh, xIn, vHlp, myVehicles.begin());
    return true;
}


bool
MSLane::maxSpeedGapInsertion(MSVehicle& veh, SUMOReal mspeed) {
    if (myVehicles.size() == 0) {
        return isInsertionSuccess(&veh, mspeed, myLength / 2, true);
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    SUMOReal maxSpeed = 0;
    SUMOReal maxPos = 0;
    MSLane::VehCont::iterator maxIt = myVehicles.begin();
    while (predIt != myVehicles.end()) {
        // get leader (may be zero) and follower
        const MSVehicle* leader = predIt != myVehicles.end() - 1 ? *(predIt + 1) : getPartialOccupator();
        const MSVehicle* follower = *predIt;
        SUMOReal leaderRearPos = getLength();
        SUMOReal leaderSpeed = mspeed;
        if (leader != 0) {
            leaderRearPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
            if (leader == getPartialOccupator()) {
                leaderRearPos = getPartialOccupatorEnd();
            }
            leaderSpeed = leader->getSpeed();
        }
        const SUMOReal nettoGap = leaderRearPos - follower->getPositionOnLane() - veh.getVehicleType().getLengthWithGap();
        if (nettoGap > 0) {
            const SUMOReal tau = veh.getCarFollowModel().getHeadwayTime();
            const SUMOReal tauDecel = tau * veh.getCarFollowModel().getMaxDecel();
            const SUMOReal fSpeed = follower->getSpeed();
            const SUMOReal lhs = nettoGap / tau + tauDecel - fSpeed - fSpeed * fSpeed / (2 * tauDecel) + leaderSpeed * leaderSpeed / (2 * tauDecel);
            if (lhs >= sqrt(tauDecel * tauDecel + leaderSpeed * leaderSpeed)) {
                const SUMOReal frontGap = (lhs * lhs - tauDecel * tauDecel - leaderSpeed * leaderSpeed) / (2 * veh.getCarFollowModel().getMaxDecel());
                const SUMOReal currentMaxSpeed = lhs - tauDecel;
                if (MIN2(currentMaxSpeed, mspeed) > maxSpeed) {
                    maxSpeed = currentMaxSpeed;
                    maxPos = leaderRearPos + frontGap;
                    maxIt = predIt + 1;
                }
            }
        }
        ++predIt;
    }
    if (maxSpeed > 0) {
        incorporateVehicle(&veh, maxPos, maxSpeed, maxIt);
        return true;
    }
    return false;
}


bool
MSLane::freeInsertion(MSVehicle& veh, SUMOReal mspeed,
                      MSMoveReminder::Notification notification) {
    bool adaptableSpeed = true;
    if (myVehicles.size() == 0) {
        if (isInsertionSuccess(&veh, mspeed, 0, adaptableSpeed, notification)) {
            return true;
        }
    } else {
        // check whether the vehicle can be put behind the last one if there is such
        MSVehicle* leader = myVehicles.back();
        SUMOReal leaderPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
        SUMOReal speed = mspeed;
        if (adaptableSpeed) {
            speed = leader->getSpeed();
        }
        SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
        if (leaderPos - frontGapNeeded >= 0) {
            SUMOReal tspeed = MIN2(veh.getCarFollowModel().followSpeed(&veh, mspeed, frontGapNeeded, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()), mspeed);
            // check whether we can insert our vehicle behind the last vehicle on the lane
            if (isInsertionSuccess(&veh, tspeed, 0, adaptableSpeed, notification)) {
                return true;
            }
        }
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    while (predIt != myVehicles.end()) {
        // get leader (may be zero) and follower
        const MSVehicle* leader = predIt != myVehicles.end() - 1 ? *(predIt + 1) : getPartialOccupator();
        const MSVehicle* follower = *predIt;

        // patch speed if allowed
        SUMOReal speed = mspeed;
        if (adaptableSpeed && leader != 0) {
            speed = MIN2(leader->getSpeed(), mspeed);
        }

        // compute the space needed to not collide with leader
        SUMOReal frontMax = getLength();
        if (leader != 0) {
            SUMOReal leaderRearPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
            if (leader == getPartialOccupator()) {
                leaderRearPos = getPartialOccupatorEnd();
            }
            SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
            frontMax = leaderRearPos - frontGapNeeded;
        }
        // compute the space needed to not let the follower collide
        const SUMOReal followPos = follower->getPositionOnLane() + follower->getVehicleType().getMinGap();
        const SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), veh.getSpeed(), veh.getCarFollowModel().getMaxDecel());
        const SUMOReal backMin = followPos + backGapNeeded + veh.getVehicleType().getLength();

        // check whether there is enough room (given some extra space for rounding errors)
        if (frontMax > 0 && backMin + POSITION_EPS < frontMax) {
            // try to insert vehicle (should be always ok)
            if (isInsertionSuccess(&veh, speed, backMin + POSITION_EPS, adaptableSpeed, notification)) {
                return true;
            }
        }
        ++predIt;
    }
    // first check at lane's begin
    return false;
}


bool
MSLane::insertVehicle(MSVehicle& veh) {
    SUMOReal pos = 0;
    SUMOReal speed = 0;
    bool patchSpeed = true; // whether the speed shall be adapted to infrastructure/traffic in front

    // determine the speed
    const SUMOVehicleParameter& pars = veh.getParameter();
    switch (pars.departSpeedProcedure) {
        case DEPART_SPEED_GIVEN:
            speed = pars.departSpeed;
            patchSpeed = false;
            break;
        case DEPART_SPEED_RANDOM:
            speed = RandHelper::rand(MIN2(veh.getMaxSpeed(), getVehicleMaxSpeed(&veh)));
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_MAX:
            speed = MIN2(veh.getMaxSpeed(), getVehicleMaxSpeed(&veh));
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            // speed = 0 was set before
            patchSpeed = false; // @todo check
            break;
    }

    // determine the position
    switch (pars.departPosProcedure) {
        case DEPART_POS_GIVEN:
            pos = pars.departPos;
            if (pos < 0.) {
                pos += myLength;
            }
            break;
        case DEPART_POS_RANDOM:
            pos = RandHelper::rand(getLength());
            break;
        case DEPART_POS_RANDOM_FREE: {
            for (unsigned int i = 0; i < 10; i++) {
                // we will try some random positions ...
                pos = RandHelper::rand(getLength());
                if (isInsertionSuccess(&veh, speed, pos, patchSpeed)) {
                    return true;
                }
            }
            // ... and if that doesn't work, we put the vehicle to the free position
            return freeInsertion(veh, speed);
        }
        break;
        case DEPART_POS_FREE:
            return freeInsertion(veh, speed);
        case DEPART_POS_PWAG_SIMPLE:
            return pWagSimpleInsertion(veh, speed, getLength(), 0.0);
        case DEPART_POS_PWAG_GENERIC:
            return pWagGenericInsertion(veh, speed, getLength(), 0.0);
        case DEPART_POS_MAX_SPEED_GAP:
            return maxSpeedGapInsertion(veh, speed);
        case DEPART_POS_BASE:
        case DEPART_POS_DEFAULT:
        default:
            pos = MIN2(static_cast<SUMOReal>(veh.getVehicleType().getLength() + POSITION_EPS), myLength);
            break;
    }
    // try to insert
    return isInsertionSuccess(&veh, speed, pos, patchSpeed);
}


bool
MSLane::checkFailure(MSVehicle* aVehicle, SUMOReal& speed, SUMOReal& dist, const SUMOReal nspeed, const bool patchSpeed, const std::string errorMsg) const {
    if (nspeed < speed) {
        if (patchSpeed) {
            speed = MIN2(nspeed, speed);
            dist = aVehicle->getCarFollowModel().brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
        } else {
            if (errorMsg != "") {
                WRITE_ERROR("Vehicle '" + aVehicle->getID() + "' will not be able to depart using the given velocity (" + errorMsg + ")!");
                MSNet::getInstance()->getInsertionControl().descheduleDeparture(aVehicle);
            }
            return true;
        }
    }
    return false;
}


bool
MSLane::isInsertionSuccess(MSVehicle* aVehicle,
                           SUMOReal speed, SUMOReal pos, bool patchSpeed,
                           MSMoveReminder::Notification notification) {
    if (pos < 0 || pos > myLength) {
        // we may not start there
        WRITE_WARNING("Invalid departPos " + toString(pos) + " given for vehicle '" +
                      aVehicle->getID() + "'. Inserting at lane end instead.");
        pos = myLength;
    }
    aVehicle->getBestLanes(true, this);
    const MSCFModel& cfModel = aVehicle->getCarFollowModel();
    const std::vector<MSLane*>& bestLaneConts = aVehicle->getBestLanesContinuation(this);
    std::vector<MSLane*>::const_iterator ri = bestLaneConts.begin();
    SUMOReal seen = getLength() - pos;
    SUMOReal dist = cfModel.brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
    const MSRoute& r = aVehicle->getRoute();
    MSRouteIterator ce = r.begin();
    unsigned int nRouteSuccs = 1;
    MSLane* currentLane = this;
    MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / speed);
    while (seen < dist && ri != bestLaneConts.end()) {
        // get the next link used...
        MSLinkCont::const_iterator link = currentLane->succLinkSec(*aVehicle, nRouteSuccs, *currentLane, bestLaneConts);
        if (currentLane->isLinkEnd(link)) {
            if (&currentLane->getEdge() == r.getLastEdge()) {
                // reached the end of the route
                if (aVehicle->getParameter().arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                    if (checkFailure(aVehicle, speed, dist, cfModel.freeSpeed(aVehicle, speed, seen, aVehicle->getParameter().arrivalSpeed),
                                     patchSpeed, "arrival speed too low")) {
                        // we may not drive with the given velocity - we cannot match the specified arrival speed
                        return false;
                    }
                }
            } else {
                // lane does not continue
                if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, seen),
                                 patchSpeed, "junction too close")) {
                    // we may not drive with the given velocity - we cannot stop at the junction
                    return false;
                }
            }
            break;
        }
        if (!(*link)->opened(arrivalTime, speed, speed, aVehicle->getVehicleType().getLength(), aVehicle->getImpatience(), cfModel.getMaxDecel(), 0)
                || !(*link)->havePriority()) {
            // have to stop at junction
            std::string errorMsg = "";
            const LinkState state = (*link)->getState();
            if (state == LINKSTATE_MINOR
                    || state == LINKSTATE_EQUAL
                    || state == LINKSTATE_STOP
                    || state == LINKSTATE_ALLWAY_STOP) {
                // no sense in trying later
                errorMsg = "unpriorised junction too close";
            }
            if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, seen),
                             patchSpeed, errorMsg)) {
                // we may not drive with the given velocity - we cannot stop at the junction in time
                return false;
            }
            break;
        }
        // get the next used lane (including internal)
        nextLane = (*link)->getViaLaneOrLane();
        // check how next lane effects the journey
        if (nextLane != 0) {
            arrivalTime += TIME2STEPS(nextLane->getLength() / speed);
            SUMOReal gap = 0;
            MSVehicle* leader = currentLane->getPartialOccupator();
            if (leader != 0) {
                gap = seen + currentLane->getPartialOccupatorEnd() - currentLane->getLength() - aVehicle->getVehicleType().getMinGap();
            } else {
                // check leader on next lane
                leader = nextLane->getLastVehicle();
                if (leader != 0) {
                    gap = seen + leader->getPositionOnLane() - leader->getVehicleType().getLength() -  aVehicle->getVehicleType().getMinGap();
                }
            }
            if (leader != 0) {
                if (gap < 0) {
                    return false;
                }
                const SUMOReal nspeed = cfModel.followSpeed(aVehicle, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                if (checkFailure(aVehicle, speed, dist, nspeed, patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash into the leader
                    return false;
                }
            }
            // check next lane's maximum velocity
            const SUMOReal nspeed = nextLane->getVehicleMaxSpeed(aVehicle); 
            if (nspeed < speed) {
                if (patchSpeed) {
                    speed = MIN2(cfModel.freeSpeed(aVehicle, speed, seen, nspeed), speed);
                    dist = cfModel.brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
                } else {
                    // we may not drive with the given velocity - we would be too fast on the next lane 
                    WRITE_ERROR("Vehicle '" + aVehicle->getID() + "' will not be able to depart using the given velocity (slow lane ahead)!");
                    MSNet::getInstance()->getInsertionControl().descheduleDeparture(aVehicle);
                    return false;
                }
            }
            // check traffic on next junction
            // we cannot use (*link)->opened because a vehicle without priority
            // may already be comitted to blocking the link and unable to stop
            const SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / speed);
            const SUMOTime leaveTime = arrivalTime + TIME2STEPS((*link)->getLength() * speed);
            if ((*link)->hasApproachingFoe(arrivalTime, leaveTime, speed, cfModel.getMaxDecel())) {
                if (checkFailure(aVehicle, speed, dist, cfModel.followSpeed(aVehicle, speed, seen, 0, 0),
                                 patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash at the junction
                    return false;
                }
            }
            seen += nextLane->getLength();
            currentLane = nextLane;
#ifdef HAVE_INTERNAL_LANES
            if ((*link)->getViaLane() == 0) {
#else
            if (true) {
#endif
                nRouteSuccs++;
                ++ce;
                ++ri;
            }
        }
    }

    // get the pointer to the vehicle next in front of the given position
    MSLane::VehCont::iterator predIt = find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos));
    if (predIt != myVehicles.end()) {
        // ok, there is one (a leader)
        MSVehicle* leader = *predIt;
        SUMOReal frontGapNeeded = cfModel.getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
        SUMOReal gap = MSVehicle::gap(leader->getPositionOnLane(), leader->getVehicleType().getLength(), pos + aVehicle->getVehicleType().getMinGap());
        if (gap < frontGapNeeded) {
            // too close to the leader on this lane
            return false;
        }
    }

    // check back vehicle
    if (predIt != myVehicles.begin()) {
        // there is direct follower on this lane
        MSVehicle* follower = *(predIt - 1);
        SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), aVehicle->getSpeed(), cfModel.getMaxDecel());
        SUMOReal gap = MSVehicle::gap(pos, aVehicle->getVehicleType().getLength(), follower->getPositionOnLane() + follower->getVehicleType().getMinGap());
        if (gap < backGapNeeded) {
            // too close to the follower on this lane
            return false;
        }
    } else {
        // check approaching vehicles to prevent rear-end collisions
        // to compute an uper bound on the look-back distance we need
        // the chosenSpeedFactor, minGap and maxDeceleration of approaching vehicles
        // since we do not know these we use the values from the vehicle to be inserted
        // and add a safety factor
        const SUMOReal dist = 2 * (aVehicle->getCarFollowModel().brakeGap(myMaxSpeed) + aVehicle->getVehicleType().getMinGap());
        const SUMOReal backOffset = pos - aVehicle->getVehicleType().getLength();
        const SUMOReal missingRearGap = getMissingRearGap(dist, backOffset, speed, aVehicle->getCarFollowModel().getMaxDecel());
        if (missingRearGap > 0) {
            // too close to a follower
            const SUMOReal neededStartPos = pos + missingRearGap;
            if (myVehicles.size() == 0 && notification == MSMoveReminder::NOTIFICATION_TELEPORT && neededStartPos <= myLength) {
                // shift starting positiong as needed entering from teleport
                pos = neededStartPos;
            } else {
                return false;
            }
        }
    }
    // may got negative while adaptation
    if (speed < 0) {
        return false;
    }
    // enter
    incorporateVehicle(aVehicle, pos, speed, predIt, notification);
    return true;
}


void
MSLane::forceVehicleInsertion(MSVehicle* veh, SUMOReal pos) {
    veh->getBestLanes(true, this);
    incorporateVehicle(veh, pos, veh->getSpeed(), find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos)));
}


// ------ Handling vehicles lapping into lanes ------
SUMOReal
MSLane::setPartialOccupation(MSVehicle* v, SUMOReal leftVehicleLength) {
    myInlappingVehicle = v;
    if (leftVehicleLength > myLength) {
        myInlappingVehicleEnd = 0;
    } else {
        myInlappingVehicleEnd = myLength - leftVehicleLength;
    }
    return myLength;
}


void
MSLane::resetPartialOccupation(MSVehicle* v) {
    if (v == myInlappingVehicle) {
        myInlappingVehicleEnd = 10000;
    }
    myInlappingVehicle = 0;
}


std::pair<MSVehicle*, SUMOReal>
MSLane::getLastVehicleInformation() const {
    if (myVehicles.size() != 0) {
        // the last vehicle is the one in scheduled by this lane
        MSVehicle* last = *myVehicles.begin();
        const SUMOReal pos = last->getPositionOnLane() - last->getVehicleType().getLength();
        return std::make_pair(last, pos);
    }
    if (myInlappingVehicle != 0) {
        // the last one is a vehicle extending into this lane
        return std::make_pair(myInlappingVehicle, myInlappingVehicleEnd);
    }
    return std::make_pair<MSVehicle*, SUMOReal>(0, 0);
}


// ------  ------
void
MSLane::planMovements(SUMOTime t) {
    assert(myVehicles.size() != 0);
    SUMOReal cumulatedVehLength = 0.;
    const MSVehicle* pred = getPartialOccupator();
    for (VehCont::reverse_iterator veh = myVehicles.rbegin(); veh != myVehicles.rend(); ++veh) {
        if ((*veh)->getLane() == this) {
            (*veh)->planMove(t, pred, cumulatedVehLength);
        }
        pred = *veh;
        cumulatedVehLength += pred->getVehicleType().getLengthWithGap();
    }
}


void
MSLane::detectCollisions(SUMOTime timestep, int stage) {
    if (myVehicles.size() < 2) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin(); veh != lastVeh;) {
        VehCont::iterator pred = veh + 1;
        if ((*veh)->hasInfluencer() && (*veh)->getInfluencer().isVTDControlled()) {
            ++veh;
            continue;
        }
        if ((*pred)->hasInfluencer() && (*pred)->getInfluencer().isVTDControlled()) {
            ++veh;
            continue;
        }
        SUMOReal gap = (*pred)->getPositionOnLane() - (*pred)->getVehicleType().getLength() - (*veh)->getPositionOnLane() - (*veh)->getVehicleType().getMinGap();
        if (gap < -0.001) {
            MSVehicle* vehV = *veh;
            if (vehV->getLane() == this) {
                WRITE_WARNING("Teleporting vehicle '" + vehV->getID() + "'; collision with '"
                              + (*pred)->getID() + "', lane='" + getID() + "', gap=" + toString(gap)
                              + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " stage=" + toString(stage) + ".");
                MSNet::getInstance()->getVehicleControl().registerCollision();
                myBruttoVehicleLengthSum -= vehV->getVehicleType().getLengthWithGap();
                myNettoVehicleLengthSum -= vehV->getVehicleType().getLength();
                MSVehicleTransfer::getInstance()->addVeh(timestep, vehV);
                veh = myVehicles.erase(veh); // remove current vehicle
                lastVeh = myVehicles.end() - 1;
                if (veh == myVehicles.end()) {
                    break;
                }
            } else {
                WRITE_WARNING("Shadow of vehicle '" + vehV->getID() + "'; collision with '"
                              + (*pred)->getID() + "', lane='" + getID() + "', gap=" + toString(gap)
                              + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + " stage=" + toString(stage) + ".");
                veh = myVehicles.erase(veh); // remove current vehicle
                lastVeh = myVehicles.end() - 1;
                vehV->getLaneChangeModel().endLaneChangeManeuver();
                if (veh == myVehicles.end()) {
                    break;
                }
            }
        } else {
            ++veh;
        }
    }
}


bool
MSLane::executeMovements(SUMOTime t, std::vector<MSLane*>& into) {
    // iteratate over vehicles in reverse so that move reminders will be called in the correct order
    for (VehCont::reverse_iterator i = myVehicles.rbegin(); i != myVehicles.rend();) {
        MSVehicle* veh = *i;
        if (veh->getLane() != this || veh->getLaneChangeModel().alreadyMoved()) {
            // this is the shadow during a continuous lane change
            ++i;
            continue;
        }
        // length is needed later when the vehicle may not exist anymore
        const SUMOReal length = veh->getVehicleType().getLengthWithGap();
        const SUMOReal nettoLength = veh->getVehicleType().getLength();
        bool moved = veh->executeMove();
        MSLane* target = veh->getLane();
#ifndef NO_TRACI
        bool vtdControlled = veh->hasInfluencer() && veh->getInfluencer().isVTDControlled();
        if (veh->hasArrived() && !vtdControlled) {
#else
        if (veh->hasArrived()) {
#endif
            // vehicle has reached its arrival position
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        } else if (target != 0 && moved) {
            if (target->getEdge().isVaporizing()) {
                // vehicle has reached a vaporizing edge
                veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED);
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            } else {
                // vehicle has entered a new lane (leaveLane was already called in MSVehicle::executeMove)
                target->myVehBuffer.push_back(veh);
                SUMOReal pspeed = veh->getSpeed();
                SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
                veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
                into.push_back(target);
                if (veh->getLaneChangeModel().isChangingLanes()) {
                    MSLane* shadowLane = veh->getLaneChangeModel().getShadowLane();
                    if (shadowLane != 0) {
                        into.push_back(shadowLane);
                        shadowLane->myVehBuffer.push_back(veh);
                    }
                }
            }
        } else if (veh->isParking()) {
            // vehicle started to park
            veh->leaveLane(MSMoveReminder::NOTIFICATION_JUNCTION);
            MSVehicleTransfer::getInstance()->addVeh(t, veh);
        } else if (veh->getPositionOnLane() > getLength()) {
            // for any reasons the vehicle is beyond its lane... error
            WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; beyond lane (2), targetLane='" + getID() + "', time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSNet::getInstance()->getVehicleControl().registerTeleport();
            MSVehicleTransfer::getInstance()->addVeh(t, veh);
        } else {
            ++i;
            continue;
        }
        myBruttoVehicleLengthSum -= length;
        myNettoVehicleLengthSum -= nettoLength;
        ++i;
        i = VehCont::reverse_iterator(myVehicles.erase(i.base())); 
    }
    if (myVehicles.size() > 0) {
        if (MSGlobals::gTimeToGridlock > 0 || MSGlobals::gTimeToGridlockHighways > 0) {
            MSVehicle* last = myVehicles.back();
            bool r1 = MSGlobals::gTimeToGridlock > 0 && !last->isStopped() && last->getWaitingTime() > MSGlobals::gTimeToGridlock;
            bool r2 = MSGlobals::gTimeToGridlockHighways > 0 && !last->isStopped() && last->getWaitingTime() > MSGlobals::gTimeToGridlockHighways && last->getLane()->getSpeedLimit() > 69. / 3.6 && !last->getLane()->appropriate(last);
            if (r1 || r2) {
                MSVehicle* veh = *(myVehicles.end() - 1);
                myBruttoVehicleLengthSum -= veh->getVehicleType().getLengthWithGap();
                myNettoVehicleLengthSum -= veh->getVehicleType().getLength();
                myVehicles.erase(myVehicles.end() - 1);
                WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; waited too long"
                              + (r2 ? " on highway" : "")
                              + ", lane='" + getID() + "', time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                MSNet::getInstance()->getVehicleControl().registerTeleport();
                MSVehicleTransfer::getInstance()->addVeh(t, veh);
            }
        }
    }
    return myVehicles.size() == 0;
}



// ------ Static (sic!) container methods  ------
bool
MSLane::dictionary(const std::string &id, MSLane* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSLane*
MSLane::dictionary(const std::string &id) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSLane::clear() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSLane::insertIDs(std::vector<std::string>& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


void 
MSLane::fill(NamedRTree& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        MSLane *l = (*i).second;
        Boundary b = l->getShape().getBoxBoundary();
        b.grow(3.);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        into.Insert(cmin, cmax, l);
    }
}



// ------   ------
bool
MSLane::appropriate(const MSVehicle* veh) {
    if (myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
    return (link != myLinks.end());
}


bool
MSLane::integrateNewVehicle(SUMOTime) {
    bool wasInactive = myVehicles.size() == 0;
    sort(myVehBuffer.begin(), myVehBuffer.end(), vehicle_position_sorter());
    for (std::vector<MSVehicle*>::const_iterator i = myVehBuffer.begin(); i != myVehBuffer.end(); ++i) {
        MSVehicle* veh = *i;
        myVehicles.insert(myVehicles.begin(), veh);
        myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
        myNettoVehicleLengthSum += veh->getVehicleType().getLength();
    }
    myVehBuffer.clear();
    return wasInactive && myVehicles.size() != 0;
}


bool
MSLane::isLinkEnd(MSLinkCont::const_iterator& i) const {
    return i == myLinks.end();
}


bool
MSLane::isLinkEnd(MSLinkCont::iterator& i) {
    return i == myLinks.end();
}


MSVehicle*
MSLane::getLastVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return *myVehicles.begin();
}


const MSVehicle*
MSLane::getFirstVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return *(myVehicles.end() - 1);
}


MSLinkCont::const_iterator
MSLane::succLinkSec(const SUMOVehicle& veh, unsigned int nRouteSuccs,
                    const MSLane& succLinkSource, const std::vector<MSLane*>& conts) const {
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge == 0) {
        // return end (no succeeding link) if so
        return succLinkSource.myLinks.end();
    }
    // if we are on an internal lane there should only be one link and it must be allowed
    if (succLinkSource.getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        assert(succLinkSource.myLinks.size() == 1);
        assert(succLinkSource.myLinks[0]->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass()));
        return succLinkSource.myLinks.begin();
    }
    // a link may be used if
    //  1) there is a destination lane ((*link)->getLane()!=0)
    //  2) the destination lane belongs to the next edge in route ((*link)->getLane()->myEdge == nRouteEdge)
    //  3) the destination lane allows the vehicle's class ((*link)->getLane()->allowsVehicleClass(veh.getVehicleClass()))

    // there should be a link which leads to the next desired lane our route in "conts" (built in "getBestLanes")
    // "conts" stores the best continuations of our current lane
    // we should never return an arbitrary link since this may cause collisions
    MSLinkCont::const_iterator link;
    if (nRouteSuccs < conts.size()) {
        // we go through the links in our list and return the matching one
        for (link = succLinkSource.myLinks.begin(); link != succLinkSource.myLinks.end(); ++link) {
            if ((*link)->getLane() != 0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
                // we should use the link if it connects us to the best lane
                if ((*link)->getLane() == conts[nRouteSuccs]) {
                    return link;
                }
            }
        }
    } else {
        // the source lane is a dead end (no continuations exist)
        return succLinkSource.myLinks.end();
    }
    // the only case where this should happen is for a disconnected route (deliberately ignored)
#ifdef _DEBUG
    WRITE_WARNING("Could not find connection between '" + succLinkSource.getID() + "' and '" + conts[nRouteSuccs]->getID() + 
            "' for vehicle '" + veh.getID() + "' time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");   
#endif
    return succLinkSource.myLinks.end();
}



const MSLinkCont&
MSLane::getLinkCont() const {
    return myLinks;
}


void
MSLane::swapAfterLaneChange(SUMOTime) {
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
}


MSVehicle*
MSLane::removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification) {
    for (MSLane::VehCont::iterator it = myVehicles.begin(); it < myVehicles.end(); it++) {
        if (remVehicle == *it) {
            remVehicle->leaveLane(notification);
            myVehicles.erase(it);
            myBruttoVehicleLengthSum -= remVehicle->getVehicleType().getLengthWithGap();
            myNettoVehicleLengthSum -= remVehicle->getVehicleType().getLength();
            break;
        }
    }
    return remVehicle;
}


MSLane*
MSLane::getParallelLane(int offset) const {
    return myEdge->parallelLane(this, offset);
}


void
MSLane::addIncomingLane(MSLane* lane, MSLink* viaLink) {
    IncomingLaneInfo ili;
    ili.lane = lane;
    ili.viaLink = viaLink;
    ili.length = lane->getLength();
    myIncomingLanes.push_back(ili);
}


void
MSLane::addApproachingLane(MSLane* lane) {
    MSEdge* approachingEdge = &lane->getEdge();
    if (myApproachingLanes.find(approachingEdge) == myApproachingLanes.end()) {
        myApproachingLanes[approachingEdge] = std::vector<MSLane*>();
    }
    myApproachingLanes[approachingEdge].push_back(lane);
}


bool
MSLane::isApproachedFrom(MSEdge* const edge) {
    return myApproachingLanes.find(edge) != myApproachingLanes.end();
}


bool
MSLane::isApproachedFrom(MSEdge* const edge, MSLane* const lane) {
    std::map<MSEdge*, std::vector<MSLane*> >::const_iterator i = myApproachingLanes.find(edge);
    if (i == myApproachingLanes.end()) {
        return false;
    }
    const std::vector<MSLane*>& lanes = (*i).second;
    return find(lanes.begin(), lanes.end(), lane) != lanes.end();
}


class by_second_sorter {
public:
    inline int operator()(const std::pair<const MSVehicle* , SUMOReal>& p1, const std::pair<const MSVehicle* , SUMOReal>& p2) const {
        return p1.second < p2.second;
    }
};


SUMOReal MSLane::getMissingRearGap(
    SUMOReal dist, SUMOReal backOffset, SUMOReal leaderSpeed, SUMOReal leaderMaxDecel) const {
    // this follows the same logic as getFollowerOnConsecutive. we do a tree
    // search until dist and check for the vehicle with the largest missing rear gap
    SUMOReal result = 0;
    std::set<MSLane*> visited;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size() != 0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i = toExamine.begin(); i != toExamine.end(); ++i) {
            MSLane* next = (*i).lane;
            if (next->getFirstVehicle() != 0) {
                MSVehicle* v = (MSVehicle*) next->getFirstVehicle();
                const SUMOReal agap = (*i).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                const SUMOReal missingRearGap = v->getCarFollowModel().getSecureGap(
                                                    v->getCarFollowModel().maxNextSpeed(v->getSpeed(), v), leaderSpeed, leaderMaxDecel) - agap;
                result = MAX2(result, missingRearGap);
            } else {
                if ((*i).length < dist) {
                    const std::vector<MSLane::IncomingLaneInfo>& followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j = followers.begin(); j != followers.end(); ++j) {
                        if (visited.find((*j).lane) == visited.end()) {
                            visited.insert((*j).lane);
                            MSLane::IncomingLaneInfo ili;
                            ili.lane = (*j).lane;
                            ili.length = (*j).length + (*i).length;
                            ili.viaLink = (*j).viaLink;
                            newFound.push_back(ili);
                        }
                    }
                }
            }
        }
        toExamine.clear();
        swap(newFound, toExamine);
    }
    return result;
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getFollowerOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal leaderSpeed,
                                 SUMOReal backOffset, SUMOReal predMaxDecel) const {
    // ok, a vehicle has not noticed the lane about itself;
    //  iterate as long as necessary to search for an approaching one
    std::set<MSLane*> visited;
    std::vector<std::pair<MSVehicle*, SUMOReal> > possible;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size() != 0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i = toExamine.begin(); i != toExamine.end(); ++i) {
            /*
            if ((*i).viaLink->getState()==LINKSTATE_TL_RED) {
                continue;
            }
            */
            MSLane* next = (*i).lane;
            if (next->getFirstVehicle() != 0) {
                MSVehicle* v = (MSVehicle*) next->getFirstVehicle();
                SUMOReal agap = (*i).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                if (agap <= v->getCarFollowModel().getSecureGap(v->getCarFollowModel().maxNextSpeed(v->getSpeed(), v), leaderSpeed, predMaxDecel)) {
                    possible.push_back(std::make_pair(v, agap));
                }
            } else {
                if ((*i).length + seen < dist) {
                    const std::vector<MSLane::IncomingLaneInfo>& followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j = followers.begin(); j != followers.end(); ++j) {
                        if (visited.find((*j).lane) == visited.end()) {
                            visited.insert((*j).lane);
                            MSLane::IncomingLaneInfo ili;
                            ili.lane = (*j).lane;
                            ili.length = (*j).length + (*i).length;
                            ili.viaLink = (*j).viaLink;
                            newFound.push_back(ili);
                        }
                    }
                }
            }
        }
        toExamine.clear();
        swap(newFound, toExamine);
    }
    if (possible.size() == 0) {
        return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
    }
    sort(possible.begin(), possible.end(), by_second_sorter());
    return *(possible.begin());
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle& veh,
                               const std::vector<MSLane*>& bestLaneConts) const {
    if (seen > dist) {
        return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
    }
    unsigned int view = 1;
    // loop over following lanes
    const MSLane* targetLane = this;
    MSVehicle* leader = targetLane->getPartialOccupator();
    if (leader != 0) {
        return std::pair<MSVehicle* const, SUMOReal>(leader, seen - targetLane->getPartialOccupatorEnd() - veh.getVehicleType().getMinGap());
    }
    const MSLane* nextLane = targetLane;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / speed);
    do {
        // get the next link used
        MSLinkCont::const_iterator link = targetLane->succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, veh.getVehicleType().getLength(),
                veh.getImpatience(), veh.getCarFollowModel().getMaxDecel(), 0) || (*link)->getState() == LINKSTATE_TL_RED) {
            break;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(seen, veh.getVehicleType().getMinGap());
        if (linkLeaders.size() > 0) {
            // XXX if there is more than one link leader we should return the most important
            // one (gap, decel) but this is hard to know at this point
            return linkLeaders[0];
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }
        arrivalTime += TIME2STEPS(nextLane->getLength() / speed);
        MSVehicle* leader = nextLane->getLastVehicle();
        if (leader != 0) {
            return std::pair<MSVehicle* const, SUMOReal>(leader, seen + leader->getPositionOnLane() - leader->getVehicleType().getLength() - veh.getVehicleType().getMinGap());
        } else {
            leader = nextLane->getPartialOccupator();
            if (leader != 0) {
                return std::pair<MSVehicle* const, SUMOReal>(leader, seen + nextLane->getPartialOccupatorEnd() - veh.getVehicleType().getMinGap());
            }
        }
        if (nextLane->getVehicleMaxSpeed(&veh) < speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(&veh));
        }
        seen += nextLane->getLength();
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    } while (seen <= dist);
    return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
}


MSLane*
MSLane::getLogicalPredecessorLane() const {
    if (myLogicalPredecessorLane != 0) {
        return myLogicalPredecessorLane;
    }
    if (myLogicalPredecessorLane == 0) {
        std::vector<MSEdge*> pred = myEdge->getIncomingEdges();
        // get only those edges which connect to this lane
        for (std::vector<MSEdge*>::iterator i = pred.begin(); i != pred.end();) {
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(*i));
            if (j == myIncomingLanes.end()) {
                i = pred.erase(i);
            } else {
                ++i;
            }
        }
        // get the edge with the most connections to this lane's edge
        if (pred.size() != 0) {
            std::sort(pred.begin(), pred.end(), by_connections_to_sorter(&getEdge()));
            MSEdge* best = *pred.begin();
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(best));
            myLogicalPredecessorLane = (*j).lane;
        }
    }
    return myLogicalPredecessorLane;
}


void
MSLane::leftByLaneChange(MSVehicle* v) {
    myBruttoVehicleLengthSum -= v->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum -= v->getVehicleType().getLength();
}


void
MSLane::enteredByLaneChange(MSVehicle* v) {
    myBruttoVehicleLengthSum += v->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum += v->getVehicleType().getLength();
}


// ------------ Current state retrieval
SUMOReal
MSLane::getBruttoOccupancy() const {
    SUMOReal fractions = myInlappingVehicle!=0 ? myLength - myInlappingVehicleEnd : 0;
    getVehiclesSecure();
    if(myVehicles.size()!=0) {
        MSVehicle *lastVeh = myVehicles.front();
        if(lastVeh->getPositionOnLane()<lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength()-lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myBruttoVehicleLengthSum+fractions) / myLength;
}


SUMOReal
MSLane::getNettoOccupancy() const {
    SUMOReal fractions = myInlappingVehicle!=0 ? myLength - myInlappingVehicleEnd : 0;
    getVehiclesSecure();
    if(myVehicles.size()!=0) {
        MSVehicle *lastVeh = myVehicles.front();
        if(lastVeh->getPositionOnLane()<lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength()-lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myBruttoVehicleLengthSum+fractions) / myLength;
}


SUMOReal
MSLane::getBruttoVehLenSum() const {
    return myBruttoVehicleLengthSum;
}


SUMOReal
MSLane::getWaitingSeconds() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    SUMOReal wtime = 0;
    for (VehCont::const_iterator i = myVehicles.begin(); i != myVehicles.end(); ++i) {
		wtime += (*i)->getWaitingSeconds();
    }
    return wtime;
}


SUMOReal
MSLane::getMeanSpeed() const {
    if (myVehicles.size() == 0) {
        return myMaxSpeed;
    }
    SUMOReal v = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        v += (*i)->getSpeed();
    }
    SUMOReal ret = v / (SUMOReal) myVehicles.size();
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_CO2Emissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_CO2Emissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_COEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_COEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_PMxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_PMxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_NOxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_NOxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_HCEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_HCEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_FuelConsumption() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHBEFA_FuelConsumption();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHarmonoise_NoiseEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    if (vehs.size() == 0) {
        releaseVehicles();
        return 0;
    }
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        SUMOReal sv = (*i)->getHarmonoise_NoiseEmissions();
        ret += (SUMOReal) pow(10., (sv / 10.));
    }
    releaseVehicles();
    return HelpersHarmonoise::sum(ret);
}


bool
MSLane::VehPosition::operator()(const MSVehicle* cmp, SUMOReal pos) const {
    return cmp->getPositionOnLane() >= pos;
}


int
MSLane::vehicle_position_sorter::operator()(MSVehicle* v1, MSVehicle* v2) const {
    return v1->getPositionOnLane() > v2->getPositionOnLane();
}

MSLane::by_connections_to_sorter::by_connections_to_sorter(const MSEdge* const e) :
    myEdge(e),
    myLaneDir(e->getLanes()[0]->getShape().getBegLine().atan2PositiveAngle())
{ }


int
MSLane::by_connections_to_sorter::operator()(const MSEdge* const e1, const MSEdge* const e2) const {
    const std::vector<MSLane*>* ae1 = e1->allowedLanes(*myEdge);
    const std::vector<MSLane*>* ae2 = e2->allowedLanes(*myEdge);
    SUMOReal s1 = 0;
    if (ae1 != 0 && ae1->size() != 0) {
        s1 = (SUMOReal) ae1->size() + GeomHelper::getMinAngleDiff((*ae1)[0]->getShape().getBegLine().atan2PositiveAngle(), myLaneDir) / M_PI / 2.;
    }
    SUMOReal s2 = 0;
    if (ae2 != 0 && ae2->size() != 0) {
        s2 = (SUMOReal) ae2->size() + GeomHelper::getMinAngleDiff((*ae2)[0]->getShape().getBegLine().atan2PositiveAngle(), myLaneDir) / M_PI / 2.;
    }
    return s1 < s2;
}


void
MSLane::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_LANE);
    out.openTag(SUMO_TAG_VIEWSETTINGS_VEHICLES);
    out.writeAttr(SUMO_ATTR_VALUE, myVehicles);
    out.closeTag();
    out.closeTag();
}


void
MSLane::loadState(std::vector<std::string>& vehIds, MSVehicleControl& vc) {
    for (std::vector<std::string>::const_iterator it = vehIds.begin(); it != vehIds.end(); ++it) {
        MSVehicle* v = dynamic_cast<MSVehicle*>(vc.getVehicle(*it));
        assert(v != 0);
        v->getBestLanes(true, this);
        incorporateVehicle(v, v->getPositionOnLane(), v->getSpeed(), myVehicles.end(),
                           MSMoveReminder::NOTIFICATION_JUNCTION);
    }
}



/****************************************************************************/

