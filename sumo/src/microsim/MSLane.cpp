/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Christoph Sommer
/// @author  Mario Krumnow
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/pedestrians/MSPModel.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
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
#include "MSLeaderInfo.h"
#include "MSVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_INSERTION
//#define DEBUG_PLAN_MOVE
//#define DEBUG_CONTEXT
//#define DEBUG_OPPOSITE
//#define DEBUG_VEHICLE_CONTAINER
//#define DEBUG_COLLISIONS
#define DEBUG_COND (getID() == "disabled")
#define DEBUG_COND2(obj) ((obj != 0 && (obj)->getID() == "disabled"))

// ===========================================================================
// static member definitions
// ===========================================================================
MSLane::DictType MSLane::myDict;
MSLane::CollisionAction MSLane::myCollisionAction(MSLane::COLLISION_ACTION_TELEPORT);
bool MSLane::myCheckJunctionCollisions(false);

// ===========================================================================
// internal class method definitions
// ===========================================================================


MSLane::AnyVehicleIterator&
MSLane::AnyVehicleIterator::operator++() {
    if (nextIsMyVehicles()) {
        if (myI1 != myI1End) {
            myI1 += myDirection;
        }
        // else: already at end
    } else {
        myI2 += myDirection;
    }
    //if (DEBUG_COND2(myLane)) std::cout << SIMTIME << "          AnyVehicleIterator::operator++ lane=" << myLane->getID() << " myI1=" << myI1 << " myI2=" << myI2 << "\n";
    return *this;
}


const MSVehicle*
MSLane::AnyVehicleIterator::operator*() {
    if (nextIsMyVehicles()) {
        if (myI1 != myI1End) {
            return myLane->myVehicles[myI1];
        } else {
            return 0;
        }
    } else {
        return myLane->myPartialVehicles[myI2];
    }
}


bool
MSLane::AnyVehicleIterator::nextIsMyVehicles() const {
    //if (DEBUG_COND2(myLane)) std::cout << SIMTIME << "          AnyVehicleIterator::nextIsMyVehicles lane=" << myLane->getID()
    //        << " myI1=" << myI1
    //        << " myI2=" << myI2
    //        << "\n";
    if (myI1 == myI1End) {
        if (myI2 != myI2End) {
            return false;
        } else {
            return true; // @note. must be caught
        }
    } else {
        if (myI2 == myI2End) {
            return true;
        } else {
            //if (DEBUG_COND2(myLane)) std::cout << "              "
            //        << " veh1=" << myLane->myVehicles[myI1]->getID()
            //        << " veh2=" << myLane->myPartialVehicles[myI2]->getID()
            //        << " pos1=" << myLane->myVehicles[myI1]->getPositionOnLane(myLane)
            //        << " pos2=" << myLane->myPartialVehicles[myI2]->getPositionOnLane(myLane)
            //        << "\n";
            if (myLane->myVehicles[myI1]->getPositionOnLane(myLane) < myLane->myPartialVehicles[myI2]->getPositionOnLane(myLane)) {
                return myDownstream;
            } else {
                return !myDownstream;
            }
        }
    }
}


// ===========================================================================
// member method definitions
// ===========================================================================
MSLane::MSLane(const std::string& id, SUMOReal maxSpeed, SUMOReal length, MSEdge* const edge,
               int numericalID, const PositionVector& shape, SUMOReal width,
               SVCPermissions permissions, int index) :
    Named(id),
    myNumericalID(numericalID), myShape(shape), myIndex(index),
    myVehicles(), myLength(length), myWidth(width), myEdge(edge), myMaxSpeed(maxSpeed),
    myPermissions(permissions),
    myOriginalPermissions(permissions),
    myLogicalPredecessorLane(0),
    myBruttoVehicleLengthSum(0), myNettoVehicleLengthSum(0),
    myLeaderInfo(this, 0, 0),
    myFollowerInfo(this, 0, 0),
    myLeaderInfoTmp(this, 0, 0),
    myLeaderInfoTime(SUMOTime_MIN),
    myFollowerInfoTime(SUMOTime_MIN),
    myLengthGeometryFactor(MAX2(POSITION_EPS, myShape.length()) / myLength), // factor should not be 0
    myRightSideOnEdge(0), // initialized in MSEdge::initialize
    myRightmostSublane(0) { // initialized in MSEdge::initialize
    myRestrictions = MSNet::getInstance()->getRestrictions(edge->getEdgeType());
}


MSLane::~MSLane() {
    for (MSLinkCont::iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        delete *i;
    }
}


void
MSLane::addLink(MSLink* link) {
    myLinks.push_back(link);
}


void
MSLane::addNeigh(const std::string& id) {
    myNeighs.push_back(id);
}


// ------ interaction with MSMoveReminder ------
void
MSLane::addMoveReminder(MSMoveReminder* rem) {
    myMoveReminders.push_back(rem);
    for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end(); ++veh) {
        (*veh)->addReminder(rem);
    }
}


SUMOReal
MSLane::setPartialOccupation(MSVehicle* v) {
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(v)) {
        std::cout << SIMTIME << " setPartialOccupation. lane=" << getID() << " veh=" << v->getID() << "\n";
    }
#endif
    // XXX update occupancy here?
    myPartialVehicles.push_back(v);
    return myLength;
}


void
MSLane::resetPartialOccupation(MSVehicle* v) {
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(v)) {
        std::cout << SIMTIME << " resetPartialOccupation. lane=" << getID() << " veh=" << v->getID() << "\n";
    }
#endif
    for (VehCont::iterator i = myPartialVehicles.begin(); i != myPartialVehicles.end(); ++i) {
        if (v == *i) {
            myPartialVehicles.erase(i);
            // XXX update occupancy here?
            //std::cout << "    removed from myPartialVehicles\n";
            return;
        }
    }
    assert(false);
}


// ------ Vehicle emission ------
void
MSLane::incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed, SUMOReal posLat, const MSLane::VehCont::iterator& at, MSMoveReminder::Notification notification) {
    assert(pos <= myLength);
    bool wasInactive = myVehicles.size() == 0;
    veh->enterLaneAtInsertion(this, pos, speed, posLat, notification);
    if (at == myVehicles.end()) {
        // vehicle will be the first on the lane
        myVehicles.push_back(veh);
    } else {
        myVehicles.insert(at, veh);
    }
    myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
    myNettoVehicleLengthSum += veh->getVehicleType().getLength();
    myEdge->markDelayed();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
}


bool
MSLane::lastInsertion(MSVehicle& veh, SUMOReal mspeed, bool patchSpeed) {
    // XXX interpret departPosLat value
    const SUMOReal posLat = 0;
    SUMOReal pos = getLength() - POSITION_EPS;
    MSVehicle* leader = getLastAnyVehicle();
    // back position of leader relative to this lane
    SUMOReal leaderBack;
    if (leader == 0) {
        /// look for a leaders on consecutive lanes
        veh.setTentativeLaneAndPosition(this, pos, posLat);
        veh.updateBestLanes(false, this);
        std::pair<MSVehicle* const, SUMOReal> leaderInfo = getLeader(&veh, pos, veh.getBestLanesContinuation(), veh.getCarFollowModel().brakeGap(mspeed));
        leader = leaderInfo.first;
        leaderBack = pos + leaderInfo.second + veh.getVehicleType().getMinGap();
    } else {
        leaderBack = leader->getBackPositionOnLane(this);
        //std::cout << " leaderPos=" << leader->getPositionOnLane(this) << " leaderBack=" << leader->getBackPositionOnLane(this) << " leaderLane=" << leader->getLane()->getID() << "\n";
    }
    if (leader == 0) {
        // insert at the end of this lane
        return isInsertionSuccess(&veh, mspeed, pos, posLat, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED);
    } else {
        // try to insert behind the leader
        const SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(mspeed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap() + POSITION_EPS;
        if (leaderBack >= frontGapNeeded) {
            pos = MIN2(pos, leaderBack - frontGapNeeded);
            bool result = isInsertionSuccess(&veh, mspeed, pos, posLat, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED);
            //if (!result) std::cout << " insertLast failed for " << veh.getID() << " pos=" << pos << " leaderBack=" << leaderBack << " frontGapNeeded=" << frontGapNeeded << "\n";
            return result;
        }
        //std::cout << " insertLast failed for " << veh.getID() << " pos=" << pos << " leaderBack=" << leaderBack << " frontGapNeeded=" << frontGapNeeded << "\n";
    }
    return false;
}


bool
MSLane::freeInsertion(MSVehicle& veh, SUMOReal mspeed,
                      MSMoveReminder::Notification notification) {
    bool adaptableSpeed = true;
    // try to insert teleporting vehicles fully on this lane
    const SUMOReal minPos = (notification == MSMoveReminder::NOTIFICATION_TELEPORT ?
                             MIN2(myLength, veh.getVehicleType().getLength()) : 0);

    if (myVehicles.size() == 0) {
        // ensure sufficient gap to followers on predecessor lanes
        const SUMOReal backOffset = minPos - veh.getVehicleType().getLength();
        const SUMOReal missingRearGap = getMissingRearGap(backOffset, mspeed, veh.getCarFollowModel().getMaxDecel());
        if (missingRearGap > 0) {
            if (minPos + missingRearGap <= myLength) {
                // @note. The rear gap is tailored to mspeed. If it changes due
                // to a leader vehicle (on subsequent lanes) insertion will
                // still fail. Under the right combination of acceleration and
                // deceleration values there might be another insertion
                // positions that would be successful be we do not look for it.
                //std::cout << SIMTIME << " freeInsertion lane=" << getID() << " veh=" << veh.getID() << " unclear @(340)\n";
                return isInsertionSuccess(&veh, mspeed, minPos + missingRearGap, 0, adaptableSpeed, notification);
            } else {
                return false;
            }
        } else {
            return isInsertionSuccess(&veh, mspeed, minPos, 0, adaptableSpeed, notification);
        }

    } else {
        // check whether the vehicle can be put behind the last one if there is such
        MSVehicle* leader = getFirstFullVehicle(); // @todo reproduction of bogus old behavior. see #1961
        const SUMOReal leaderPos = leader->getBackPositionOnLane(this);
        const SUMOReal speed = adaptableSpeed ? leader->getSpeed() : mspeed;
        const SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
        if (leaderPos >= frontGapNeeded) {
            const SUMOReal tspeed = MIN2(veh.getCarFollowModel().insertionFollowSpeed(&veh, mspeed, frontGapNeeded, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()), mspeed);
            // check whether we can insert our vehicle behind the last vehicle on the lane
            if (isInsertionSuccess(&veh, tspeed, minPos, 0, adaptableSpeed, notification)) {
                //std::cout << SIMTIME << " freeInsertion lane=" << getID() << " veh=" << veh.getID() << " pos=" << minPos<< " speed=" << speed  << " tspeed=" << tspeed << " frontGapNeeded=" << frontGapNeeded << " lead=" << leader->getID() << " lPos=" << leaderPos << "\n   vehsOnLane=" << toString(myVehicles) << " @(358)\n";
                return true;
            }
        }
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    while (predIt != myVehicles.end()) {
        // get leader (may be zero) and follower
        // @todo compute secure position in regard to sublane-model
        const MSVehicle* leader = predIt != myVehicles.end() - 1 ? *(predIt + 1) : 0;
        if (leader == 0 && myPartialVehicles.size() > 0) {
            leader = myPartialVehicles.front();
        }
        const MSVehicle* follower = *predIt;

        // patch speed if allowed
        SUMOReal speed = mspeed;
        if (adaptableSpeed && leader != 0) {
            speed = MIN2(leader->getSpeed(), mspeed);
        }

        // compute the space needed to not collide with leader
        SUMOReal frontMax = getLength();
        if (leader != 0) {
            SUMOReal leaderRearPos = leader->getBackPositionOnLane(this);
            SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()) + veh.getVehicleType().getMinGap();
            frontMax = leaderRearPos - frontGapNeeded;
        }
        // compute the space needed to not let the follower collide
        const SUMOReal followPos = follower->getPositionOnLane() + follower->getVehicleType().getMinGap();
        const SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), veh.getSpeed(), veh.getCarFollowModel().getMaxDecel());
        const SUMOReal backMin = followPos + backGapNeeded + veh.getVehicleType().getLength();

        // check whether there is enough room (given some extra space for rounding errors)
        if (frontMax > minPos && backMin + POSITION_EPS < frontMax) {
            // try to insert vehicle (should be always ok)
            if (isInsertionSuccess(&veh, speed, backMin + POSITION_EPS, 0, adaptableSpeed, notification)) {
                //std::cout << SIMTIME << " freeInsertion lane=" << getID() << " veh=" << veh.getID() << " @(393)\n";
                return true;
            }
        }
        ++predIt;
    }
    // first check at lane's begin
    //std::cout << SIMTIME << " freeInsertion lane=" << getID() << " veh=" << veh.getID() << " fail final\n";
    return false;
}


SUMOReal
MSLane::getDepartSpeed(const MSVehicle& veh, bool& patchSpeed) {
    SUMOReal speed = 0;
    const SUMOVehicleParameter& pars = veh.getParameter();
    switch (pars.departSpeedProcedure) {
        case DEPART_SPEED_GIVEN:
            speed = pars.departSpeed;
            patchSpeed = false;
            break;
        case DEPART_SPEED_RANDOM:
            speed = RandHelper::rand(getVehicleMaxSpeed(&veh));
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_MAX:
            speed = getVehicleMaxSpeed(&veh);
            patchSpeed = true; // @todo check
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            // speed = 0 was set before
            patchSpeed = false; // @todo check
            break;
    }
    return speed;
}


bool
MSLane::insertVehicle(MSVehicle& veh) {
    SUMOReal pos = 0;
    SUMOReal posLat = 0;
    bool patchSpeed = true; // whether the speed shall be adapted to infrastructure/traffic in front
    const SUMOVehicleParameter& pars = veh.getParameter();
    SUMOReal speed = getDepartSpeed(veh, patchSpeed);

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
            for (int i = 0; i < 10; i++) {
                // we will try some random positions ...
                pos = RandHelper::rand(getLength());
                if (pars.departPosLatProcedure == DEPART_POSLAT_RANDOM ||
                        pars.departPosLatProcedure == DEPART_POSLAT_RANDOM_FREE) {
                    posLat = RandHelper::rand(getWidth() - veh.getVehicleType().getWidth()) - getWidth() * 0.5 + veh.getVehicleType().getWidth() * 0.5;
                }
                if (isInsertionSuccess(&veh, speed, pos, posLat, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED)) {
                    return true;
                }
            }
            // ... and if that doesn't work, we put the vehicle to the free position
            return freeInsertion(veh, speed);
        }
        break;
        case DEPART_POS_FREE:
            return freeInsertion(veh, speed);
        case DEPART_POS_LAST:
            return lastInsertion(veh, speed, patchSpeed);
        case DEPART_POS_BASE:
        case DEPART_POS_DEFAULT:
        default:
            pos = basePos(veh);
            break;
    }
    // determine the lateral position
    switch (pars.departPosLatProcedure) {
        case DEPART_POSLAT_GIVEN:
            posLat = pars.departPosLat;
            break;
        case DEPART_POSLAT_RANDOM:
            posLat = RandHelper::rand(getWidth() - veh.getVehicleType().getWidth()) - getWidth() * 0.5 + veh.getVehicleType().getWidth() * 0.5;
            break;
        case DEPART_POSLAT_RANDOM_FREE: {
            for (int i = 0; i < 10; i++) {
                // we will try some random positions ...
                posLat = RandHelper::rand(getWidth()) - getWidth() * 0.5;
                if (isInsertionSuccess(&veh, speed, pos, posLat, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED)) {
                    return true;
                }
            }
            // ... and if that doesn't work, we put the vehicle to the free position
            return freeInsertion(veh, speed);
        }
        break;
        case DEPART_POSLAT_FREE:
            return freeInsertion(veh, speed);
        case DEPART_POSLAT_RIGHT:
            posLat = -getWidth() * 0.5 + veh.getVehicleType().getWidth() * 0.5;
            break;
        case DEPART_POSLAT_LEFT:
            posLat = getWidth() * 0.5 - veh.getVehicleType().getWidth() * 0.5;
            break;
        case DEPART_POSLAT_CENTER:
        case DEPART_POS_DEFAULT:
        default:
            posLat = 0;
            break;
    }
    // try to insert
    return isInsertionSuccess(&veh, speed, pos, posLat, patchSpeed, MSMoveReminder::NOTIFICATION_DEPARTED);
}


SUMOReal
MSLane::basePos(const MSVehicle& veh) const {
    return MIN2(veh.getVehicleType().getLength() + POSITION_EPS, myLength);
}

bool
MSLane::checkFailure(MSVehicle* aVehicle, SUMOReal& speed, SUMOReal& dist, const SUMOReal nspeed, const bool patchSpeed, const std::string errorMsg) const {
    if (nspeed < speed) {
        if (patchSpeed) {
            speed = MIN2(nspeed, speed);
            dist = aVehicle->getCarFollowModel().brakeGap(speed) + aVehicle->getVehicleType().getMinGap();
        } else if (speed > 0) {
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
                           SUMOReal speed, SUMOReal pos, SUMOReal posLat, bool patchSpeed,
                           MSMoveReminder::Notification notification) {
    if (pos < 0 || pos > myLength) {
        // we may not start there
        WRITE_WARNING("Invalid departPos " + toString(pos) + " given for vehicle '" +
                      aVehicle->getID() + "'. Inserting at lane end instead.");
        pos = myLength;
    }

#ifdef DEBUG_INSERTION
    if (DEBUG_COND2(aVehicle)) std::cout << "\nIS_INSERTION_SUCCESS\n"
                                             << SIMTIME  << " lane=" << getID()
                                             << " veh '" << aVehicle->getID() << "'\n";
#endif

    aVehicle->setTentativeLaneAndPosition(this, pos, posLat);
    aVehicle->updateBestLanes(false, this);
    const MSCFModel& cfModel = aVehicle->getCarFollowModel();
    const std::vector<MSLane*>& bestLaneConts = aVehicle->getBestLanesContinuation(this);
    std::vector<MSLane*>::const_iterator ri = bestLaneConts.begin();
    SUMOReal seen = getLength() - pos; // == distance from insertion position until the end of the currentLane
    SUMOReal dist = cfModel.brakeGap(speed) + aVehicle->getVehicleType().getMinGap();

    // before looping through the continuation lanes, check if a stop is scheduled on this lane
    // (the code is duplicated in the loop)
    if (aVehicle->hasStops()) {
        const MSVehicle::Stop& nextStop = aVehicle->getNextStop();
        if (nextStop.lane == this) {
            std::stringstream msg;
            msg << "scheduled stop on lane '" << myID << "' too close";
            const SUMOReal distToStop = nextStop.endPos - pos; // XXX: Please approve whether endPos is appropriate, here. (Leo)
            if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, distToStop),
                             patchSpeed, msg.str())) {
                // we may not drive with the given velocity - we cannot stop at the stop
                return false;
            }
        }
    }

    const MSRoute& r = aVehicle->getRoute();
    MSRouteIterator ce = r.begin();
    int nRouteSuccs = 1;
    MSLane* currentLane = this;
    MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, SUMO_const_haltingSpeed));
    while (seen < dist && ri != bestLaneConts.end()) {
        // get the next link used...
        MSLinkCont::const_iterator link = succLinkSec(*aVehicle, nRouteSuccs, *currentLane, bestLaneConts);
        if (currentLane->isLinkEnd(link)) {
            if (&currentLane->getEdge() == r.getLastEdge()) {
                // reached the end of the route
                if (aVehicle->getParameter().arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                    if (checkFailure(aVehicle, speed, dist, cfModel.freeSpeed(aVehicle, speed, seen, aVehicle->getParameter().arrivalSpeed, true),
                                     patchSpeed, "arrival speed too low")) {
                        // we may not drive with the given velocity - we cannot match the specified arrival speed
                        return false;
                    }
                }
            } else {
                // lane does not continue
                if (checkFailure(aVehicle, speed, dist, cfModel.insertionStopSpeed(aVehicle, speed, seen),
                                 patchSpeed, "junction too close")) {
                    // we may not drive with the given velocity - we cannot stop at the junction
                    return false;
                }
            }
            break;
        }

        if (!(*link)->opened(arrivalTime, speed, speed, aVehicle->getVehicleType().getLength(), aVehicle->getImpatience(), cfModel.getMaxDecel(), 0, posLat)
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
            if (checkFailure(aVehicle, speed, dist, cfModel.insertionStopSpeed(aVehicle, speed, seen),
                             patchSpeed, errorMsg)) {
                // we may not drive with the given velocity - we cannot stop at the junction in time
                return false;
            }
#ifdef DEBUG_INSERTION
            if DEBUG_COND2(aVehicle) {
                std::cout << "trying insertion before minor link: "
                          << "insertion speed = " << speed << " dist=" << dist
                          << "\n";
            }
#endif
            break;
        }
        // get the next used lane (including internal)
        nextLane = (*link)->getViaLaneOrLane();
        // check how next lane affects the journey
        if (nextLane != 0) {

            // check if there are stops on the next lane that should be regarded
            // (this block is duplicated before the loop to deal with the insertion lane)
            if (aVehicle->hasStops()) {
                const MSVehicle::Stop& nextStop = aVehicle->getNextStop();
                if (nextStop.lane == nextLane) {
                    std::stringstream msg;
                    msg << "scheduled stop on lane '" << nextStop.lane->getID() << "' too close";
                    const SUMOReal distToStop = seen + nextStop.endPos;
                    if (checkFailure(aVehicle, speed, dist, cfModel.stopSpeed(aVehicle, speed, distToStop),
                                     patchSpeed, msg.str())) {
                        // we may not drive with the given velocity - we cannot stop at the stop
                        return false;
                    }
                }
            }

            // check leader on next lane
            // XXX check all leaders in the sublane case
            SUMOReal gap = 0;
            MSVehicle* leader = nextLane->getLastAnyVehicle();
            if (leader != 0) {
#ifdef DEBUG_INSERTION
                if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                         << "leader on lane '" << nextLane->getID() << "': " << leader->getID() << "\n";
#endif
                gap = seen + leader->getBackPositionOnLane(nextLane) -  aVehicle->getVehicleType().getMinGap();
            }
            if (leader != 0) {
                if (gap < 0) {
                    return false;
                }
                const SUMOReal nspeed = cfModel.insertionFollowSpeed(aVehicle, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                if (checkFailure(aVehicle, speed, dist, nspeed, patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash into the leader
#ifdef DEBUG_INSERTION
                    if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                             << " isInsertionSuccess lane=" << getID()
                                                             << " veh=" << aVehicle->getID()
                                                             << " pos=" << pos
                                                             << " posLat=" << posLat
                                                             << " patchSpeed=" << patchSpeed
                                                             << " speed=" << speed
                                                             << " nspeed=" << nspeed
                                                             << " nextLane=" << nextLane->getID()
                                                             << " lead=" << leader->getID()
                                                             << " gap=" << gap
                                                             << " failed (@641)!\n";
#endif
                    return false;
                }
            }
            // check next lane's maximum velocity
            const SUMOReal nspeed = cfModel.freeSpeed(aVehicle, speed, seen, nextLane->getVehicleMaxSpeed(aVehicle), true);
            if (nspeed < speed) {
                if (patchSpeed) {
                    speed = nspeed;
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
            const SUMOTime leaveTime = (*link)->getLeaveTime(arrivalTime, speed, speed, aVehicle->getVehicleType().getLength());
            if ((*link)->hasApproachingFoe(arrivalTime, leaveTime, speed, cfModel.getMaxDecel())) {
                if (checkFailure(aVehicle, speed, dist, cfModel.insertionStopSpeed(aVehicle, speed, seen), patchSpeed, "")) {
                    // we may not drive with the given velocity - we crash at the junction
                    return false;
                }
            }
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
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
    MSLeaderInfo leaders = getLastVehicleInformation(aVehicle, 0, pos);
    //if (aVehicle->getID() == "disabled") std::cout << " leaders=" << leaders.toString() << "\n";
    const SUMOReal nspeed = safeInsertionSpeed(aVehicle, leaders, speed);
    if (nspeed < 0 || checkFailure(aVehicle, speed, dist, nspeed, patchSpeed, "")) {
        // XXX: checking for nspeed<0... Might appear naturally with ballistic update (see #860, Leo)
        // TODO: check if ballistic update needs adjustments here, refs. #2577

        // we may not drive with the given velocity - we crash into the leader
#ifdef DEBUG_INSERTION
        if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                 << " isInsertionSuccess lane=" << getID()
                                                 << " veh=" << aVehicle->getID()
                                                 << " pos=" << pos
                                                 << " posLat=" << posLat
                                                 << " patchSpeed=" << patchSpeed
                                                 << " speed=" << speed
                                                 << " nspeed=" << nspeed
                                                 << " nextLane=" << nextLane->getID()
                                                 << " leaders=" << leaders.toString()
                                                 << " failed (@700)!\n";
#endif
        return false;
    }
#ifdef DEBUG_INSERTION
    if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                             << " speed = " << speed
                                             << " nspeed = " << nspeed
                                             << std::endl;
#endif

    MSLeaderDistanceInfo followers = getFollowersOnConsecutive(aVehicle, false);
    for (int i = 0; i < followers.numSublanes(); ++i) {
        const MSVehicle* follower = followers[i].first;
        if (follower != 0) {
            const SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), speed, cfModel.getMaxDecel());
            if (followers[i].second < backGapNeeded) {
                // too close to the follower on this lane
#ifdef DEBUG_INSERTION
                if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                         << " isInsertionSuccess lane=" << getID()
                                                         << " veh=" << aVehicle->getID()
                                                         << " pos=" << pos
                                                         << " posLat=" << posLat
                                                         << " patchSpeed=" << patchSpeed
                                                         << " speed=" << speed
                                                         << " nspeed=" << nspeed
                                                         << " follower=" << follower->getID()
                                                         << " backGapNeeded=" << backGapNeeded
                                                         << " gap=" << followers[i].second
                                                         << " failure (@719)!\n";
#endif
                return false;
            }
        }
    }
    if (followers.numFreeSublanes() > 0) {
        // check approaching vehicles to prevent rear-end collisions
        const SUMOReal backOffset = pos - aVehicle->getVehicleType().getLength();
        const SUMOReal missingRearGap = getMissingRearGap(backOffset, speed, aVehicle->getCarFollowModel().getMaxDecel());
        if (missingRearGap > 0) {
            // too close to a follower
#ifdef DEBUG_INSERTION
            if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                     << " isInsertionSuccess lane=" << getID()
                                                     << " veh=" << aVehicle->getID()
                                                     << " pos=" << pos
                                                     << " posLat=" << posLat
                                                     << " patchSpeed=" << patchSpeed
                                                     << " speed=" << speed
                                                     << " nspeed=" << nspeed
                                                     << " missingRearGap=" << missingRearGap
                                                     << " failure (@728)!\n";
#endif
            return false;
        }
    }
    // may got negative while adaptation
    if (speed < 0) {
#ifdef DEBUG_INSERTION
        if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                                 << " isInsertionSuccess lane=" << getID()
                                                 << " veh=" << aVehicle->getID()
                                                 << " pos=" << pos
                                                 << " posLat=" << posLat
                                                 << " patchSpeed=" << patchSpeed
                                                 << " speed=" << speed
                                                 << " nspeed=" << nspeed
                                                 << " failed (@733)!\n";
#endif
        return false;
    }
    // enter
    incorporateVehicle(aVehicle, pos, speed, posLat, find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos)), notification);
#ifdef DEBUG_INSERTION
    if (DEBUG_COND2(aVehicle)) std::cout << SIMTIME
                                             << " isInsertionSuccess lane=" << getID()
                                             << " veh=" << aVehicle->getID()
                                             << " pos=" << pos
                                             << " posLat=" << posLat
                                             << " patchSpeed=" << patchSpeed
                                             << " speed=" << speed
                                             << " nspeed=" << nspeed
                                             << " success!\n";
#endif
    return true;
}


void
MSLane::forceVehicleInsertion(MSVehicle* veh, SUMOReal pos, MSMoveReminder::Notification notification, SUMOReal posLat) {
    veh->updateBestLanes(true, this);
    bool dummy;
    const SUMOReal speed = veh->hasDeparted() ? veh->getSpeed() : getDepartSpeed(*veh, dummy);
    incorporateVehicle(veh, pos, speed, posLat, find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos)), notification);
}


SUMOReal
MSLane::safeInsertionSpeed(const MSVehicle* veh, const MSLeaderInfo& leaders, SUMOReal speed) {
    SUMOReal nspeed = speed;
    for (int i = 0; i < leaders.numSublanes(); ++i) {
        const MSVehicle* leader = leaders[i];
        if (leader != 0) {
            const SUMOReal gap = leader->getBackPositionOnLane(this) - veh->getPositionOnLane() - veh->getVehicleType().getMinGap();
            if (gap < 0) {
                return -1;
            }
            nspeed = MIN2(nspeed,
                          veh->getCarFollowModel().insertionFollowSpeed(veh, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()));
        }
    }
    return nspeed;
}


// ------ Handling vehicles lapping into lanes ------
const MSLeaderInfo&
MSLane::getLastVehicleInformation(const MSVehicle* ego, SUMOReal latOffset, SUMOReal minPos, bool allowCached) const {
    if (myLeaderInfoTime < MSNet::getInstance()->getCurrentTimeStep() || ego != 0 || minPos > 0 || !allowCached) {
        myLeaderInfoTmp = MSLeaderInfo(this, ego, latOffset);
        AnyVehicleIterator last = anyVehiclesBegin();
        int freeSublanes = 1; // number of sublanes for which no leader was found
        //if (ego->getID() == "disabled" && SIMTIME == 58) {
        //    std::cout << "DEBUG\n";
        //}
        const MSVehicle* veh = *last;
        while (freeSublanes > 0 && veh != 0) {
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND2(ego)) {
                std::cout << "      getLastVehicleInformation lane=" << getID() << " minPos=" << minPos << " veh=" << veh->getID() << " pos=" << veh->getPositionOnLane(this)  << "\n";
            }
#endif
            if (veh != ego && veh->getPositionOnLane(this) >= minPos) {
                const SUMOReal latOffset = veh->getLatOffset(this);
                freeSublanes = myLeaderInfoTmp.addLeader(veh, true, latOffset);
#ifdef DEBUG_PLAN_MOVE
                if (DEBUG_COND2(ego)) {
                    std::cout << "         latOffset=" << latOffset << " newLeaders=" << myLeaderInfoTmp.toString() << "\n";
                }
#endif
            }
            veh = *(++last);
        }
        if (ego == 0 && minPos == 0) {
            // update cached value
            myLeaderInfoTime = MSNet::getInstance()->getCurrentTimeStep();
            myLeaderInfo = myLeaderInfoTmp;
        }
#ifdef DEBUG_PLAN_MOVE
        //if (DEBUG_COND2(ego)) std::cout << SIMTIME
        //    << " getLastVehicleInformation lane=" << getID()
        //        << " ego=" << Named::getIDSecure(ego)
        //        << "\n"
        //        << "    vehicles=" << toString(myVehicles)
        //        << "    partials=" << toString(myPartialVehicles)
        //        << "\n"
        //        << "    result=" << myLeaderInfoTmp.toString()
        //        << "    cached=" << myLeaderInfo.toString()
        //        << "    myLeaderInfoTime=" << myLeaderInfoTime
        //        << "\n";
#endif
        return myLeaderInfoTmp;
    }
    return myLeaderInfo;
}


const MSLeaderInfo&
MSLane::getFirstVehicleInformation(const MSVehicle* ego, SUMOReal latOffset, bool onlyFrontOnLane, SUMOReal maxPos, bool allowCached) const {
    if (myFollowerInfoTime < MSNet::getInstance()->getCurrentTimeStep() || ego != 0 || maxPos < myLength || !allowCached || onlyFrontOnLane) {
        // XXX separate cache for onlyFrontOnLane = true
        myLeaderInfoTmp = MSLeaderInfo(this, ego, latOffset);
        AnyVehicleIterator first = anyVehiclesUpstreamBegin();
        int freeSublanes = 1; // number of sublanes for which no leader was found
        const MSVehicle* veh = *first;
        while (freeSublanes > 0 && veh != 0) {
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND2(ego)) {
                std::cout << "       veh=" << veh->getID() << " pos=" << veh->getPositionOnLane(this) << " maxPos=" << maxPos << "\n";
            }
#endif
            if (veh != ego && veh->getPositionOnLane(this) <= maxPos
                    && (!onlyFrontOnLane || veh->isFrontOnLane(this))) {
                //const SUMOReal latOffset = veh->getLane()->getRightSideOnEdge() - getRightSideOnEdge();
                const SUMOReal latOffset = veh->getLatOffset(this);
#ifdef DEBUG_PLAN_MOVE
                if (DEBUG_COND2(ego)) {
                    std::cout << "          veh=" << veh->getID() << " latOffset=" << latOffset << "\n";
                }
#endif
                freeSublanes = myLeaderInfoTmp.addLeader(veh, true, latOffset);
            }
            veh = *(++first);
        }
        if (ego == 0 && maxPos == std::numeric_limits<SUMOReal>::max()) {
            // update cached value
            myFollowerInfoTime = MSNet::getInstance()->getCurrentTimeStep();
            myFollowerInfo = myLeaderInfoTmp;
        }
#ifdef DEBUG_PLAN_MOVE
        //if (DEBUG_COND2(ego)) std::cout << SIMTIME
        //    << " getFirstVehicleInformation lane=" << getID()
        //        << " ego=" << Named::getIDSecure(ego)
        //        << "\n"
        //        << "    vehicles=" << toString(myVehicles)
        //        << "    partials=" << toString(myPartialVehicles)
        //        << "\n"
        //        << "    result=" << myLeaderInfoTmp.toString()
        //        //<< "    cached=" << myLeaderInfo.toString()
        //        << "    myLeaderInfoTime=" << myLeaderInfoTime
        //        << "\n";
#endif
        return myLeaderInfoTmp;
    }
    return myFollowerInfo;
}


// ------  ------
void
MSLane::planMovements(SUMOTime t) {
    assert(myVehicles.size() != 0);
    SUMOReal cumulatedVehLength = 0.;
    MSLeaderInfo ahead(this);
    // iterate over myVehicles and myPartialVehicles merge-sort style
    VehCont::reverse_iterator veh = myVehicles.rbegin();
    VehCont::reverse_iterator vehPart = myPartialVehicles.rbegin();
#ifdef DEBUG_PLAN_MOVE
    if (DEBUG_COND) std::cout
                << "\n"
                << SIMTIME
                << " planMovements lane=" << getID()
                << "\n"
                << "    vehicles=" << toString(myVehicles)
                << "    partials=" << toString(myPartialVehicles)
                << "\n";
#endif
    for (; veh != myVehicles.rend(); ++veh) {
        while (vehPart != myPartialVehicles.rend()
                && ((*vehPart)->getPositionOnLane(this) > (*veh)->getPositionOnLane())) {
            const SUMOReal latOffset = (*vehPart)->getLatOffset(this);
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << "    partial ahead: " << (*vehPart)->getID() << " latOffset=" << latOffset << "\n";
            }
#endif
            ahead.addLeader(*vehPart, false, latOffset);
            ++vehPart;
        }
#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            std::cout << "   plan move for: " << (*veh)->getID() << " ahead=" << ahead.toString() << "\n";
        }
#endif
        (*veh)->planMove(t, ahead, cumulatedVehLength);
        cumulatedVehLength += (*veh)->getVehicleType().getLengthWithGap();
        ahead.addLeader(*veh, false, 0);
    }
}


void
MSLane::detectCollisions(SUMOTime timestep, const std::string& stage) {
#ifdef DEBUG_COLLISIONS
    if (DEBUG_COND) {
        std::vector<const MSVehicle*> all;
        for (AnyVehicleIterator last = anyVehiclesBegin(); last != anyVehiclesEnd(); ++last) {
            all.push_back(*last);
        }
        std::cout << SIMTIME << " detectCollisions stage=" << stage << " lane=" << getID() << ":\n"
                  << "   vehs=" << toString(myVehicles) << "\n"
                  << "   part=" << toString(myPartialVehicles) << "\n"
                  << "   all=" << toString(all) << "\n"
                  << "\n";
    }
#endif

    if (myVehicles.size() == 0 || myCollisionAction == COLLISION_ACTION_NONE) {
        return;
    }
    std::set<const MSVehicle*, SUMOVehicle::ComparatorIdLess> toRemove;
    std::set<const MSVehicle*> toTeleport;
    if (MSGlobals::gLateralResolution <= 0 && MSGlobals::gLaneChangeDuration <= 0) {
        // no sublanes
        VehCont::iterator lastVeh = myVehicles.end() - 1;
        for (VehCont::iterator veh = myVehicles.begin(); veh != lastVeh; ++veh) {
            VehCont::iterator pred = veh + 1;
            detectCollisionBetween(timestep, stage, *veh, *pred, toRemove, toTeleport);
        }
        if (myPartialVehicles.size() > 0) {
            detectCollisionBetween(timestep, stage, *lastVeh, myPartialVehicles.front(), toRemove, toTeleport);
        }
    } else {
        // in the sublane-case it is insufficient to check the vehicles ordered
        // by their front position as there might be more than 2 vehicles next to each
        // other on the same lane
        // instead, a moving-window approach is used where all vehicles that
        // overlap in the longitudinal direction receive pairwise checks
        // XXX for efficiency, all lanes of an edge should be checked together
        // (lanechanger-style)

        // XXX quick hack: check each in myVehicles against all others
        for (AnyVehicleIterator veh = anyVehiclesBegin(); veh != anyVehiclesEnd(); ++veh) {
            MSVehicle* follow = (MSVehicle*)*veh;
            for (AnyVehicleIterator veh2 = anyVehiclesBegin(); veh2 != anyVehiclesEnd(); ++veh2) {
                MSVehicle* lead = (MSVehicle*)*veh2;
                if (lead == follow) {
                    continue;
                }
                if (lead->getPositionOnLane(this) < follow->getPositionOnLane(this)) {
                    continue;
                }
                if (detectCollisionBetween(timestep, stage, follow, lead, toRemove, toTeleport)) {
                    break;
                }
            }
            if (follow->getLaneChangeModel().getShadowLane() != 0 && follow->getLane() == this) {
                // check whether follow collides on the shadow lane
                const MSLane* shadowLane = follow->getLaneChangeModel().getShadowLane();
                MSLeaderInfo ahead = shadowLane->getLastVehicleInformation(follow,
                                     getRightSideOnEdge() - shadowLane->getRightSideOnEdge(),
                                     follow->getPositionOnLane());
                for (int i = 0; i < ahead.numSublanes(); ++i) {
                    const MSVehicle* lead = ahead[i];
                    if (lead != 0 && lead != follow && shadowLane->detectCollisionBetween(timestep, stage, follow, lead, toRemove, toTeleport)) {
                        break;
                    }
                }
            }
        }
    }

    if (myCheckJunctionCollisions && myEdge->isInternal()) {
        assert(myLinks.size() == 1);
        //std::cout << SIMTIME << " checkJunctionCollisions " << getID() << "\n";
        const std::vector<const MSLane*>& foeLanes = myLinks.front()->getFoeLanes();
        for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end(); ++veh) {
            const MSVehicle* collider = *veh;
            //std::cout << "   collider " << collider->getID() << "\n";
            PositionVector colliderBoundary = collider->getBoundingBox();
            for (std::vector<const MSLane*>::const_iterator it = foeLanes.begin(); it != foeLanes.end(); ++it) {
                const MSLane* foeLane = *it;
                //std::cout << "     foeLane " << foeLane->getID() << "\n";
                MSLane::AnyVehicleIterator end = foeLane->anyVehiclesEnd();
                for (MSLane::AnyVehicleIterator it_veh = foeLane->anyVehiclesBegin(); it_veh != end; ++it_veh) {
                    MSVehicle* victim = (MSVehicle*)*it_veh;
                    //std::cout << "             victim " << victim->getID() << "\n";
                    if (colliderBoundary.overlapsWith(victim->getBoundingBox())) {
                        // make a detailed check
                        if (collider->getBoundingPoly().overlapsWith(victim->getBoundingPoly())) {
                            handleCollisionBetween(timestep, stage, collider, victim, -1, 0, toRemove, toTeleport);
                        }
                    }
                }
            }
        }
    }

    for (std::set<const MSVehicle*, SUMOVehicle::ComparatorIdLess>::iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
        MSVehicle* veh = const_cast<MSVehicle*>(*it);
        MSLane* vehLane = veh->getLane();
        vehLane->removeVehicle(veh, MSMoveReminder::NOTIFICATION_TELEPORT, false);
        if (toTeleport.count(veh) > 0) {
            MSVehicleTransfer::getInstance()->add(timestep, veh);
        } else {
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        }
    }
}


bool
MSLane::detectCollisionBetween(SUMOTime timestep, const std::string& stage, const MSVehicle* collider, const MSVehicle* victim,
                               std::set<const MSVehicle*, SUMOVehicle::ComparatorIdLess>& toRemove,
                               std::set<const MSVehicle*>& toTeleport) const {
    assert(collider->isFrontOnLane(this));
#ifndef NO_TRACI
    if (myCollisionAction == COLLISION_ACTION_TELEPORT && ((victim->hasInfluencer() && victim->getInfluencer()->isVTDAffected(timestep)) ||
            (collider->hasInfluencer() && collider->getInfluencer()->isVTDAffected(timestep)))) {
        return false;
    }
#endif
    const bool colliderOpposite = collider->getLaneChangeModel().isOpposite();
    const bool bothOpposite = victim->getLaneChangeModel().isOpposite() && colliderOpposite;
    if (bothOpposite) {
        std::swap(victim, collider);
    }
    const SUMOReal colliderPos = colliderOpposite ? collider->getBackPositionOnLane(this) : collider->getPositionOnLane(this);
    SUMOReal gap = victim->getBackPositionOnLane(this) - colliderPos - collider->getVehicleType().getMinGap();
    if (bothOpposite) {
        gap = -gap - 2 * collider->getVehicleType().getMinGap();
    }
#ifdef DEBUG_COLLISIONS
    if (DEBUG_COND) std::cout << SIMTIME
                                  << " thisLane=" << getID()
                                  << " collider=" << collider->getID()
                                  << " victim=" << victim->getID()
                                  << " colliderLane=" << collider->getLane()->getID()
                                  << " victimLane=" << victim->getLane()->getID()
                                  << " colliderPos=" << colliderPos
                                  << " victimBackPos=" << victim->getBackPositionOnLane(this)
                                  << " colliderLat=" << collider->getCenterOnEdge(this)
                                  << " victimLat=" << victim->getCenterOnEdge(this)
                                  << " gap=" << gap
                                  << "\n";
#endif
    if (gap < -NUMERICAL_EPS) {
        SUMOReal latGap = 0;
        if (MSGlobals::gLateralResolution > 0 || MSGlobals::gLaneChangeDuration > 0) {
            latGap = (fabs(victim->getCenterOnEdge(this) - collider->getCenterOnEdge(this))
                      - 0.5 * fabs(victim->getVehicleType().getWidth() + collider->getVehicleType().getWidth()));
            if (latGap + NUMERICAL_EPS > 0) {
                return false;
            }
        }
        if (MSGlobals::gLaneChangeDuration > DELTA_T
                && collider->getLaneChangeModel().isChangingLanes()
                && victim->getLaneChangeModel().isChangingLanes()
                && victim->getLane() != this) {
            // synchroneous lane change maneuver
            return false;
        }
        handleCollisionBetween(timestep, stage, collider, victim, gap, latGap, toRemove, toTeleport);
        return true;
    }
    return false;
}


void
MSLane::handleCollisionBetween(SUMOTime timestep, const std::string& stage, const MSVehicle* collider, const MSVehicle* victim,
                               SUMOReal gap, SUMOReal latGap, std::set<const MSVehicle*, SUMOVehicle::ComparatorIdLess>& toRemove,
                               std::set<const MSVehicle*>& toTeleport) const {
    std::string prefix;
    switch (myCollisionAction) {
        case COLLISION_ACTION_WARN:
            prefix = "Vehicle '" + collider->getID() + "'; collision with vehicle '" + victim->getID() ;
            break;
        case COLLISION_ACTION_TELEPORT:
            prefix = "Teleporting vehicle '" + collider->getID() + "'; collision with vehicle '" + victim->getID() ;
            toRemove.insert(collider);
            toTeleport.insert(collider);
            break;
        case COLLISION_ACTION_REMOVE: {
            prefix = "Removing collision participants: vehicle '" + collider->getID() + "', vehicle '" + victim->getID();
            bool removeCollider = true;
            bool removeVictim = true;
#ifndef NO_TRACI
            removeVictim = !(victim->hasInfluencer() && victim->getInfluencer()->isVTDAffected(timestep));
            removeCollider = !(collider->hasInfluencer() && collider->getInfluencer()->isVTDAffected(timestep));
            if (removeVictim) {
                toRemove.insert(victim);
            }
            if (removeCollider) {
                toRemove.insert(collider);
            }
            if (!removeVictim) {
                if (!removeCollider) {
                    prefix = "Keeping remote-controlled collision participants: vehicle '" + collider->getID() + "', vehicle '" + victim->getID();
                } else {
                    prefix = "Removing collision participant: vehicle '" + collider->getID() + "', keeping remote-controlled vehicle '" + victim->getID();
                }
            } else if (!removeCollider) {
                prefix = "Keeping remote-controlled collision participant: vehicle '" + collider->getID() + "', removing vehicle '" + victim->getID();
            }
#else
            toRemove.insert(victim);
            toRemove.insert(collider);
#endif
            break;
        }
        default:
            break;
    }
    WRITE_WARNING(prefix
                  + "', lane='" + getID()
                  + "', gap=" + toString(gap)
                  + (latGap == 0 ? "" : "', latGap=" + toString(latGap))
                  + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep())
                  + " stage=" + stage + ".");
    MSNet::getInstance()->getVehicleControl().registerCollision();
}


bool
MSLane::executeMovements(SUMOTime t, std::vector<MSLane*>& lanesWithVehiclesToIntegrate) {
    // iterate over vehicles in reverse so that move reminders will be called in the correct order
    for (VehCont::reverse_iterator i = myVehicles.rbegin(); i != myVehicles.rend();) {
        MSVehicle* veh = *i;
        // length is needed later when the vehicle may not exist anymore
        const SUMOReal length = veh->getVehicleType().getLengthWithGap();
        const SUMOReal nettoLength = veh->getVehicleType().getLength();
        const bool moved = veh->executeMove();
        MSLane* const target = veh->getLane();
        if (veh->hasArrived()) {
            // vehicle has reached its arrival position
            veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_ARRIVED);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        } else if (target != 0 && moved) {
            if (target->getEdge().isVaporizing()) {
                // vehicle has reached a vaporizing edge
                veh->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED);
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
            } else {
                // vehicle has entered a new lane (leaveLane and workOnMoveReminders were already called in MSVehicle::executeMove)
                target->myVehBuffer.push_back(veh);
                lanesWithVehiclesToIntegrate.push_back(target);
            }
        } else if (veh->isParking()) {
            // vehicle started to park
            MSVehicleTransfer::getInstance()->add(t, veh);
        } else if (veh->getPositionOnLane() > getLength()) {
            // for any reasons the vehicle is beyond its lane...
            // this should never happen because it is handled in MSVehicle::executeMove
            assert(false);
            WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; beyond end of lane, target lane='" + getID() + "', time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSNet::getInstance()->getVehicleControl().registerCollision();
            MSVehicleTransfer::getInstance()->add(t, veh);
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
            MSVehicle* veh = myVehicles.back(); // the vehice at the front of the queue
            if (!veh->isStopped() && veh->getLane() == this) {
                const bool wrongLane = !veh->getLane()->appropriate(veh);
                const bool r1 = MSGlobals::gTimeToGridlock > 0 && veh->getWaitingTime() > MSGlobals::gTimeToGridlock;
                const bool r2 = MSGlobals::gTimeToGridlockHighways > 0 && veh->getWaitingTime() > MSGlobals::gTimeToGridlockHighways && veh->getLane()->getSpeedLimit() > 69. / 3.6 && wrongLane;
                if (r1 || r2) {
                    const MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
                    const bool minorLink = !wrongLane && (link != myLinks.end()) && !((*link)->havePriority());
                    const std::string reason = (wrongLane ? " (wrong lane)" : (minorLink ? " (yield)" : " (jam)"));
                    MSVehicle* veh = *(myVehicles.end() - 1);
                    myBruttoVehicleLengthSum -= veh->getVehicleType().getLengthWithGap();
                    myNettoVehicleLengthSum -= veh->getVehicleType().getLength();
                    myVehicles.erase(myVehicles.end() - 1);
                    WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; waited too long"
                                  + reason
                                  + (r2 ? " (highway)" : "")
                                  + ", lane='" + getID() + "', time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                    if (wrongLane) {
                        MSNet::getInstance()->getVehicleControl().registerTeleportWrongLane();
                    } else if (minorLink) {
                        MSNet::getInstance()->getVehicleControl().registerTeleportYield();
                    } else {
                        MSNet::getInstance()->getVehicleControl().registerTeleportJam();
                    }
                    MSVehicleTransfer::getInstance()->add(t, veh);
                }
            } // else look for a (waiting) vehicle that isn't stopped?
        }
    }
    if (MSGlobals::gLateralResolution > 0) {
        // trigger sorting of vehicles as their order may have changed
        lanesWithVehiclesToIntegrate.push_back(this);
    }
    return myVehicles.size() == 0;
}


const MSEdge*
MSLane::getNextNormal() const {
    const MSEdge* e = myEdge;
    while (e->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        e = e->getSuccessors()[0];
    }
    return e;
}


// ------ Static (sic!) container methods  ------
bool
MSLane::dictionary(const std::string& id, MSLane* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSLane*
MSLane::dictionary(const std::string& id) {
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


template<class RTREE> void
MSLane::fill(RTREE& into) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        MSLane* l = (*i).second;
        Boundary b = l->getShape().getBoxBoundary();
        b.grow(3.);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        into.Insert(cmin, cmax, l);
    }
}

template void MSLane::fill<NamedRTree>(NamedRTree& into);
#ifndef NO_TRACI
template void MSLane::fill<LANE_RTREE_QUAL>(LANE_RTREE_QUAL& into);
#endif

// ------   ------
bool
MSLane::appropriate(const MSVehicle* veh) {
    if (myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if (veh->succEdge(1) == 0) {
        assert((int)veh->getBestLanes().size() > veh->getLaneIndex());
        if (veh->getBestLanes()[veh->getLaneIndex()].bestLaneOffset == 0) {
            return true;
        } else {
            return false;
        }
    }
    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
    return (link != myLinks.end());
}


bool
MSLane::integrateNewVehicle(SUMOTime) {
    bool wasInactive = myVehicles.size() == 0;
    sort(myVehBuffer.begin(), myVehBuffer.end(), vehicle_position_sorter(this));
    for (std::vector<MSVehicle*>::const_iterator i = myVehBuffer.begin(); i != myVehBuffer.end(); ++i) {
        MSVehicle* veh = *i;
        assert(veh->getLane() == this);
        myVehicles.insert(myVehicles.begin(), veh);
        myBruttoVehicleLengthSum += veh->getVehicleType().getLengthWithGap();
        myNettoVehicleLengthSum += veh->getVehicleType().getLength();
        //if (true) std::cout << SIMTIME << " integrateNewVehicle lane=" << getID() << " veh=" << veh->getID() << " (on lane " << veh->getLane()->getID() << ") into lane=" << getID() << " myBrutto=" << myBruttoVehicleLengthSum << "\n";
        myEdge->markDelayed();
    }
    myVehBuffer.clear();
    //std::cout << SIMTIME << " integrateNewVehicle lane=" << getID() << " myVehicles1=" << toString(myVehicles);
    if (MSGlobals::gLateralResolution > 0 || myNeighs.size() > 0) {
        sort(myVehicles.begin(), myVehicles.end(), vehicle_natural_position_sorter(this));
    }
    sortPartialVehicles();
#ifdef DEBUG_VEHICLE_CONTAINER
    if (DEBUG_COND) std::cout << SIMTIME << " integrateNewVehicle lane=" << getID()
                                  << " vhicles=" << toString(myVehicles) << " partials=" << toString(myPartialVehicles) << "\n";
#endif
    return wasInactive && myVehicles.size() != 0;
}


void
MSLane::sortPartialVehicles() {
    if (myPartialVehicles.size() > 1) {
        sort(myPartialVehicles.begin(), myPartialVehicles.end(), vehicle_natural_position_sorter(this));
    }
}

bool
MSLane::isLinkEnd(MSLinkCont::const_iterator& i) const {
    return i == myLinks.end();
}


bool
MSLane::isLinkEnd(MSLinkCont::iterator& i) {
    return i == myLinks.end();
}

bool
MSLane::isEmpty() const {
    return myVehicles.empty() && myPartialVehicles.empty();
}

bool
MSLane::isInternal() const {
    return myEdge->getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL;
}

MSVehicle*
MSLane::getLastFullVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return myVehicles.front();
}


MSVehicle*
MSLane::getFirstFullVehicle() const {
    if (myVehicles.size() == 0) {
        return 0;
    }
    return myVehicles.back();
}


MSVehicle*
MSLane::getLastAnyVehicle() const {
    // all vehicles in myVehicles should have positions smaller or equal to
    // those in myPartialVehicles
    if (myVehicles.size() > 0) {
        return myVehicles.front();
    }
    if (myPartialVehicles.size() > 0) {
        return myPartialVehicles.front();
    }
    return 0;
}


MSVehicle*
MSLane::getFirstAnyVehicle() const {
    MSVehicle* result = 0;
    if (myVehicles.size() > 0) {
        result = myVehicles.back();
    }
    if (myPartialVehicles.size() > 0
            && (result == 0 || result->getPositionOnLane(this) < myPartialVehicles.back()->getPositionOnLane(this))) {
        result = myPartialVehicles.back();
    }
    return result;
}


MSLinkCont::const_iterator
MSLane::succLinkSec(const SUMOVehicle& veh, int nRouteSuccs,
                    const MSLane& succLinkSource, const std::vector<MSLane*>& conts) {
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge == 0) {
        // return end (no succeeding link) if so
        return succLinkSource.myLinks.end();
    }
    // if we are on an internal lane there should only be one link and it must be allowed
    if (succLinkSource.isInternal()) {
        assert(succLinkSource.myLinks.size() == 1);
        // could have been disallowed dynamically with a rerouter or via TraCI
        // assert(succLinkSource.myLinks[0]->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass()));
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
    if (nRouteSuccs < (int)conts.size()) {
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
    // the "'" around the ids are missing intentionally in the message below because it slows messaging down, resulting in test timeouts
    WRITE_WARNING("Could not find connection between lane " + succLinkSource.getID() + " and lane " + conts[nRouteSuccs]->getID() +
                  " for vehicle " + veh.getID() + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
#endif
    return succLinkSource.myLinks.end();
}



const MSLinkCont&
MSLane::getLinkCont() const {
    return myLinks;
}


void
MSLane::setMaxSpeed(SUMOReal val) {
    myMaxSpeed = val;
    myEdge->recalcCache();
}


void
MSLane::setLength(SUMOReal val) {
    myLength = val;
    myEdge->recalcCache();
}


void
MSLane::swapAfterLaneChange(SUMOTime) {
    //if (getID() == "disabled_lane") std::cout << SIMTIME << " swapAfterLaneChange lane=" << getID() << " myVehicles=" << toString(myVehicles) << " myTmpVehicles=" << toString(myTmpVehicles) << "\n";
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
    // this needs to be done after finishing lane-changing for all lanes on the
    // current edge (MSLaneChanger::updateLanes())
    sortPartialVehicles();
}


MSVehicle*
MSLane::removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification, bool notify) {
    assert(remVehicle->getLane() == this);
    for (MSLane::VehCont::iterator it = myVehicles.begin(); it < myVehicles.end(); it++) {
        if (remVehicle == *it) {
            if (notify) {
                remVehicle->leaveLane(notification);
            }
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
MSLane::addApproachingLane(MSLane* lane, bool warnMultiCon) {
    MSEdge* approachingEdge = &lane->getEdge();
    if (myApproachingLanes.find(approachingEdge) == myApproachingLanes.end()) {
        myApproachingLanes[approachingEdge] = std::vector<MSLane*>();
    } else if (approachingEdge->getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL && warnMultiCon) {
        // whenever a normal edge connects twice, there is a corresponding
        // internal edge wich connects twice, one warning is sufficient
        WRITE_WARNING("Lane '" + getID() + "' is approached multiple times from edge '" + approachingEdge->getID() + "'. This may cause collisions.");
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
    inline int operator()(const std::pair<const MSVehicle*, SUMOReal>& p1, const std::pair<const MSVehicle*, SUMOReal>& p2) const {
        return p1.second < p2.second;
    }
};


SUMOReal MSLane::getMissingRearGap(
    SUMOReal backOffset, SUMOReal leaderSpeed, SUMOReal leaderMaxDecel) const {
    // this follows the same logic as getFollowerOnConsecutive. we do a tree
    // search and check for the vehicle with the largest missing rear gap within
    // relevant range
    SUMOReal result = 0;
    std::pair<MSVehicle* const, SUMOReal> followerInfo = getFollowerOnConsecutive(backOffset, leaderSpeed, leaderMaxDecel);
    MSVehicle* v = followerInfo.first;
    if (v != 0) {
        result = v->getCarFollowModel().getSecureGap(v->getSpeed(), leaderSpeed, leaderMaxDecel) - followerInfo.second;
    }
    return result;
}


SUMOReal
MSLane::getMaximumBrakeDist() const {
    const MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    const SUMOReal maxSpeed = getSpeedLimit() * vc.getMaxSpeedFactor();
    // NOTE: For the euler update this is an upper bound on the actual braking distance (see ticket #860)
    return maxSpeed * maxSpeed * 0.5 / vc.getMinDeceleration();
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getFollowerOnConsecutive(
    SUMOReal backOffset, SUMOReal leaderSpeed, SUMOReal leaderMaxDecel, SUMOReal dist, bool ignoreMinorLinks) const {
    // do a tree search among all follower lanes and check for the most
    // important vehicle (the one requiring the largest reargap)
    // to get a safe bound on the necessary search depth, we need to consider the maximum speed and minimum
    // deceleration of potential follower vehicles
    if (dist == -1) {
        dist = getMaximumBrakeDist() - backOffset;
    }
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND) std::cout << SIMTIME << " getFollowerOnConsecutive"
                                  << " backOffset=" << backOffset
                                  << " leaderSpeed=" << leaderSpeed
                                  << " dist=" << dist
                                  << "\n";
#endif
    std::pair<MSVehicle*, SUMOReal> result(static_cast<MSVehicle*>(0), -1);
    SUMOReal missingRearGapMax = -std::numeric_limits<SUMOReal>::max();
    std::set<MSLane*> visited;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size() != 0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i = toExamine.begin(); i != toExamine.end(); ++i) {
            MSLane* next = (*i).lane;
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND) {
                std::cout << SIMTIME << "     nextLane=" << next->getID() << "\n";
            }
#endif
            // XXX: why take a MAX here? Even if some upcoming lane allows to go faster, the follower should be able
            // to brake in time if we only consider 'next->getMaximumBrakeDist() - backOffset'.
            dist = MAX2(dist, next->getMaximumBrakeDist() - backOffset);
            // Checking the first vehicle on the considered lane.
            // XXX: Could there be a problem if someone fast is just overtaking a a slow first vehicle, e.g.? (Leo) Refs.#2578
            MSVehicle* v = next->getFirstAnyVehicle();
            SUMOReal agap = 0;
            if (v != 0) {
                // the front of v is already on divergent trajectory from the ego vehicle
                // for which this method is called (in the context of MSLaneChanger).
                // Therefore, technically v is not a follower but only an obstruction and
                // the gap is not between the front of v and the back of ego
                // but rather between the flank of v and the back of ego.
                if (!v->isFrontOnLane(next)) {
                    agap = (*i).length - next->getLength() + backOffset
                           /// XXX dubious term. here for backwards compatibility
                           - v->getVehicleType().getMinGap();
                    if (agap > 0) {
                        // Only if ego overlaps we treat v as if it were a real follower
                        // Otherwise we ignore it and look for another follower
                        v = next->getFirstFullVehicle();
                        if (v != 0) {
                            agap = (*i).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                        }
                    }
                } else {
                    agap = (*i).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                }
#ifdef DEBUG_CONTEXT
                if (DEBUG_COND) std::cout << SIMTIME << "    "
                                              << " v=" << Named::getIDSecure(v)
                                              << " agap=" << agap
                                              << " dist=" << dist
                                              << "\n";
#endif
            }
            if (v != 0) {
                const SUMOReal missingRearGap = v->getCarFollowModel().getSecureGap(v->getSpeed(), leaderSpeed, leaderMaxDecel) - agap;
                if (missingRearGap > missingRearGapMax) {
                    missingRearGapMax = missingRearGap;
                    result.first = v;
                    result.second = agap;
                }
            } else {
                if ((*i).length < dist) {
                    const std::vector<MSLane::IncomingLaneInfo>& followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j = followers.begin(); j != followers.end(); ++j) {
                        if (visited.find((*j).lane) == visited.end() && ((*j).viaLink->havePriority() || !ignoreMinorLinks)) {
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
MSLane::getLeader(const MSVehicle* veh, const SUMOReal vehPos, const std::vector<MSLane*>& bestLaneConts, SUMOReal dist, bool checkTmpVehicles) const {
    // get the leading vehicle for (shadow) veh
    // XXX this only works as long as all lanes of an edge have equal length
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(veh)) {
        std::cout << "   getLeader lane=" << getID() << " ego=" << veh->getID() << " vehs=" << toString(myVehicles) << " tmpVehs=" << toString(myTmpVehicles) << "\n";
    }
#endif
    if (checkTmpVehicles) {
        for (VehCont::const_iterator last = myTmpVehicles.begin(); last != myTmpVehicles.end(); ++last) {
            // XXX refactor leaderInfo to use a const vehicle all the way through the call hierarchy
            MSVehicle* pred = (MSVehicle*)*last;
            if (pred == veh) {
                continue;
            }
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(veh)) {
                std::cout << "   getLeader lane=" << getID() << " ego=" << veh->getID() << " egoPos=" << vehPos << " pred=" << pred->getID() << " predPos=" << pred->getPositionOnLane() << "\n";
            }
#endif
            if (pred->getPositionOnLane() > vehPos + NUMERICAL_EPS) {
                return std::pair<MSVehicle* const, SUMOReal>(pred, pred->getBackPositionOnLane(this) - veh->getVehicleType().getMinGap() - vehPos);
            }
        }
    } else {
        for (AnyVehicleIterator last = anyVehiclesBegin(); last != anyVehiclesEnd(); ++last) {
            // XXX refactor leaderInfo to use a const vehicle all the way through the call hierarchy
            MSVehicle* pred = (MSVehicle*)*last;
            if (pred == veh) {
                continue;
            }
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(veh)) {
                std::cout << "   getLeader lane=" << getID() << " ego=" << veh->getID() << " egoPos=" << vehPos
                          << " pred=" << pred->getID() << " predPos=" << pred->getPositionOnLane(this) << " predBack=" << pred->getBackPositionOnLane(this) << "\n";
            }
#endif
            if (pred->getPositionOnLane(this) > vehPos + NUMERICAL_EPS) {
                return std::pair<MSVehicle* const, SUMOReal>(pred, pred->getBackPositionOnLane(this) - veh->getVehicleType().getMinGap() - vehPos);
            }
        }
    }
    // XXX from here on the code mirrors MSLaneChanger::getRealLeader
    if (bestLaneConts.size() > 0) {
        SUMOReal seen = getLength() - vehPos;
        SUMOReal speed = veh->getSpeed();
        if (dist < 0) {
            dist = veh->getCarFollowModel().brakeGap(speed) + veh->getVehicleType().getMinGap();
        }
#ifdef DEBUG_CONTEXT
        if (DEBUG_COND2(veh)) {
            std::cout << "   getLeader lane=" << getID() << " seen=" << seen << " dist=" << dist << "\n";
        }
#endif
        if (seen > dist) {
            return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
        }
        return getLeaderOnConsecutive(dist, seen, speed, *veh, bestLaneConts);
    } else {
        return std::make_pair(static_cast<MSVehicle*>(0), -1);
    }
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle& veh,
                               const std::vector<MSLane*>& bestLaneConts) const {
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(&veh)) {
        std::cout << "   getLeaderOnConsecutive lane=" << getID() << " ego=" << veh.getID() << " seen=" << seen << " dist=" << dist << " conts=" << toString(bestLaneConts) << "\n";
    }
#endif
    if (seen > dist) {
        return std::make_pair(static_cast<MSVehicle*>(0), -1);
    }
    int view = 1;
    // loop over following lanes
    if (myPartialVehicles.size() > 0) {
        // XXX
        MSVehicle* pred = myPartialVehicles.front();
#ifdef DEBUG_CONTEXT
        if (DEBUG_COND2(&veh)) {
            std::cout << "    partials=" << toString(myPartialVehicles) << "\n";
        }
#endif
        return std::pair<MSVehicle* const, SUMOReal>(pred, seen - (getLength() - pred->getBackPositionOnLane(this)) - veh.getVehicleType().getMinGap());
    }
    const MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, NUMERICAL_EPS));
    do {
        nextLane->getVehiclesSecure(); // lock against running sim when called from GUI for time gap coloring
        // get the next link used
        MSLinkCont::const_iterator link = succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, veh.getVehicleType().getLength(),
                veh.getImpatience(), veh.getCarFollowModel().getMaxDecel(), 0, veh.getLateralPositionOnLane()) || (*link)->haveRed()) {
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(&veh)) {
                std::cout << "    cannot continue after nextLane=" << nextLane->getID() << "\n";
            }
#endif
            nextLane->releaseVehicles();
            break;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(&veh, seen);
        nextLane->releaseVehicles();
        if (linkLeaders.size() > 0) {
            // XXX if there is more than one link leader we should return the most important
            // one (gap, decel) but this is hard to know at this point
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(&veh)) {
                std::cout << "    found linkLeader after nextLane=" << nextLane->getID() << "\n";
            }
#endif
            return linkLeaders[0].vehAndGap;
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }
        nextLane->getVehiclesSecure(); // lock against running sim when called from GUI for time gap coloring
        MSVehicle* leader = nextLane->getLastAnyVehicle();
        if (leader != 0) {
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(&veh)) {
                std::cout << "    found leader " << leader->getID() << " on nextLane=" << nextLane->getID() << "\n";
            }
#endif
            const SUMOReal dist = seen + leader->getBackPositionOnLane(nextLane) - veh.getVehicleType().getMinGap();
            nextLane->releaseVehicles();
            return std::make_pair(leader, dist);
        }
        nextLane->releaseVehicles();
        if (nextLane->getVehicleMaxSpeed(&veh) < speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(&veh));
        }
        seen += nextLane->getLength();
        if (seen <= dist) {
            // delaying the update of arrivalTime and making it conditional to avoid possible integer overflows
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    } while (seen <= dist);
    return std::make_pair(static_cast<MSVehicle*>(0), -1);
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getCriticalLeader(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle& veh) const {
    const std::vector<MSLane*>& bestLaneConts = veh.getBestLanesContinuation(this);
    std::pair<MSVehicle*, SUMOReal> result = std::make_pair(static_cast<MSVehicle*>(0), -1);
    SUMOReal safeSpeed = std::numeric_limits<SUMOReal>::max();
    int view = 1;
    // loop over following lanes
    // @note: we don't check the partial occupator for this lane since it was
    // already checked in MSLaneChanger::getRealLeader()
    const MSLane* nextLane = this;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, NUMERICAL_EPS));
    do {
        // get the next link used
        MSLinkCont::const_iterator link = succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, veh.getVehicleType().getLength(),
                veh.getImpatience(), veh.getCarFollowModel().getMaxDecel(), 0, veh.getLateralPositionOnLane()) || (*link)->haveRed()) {
            return result;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(&veh, seen);
        for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
            const MSVehicle* leader = (*it).vehAndGap.first;
            if (leader != 0 && leader != result.first) {
                // XXX ignoring pedestrians here!
                // XXX ignoring the fact that the link leader may alread by following us
                // XXX ignoring the fact that we may drive up to the crossing point
                const SUMOReal tmpSpeed = veh.getSafeFollowSpeed((*it).vehAndGap, seen, nextLane, (*it).distToCrossing);
                if (tmpSpeed < safeSpeed) {
                    safeSpeed = tmpSpeed;
                    result = (*it).vehAndGap;
                }
            }
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }
        MSVehicle* leader = nextLane->getLastAnyVehicle();
        if (leader != 0 && leader != result.first) {
            const SUMOReal gap = seen + leader->getBackPositionOnLane(nextLane) - veh.getVehicleType().getMinGap();
            const SUMOReal tmpSpeed = veh.getCarFollowModel().insertionFollowSpeed(leader, speed, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
            if (tmpSpeed < safeSpeed) {
                safeSpeed = tmpSpeed;
                result = std::make_pair(leader, gap);
            }
        }
        if (nextLane->getVehicleMaxSpeed(&veh) < speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(&veh));
        }
        seen += nextLane->getLength();
        if (seen <= dist) {
            // delaying the update of arrivalTime and making it conditional to avoid possible integer overflows
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    } while (seen <= dist);
    return result;
}


MSLane*
MSLane::getLogicalPredecessorLane() const {
    if (myLogicalPredecessorLane != 0) {
        return myLogicalPredecessorLane;
    }
    if (myLogicalPredecessorLane == 0) {
        MSEdgeVector pred = myEdge->getPredecessors();
        // get only those edges which connect to this lane
        for (MSEdgeVector::iterator i = pred.begin(); i != pred.end();) {
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(*i));
            if (j == myIncomingLanes.end()) {
                i = pred.erase(i);
            } else {
                ++i;
            }
        }
        // get the lane with the "straightest" connection
        if (pred.size() != 0) {
            std::sort(pred.begin(), pred.end(), by_connections_to_sorter(&getEdge()));
            MSEdge* best = *pred.begin();
            std::vector<IncomingLaneInfo>::const_iterator j = find_if(myIncomingLanes.begin(), myIncomingLanes.end(), edge_finder(best));
            myLogicalPredecessorLane = (*j).lane;
        }
    }
    return myLogicalPredecessorLane;
}


MSLane*
MSLane::getLogicalPredecessorLane(const MSEdge& fromEdge) const {
    for (std::vector<IncomingLaneInfo>::const_iterator i = myIncomingLanes.begin(); i != myIncomingLanes.end(); ++i) {
        MSLane* cand = (*i).lane;
        if (&(cand->getEdge()) == &fromEdge) {
            return (*i).lane;
        }
    }
    return 0;
}


LinkState
MSLane::getIncomingLinkState() const {
    MSLane* pred = getLogicalPredecessorLane();
    if (pred == 0) {
        return LINKSTATE_DEADEND;
    } else {
        return MSLinkContHelper::getConnectingLink(*pred, *this)->getState();
    }
}


std::vector<const MSLane*>
MSLane::getOutgoingLanes() const {
    std::vector<const MSLane*> result;
    for (MSLinkCont::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        assert((*i)->getLane() != 0);
        result.push_back((*i)->getLane());
    }
    return result;
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


int
MSLane::getCrossingIndex() const {
    for (MSLinkCont::const_iterator i = myLinks.begin(); i != myLinks.end(); ++i) {
        if ((*i)->getLane()->getEdge().isCrossing()) {
            return (int)(i - myLinks.begin());
        }
    }
    return -1;
}

// ------------ Current state retrieval
SUMOReal
MSLane::getBruttoOccupancy() const {
    SUMOReal fractions = myPartialVehicles.size() > 0 ? myLength - myPartialVehicles.front()->getBackPositionOnLane(this) : 0;
    getVehiclesSecure();
    if (myVehicles.size() != 0) {
        MSVehicle* lastVeh = myVehicles.front();
        if (lastVeh->getPositionOnLane() < lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength() - lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myBruttoVehicleLengthSum + fractions) / myLength;
}


SUMOReal
MSLane::getNettoOccupancy() const {
    SUMOReal fractions = myPartialVehicles.size() > 0 ? myLength - myPartialVehicles.front()->getBackPositionOnLane(this) : 0;
    getVehiclesSecure();
    if (myVehicles.size() != 0) {
        MSVehicle* lastVeh = myVehicles.front();
        if (lastVeh->getPositionOnLane() < lastVeh->getVehicleType().getLength()) {
            fractions -= (lastVeh->getVehicleType().getLength() - lastVeh->getPositionOnLane());
        }
    }
    releaseVehicles();
    return (myNettoVehicleLengthSum + fractions) / myLength;
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
MSLane::getCO2Emissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getCO2Emissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getCOEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getCOEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getPMxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getPMxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getNOxEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getNOxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHCEmissions() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getHCEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getFuelConsumption() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getFuelConsumption();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getElectricityConsumption() const {
    SUMOReal ret = 0;
    const MSLane::VehCont& vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        ret += (*i)->getElectricityConsumption();
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
    return v1->getBackPositionOnLane(myLane) > v2->getBackPositionOnLane(myLane);
}

int
MSLane::vehicle_natural_position_sorter::operator()(MSVehicle* v1, MSVehicle* v2) const {
    const SUMOReal pos1 = v1->getBackPositionOnLane(myLane);
    const SUMOReal pos2 = v2->getBackPositionOnLane(myLane);
    if (pos1 != pos2) {
        return pos1 < pos2;
    } else {
        return v1->getLateralPositionOnLane() < v2->getLateralPositionOnLane();
    }
}

MSLane::by_connections_to_sorter::by_connections_to_sorter(const MSEdge* const e) :
    myEdge(e),
    myLaneDir(e->getLanes()[0]->getShape().angleAt2D(0)) {
}


int
MSLane::by_connections_to_sorter::operator()(const MSEdge* const e1, const MSEdge* const e2) const {
    const std::vector<MSLane*>* ae1 = e1->allowedLanes(*myEdge);
    const std::vector<MSLane*>* ae2 = e2->allowedLanes(*myEdge);
    SUMOReal s1 = 0;
    if (ae1 != 0 && ae1->size() != 0) {
        s1 = (SUMOReal) ae1->size() + fabs(GeomHelper::angleDiff((*ae1)[0]->getShape().angleAt2D(0), myLaneDir)) / M_PI / 2.;
    }
    SUMOReal s2 = 0;
    if (ae2 != 0 && ae2->size() != 0) {
        s2 = (SUMOReal) ae2->size() + fabs(GeomHelper::angleDiff((*ae2)[0]->getShape().angleAt2D(0), myLaneDir)) / M_PI / 2.;
    }
    return s1 < s2;
}


void
MSLane::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_LANE);
    out.writeAttr("id", getID()); // using "id" instead of SUMO_ATTR_ID makes the value only show up in xml state
    out.openTag(SUMO_TAG_VIEWSETTINGS_VEHICLES);
    out.writeAttr(SUMO_ATTR_VALUE, myVehicles);
    out.closeTag();
    out.closeTag();
}


void
MSLane::loadState(std::vector<std::string>& vehIds, MSVehicleControl& vc) {
    for (std::vector<std::string>::const_iterator it = vehIds.begin(); it != vehIds.end(); ++it) {
        MSVehicle* v = dynamic_cast<MSVehicle*>(vc.getVehicle(*it));
        if (v != 0) {
            v->updateBestLanes(false, this);
            incorporateVehicle(v, v->getPositionOnLane(), v->getSpeed(), v->getLateralPositionOnLane(), myVehicles.end(),
                               MSMoveReminder::NOTIFICATION_JUNCTION);
            v->processNextStop(v->getSpeed());
        }
    }
}


MSLeaderDistanceInfo
MSLane::getFollowersOnConsecutive(const MSVehicle* ego, bool allSublanes) const {
    // get the follower vehicle on the lane to change to
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(ego)) {
        std::cout << SIMTIME << " getFollowers lane=" << getID() << " ego=" << ego->getID() << " pos=" << ego->getPositionOnLane() << "\n";
    }
#endif
    assert(ego != 0);
    assert(&ego->getLane()->getEdge() == &getEdge());
    const SUMOReal egoLatDist = ego->getLane()->getRightSideOnEdge() - getRightSideOnEdge();
    MSCriticalFollowerDistanceInfo result(this, allSublanes ? 0 : ego, allSublanes ? 0 : egoLatDist);
    /// XXX iterate in reverse and abort when there are no more freeSublanes
    for (AnyVehicleIterator last = anyVehiclesBegin(); last != anyVehiclesEnd(); ++last) {
        const MSVehicle* veh = *last;
#ifdef DEBUG_CONTEXT
        if (DEBUG_COND2(ego)) {
            std::cout << "  veh=" << veh->getID() << " lane=" << veh->getLane()->getID() << " pos=" << veh->getPositionOnLane(this) << "\n";
        }
#endif
        if (veh != ego && veh->getPositionOnLane(this) <= ego->getPositionOnLane()) {
            //const SUMOReal latOffset = veh->getLane()->getRightSideOnEdge() - getRightSideOnEdge();
            const SUMOReal latOffset = veh->getLatOffset(this);
            const SUMOReal dist = ego->getBackPositionOnLane() - veh->getPositionOnLane(this) - veh->getVehicleType().getMinGap();
            result.addFollower(veh, ego, dist, latOffset);
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(ego)) {
                std::cout << "  (1) added veh=" << veh->getID() << " latOffset=" << latOffset << " result=" << result.toString() << "\n";
            }
#endif
        }
    }
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(ego)) {
        std::cout << "  result.numFreeSublanes=" << result.numFreeSublanes() << "\n";
    }
#endif
    if (result.numFreeSublanes() > 0) {
        const SUMOReal backOffset = ego->getBackPositionOnLane(ego->getLane());
        // do a tree search among all follower lanes and check for the most
        // important vehicle (the one requiring the largest reargap)
        // to get a safe bound on the necessary search depth, we need to consider the maximum speed and minimum
        // deceleration of potential follower vehicles
        SUMOReal dist = getMaximumBrakeDist() - backOffset;

        std::set<MSLane*> visited;
        std::vector<MSLane::IncomingLaneInfo> newFound;
        std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
        while (toExamine.size() != 0) {
            for (std::vector<MSLane::IncomingLaneInfo>::iterator it = toExamine.begin(); it != toExamine.end(); ++it) {
                MSLane* next = (*it).lane;
                dist = MAX2(dist, next->getMaximumBrakeDist() - backOffset);
                MSLeaderInfo first = next->getFirstVehicleInformation(0, 0, false);
                MSLeaderInfo firstFront = next->getFirstVehicleInformation(0, 0, true);
#ifdef DEBUG_CONTEXT
                if (DEBUG_COND2(ego)) {
                    std::cout << "   next=" << next->getID() << " first=" << first.toString() << " firstFront=" << firstFront.toString() << "\n";
                }
#endif
                for (int i = 0; i < first.numSublanes(); ++i) {
                    const MSVehicle* v = first[i];
                    SUMOReal agap = 0;
                    if (v != 0 && v != ego) {
                        if (!v->isFrontOnLane(next)) {
                            // the front of v is already on divergent trajectory from the ego vehicle
                            // for which this method is called (in the context of MSLaneChanger).
                            // Therefore, technically v is not a follower but only an obstruction and
                            // the gap is not between the front of v and the back of ego
                            // but rather between the flank of v and the back of ego.
                            agap = (*it).length - next->getLength() + backOffset
                                   /// XXX dubious term. here for backwards compatibility
                                   - v->getVehicleType().getMinGap();
                            if (agap > 0) {
                                // Only if ego overlaps we treat v as if it were a real follower
                                // Otherwise we ignore it and look for another follower
                                v = firstFront[i];
                                if (v != 0 && v != ego) {
                                    agap = (*it).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                                } else {
                                    v = 0;
                                }
                            }
                        } else {
                            agap = (*it).length - v->getPositionOnLane() + backOffset - v->getVehicleType().getMinGap();
                        }
                        result.addFollower(v, ego, agap, 0, i);
#ifdef DEBUG_CONTEXT
                        if (DEBUG_COND2(ego)) {
                            std::cout << " (2) added veh=" << Named::getIDSecure(v) << " agap=" << agap << " next=" << next->getID() << " result=" << result.toString() << "\n";
                        }
#endif
                    }
                }
                if ((*it).length < dist) {
                    const std::vector<MSLane::IncomingLaneInfo>& followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j = followers.begin(); j != followers.end(); ++j) {
                        if (visited.find((*j).lane) == visited.end()) {
                            visited.insert((*j).lane);
                            MSLane::IncomingLaneInfo ili;
                            ili.lane = (*j).lane;
                            ili.length = (*j).length + (*it).length;
                            ili.viaLink = (*j).viaLink;
                            newFound.push_back(ili);
                        }
                    }
                }
            }
            toExamine.clear();
            swap(newFound, toExamine);
        }
        //return result;

    }
    return result;
}


void
MSLane::getLeadersOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle* ego,
                                const std::vector<MSLane*>& bestLaneConts, MSLeaderDistanceInfo& result) const {
    if (seen > dist) {
        return;
    }
    // check partial vehicles (they might be on a different route and thus not
    // found when iterating along bestLaneConts)
    for (VehCont::const_iterator it = myPartialVehicles.begin(); it != myPartialVehicles.end(); ++it) {
        MSVehicle* veh = *it;
        if (!veh->isFrontOnLane(this)) {
            result.addLeader(veh, seen);
        } else {
            break;
        }
    }
    const MSLane* nextLane = this;
    int view = 1;
    SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(seen / MAX2(speed, NUMERICAL_EPS));
    // loop over following lanes
    while (seen < dist && result.numFreeSublanes() > 0) {
        // get the next link used
        MSLinkCont::const_iterator link = succLinkSec(*ego, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->opened(arrivalTime, speed, speed, ego->getVehicleType().getLength(),
                ego->getImpatience(), ego->getCarFollowModel().getMaxDecel(), 0, ego->getLateralPositionOnLane()) || (*link)->haveRed()) {
            break;
        }
#ifdef HAVE_INTERNAL_LANES
        // check for link leaders
        const MSLink::LinkLeaders linkLeaders = (*link)->getLeaderInfo(ego, seen);
        if (linkLeaders.size() > 0) {
            const MSLink::LinkLeader ll = linkLeaders[0];
            if (ll.vehAndGap.first != 0) {
                // add link leader to all sublanes and return
                for (int i = 0; i < result.numSublanes(); ++i) {
                    MSVehicle* veh = ll.vehAndGap.first;
                    result.addLeader(veh, ll.vehAndGap.second, 0);
                }
                return; ;
            } // XXX else, deal with pedestrians
        }
        bool nextInternal = (*link)->getViaLane() != 0;
#endif
        nextLane = (*link)->getViaLaneOrLane();
        if (nextLane == 0) {
            break;
        }

        MSLeaderInfo leaders = nextLane->getLastVehicleInformation(0, 0, 0, false);
#ifdef DEBUG_CONTEXT
        if (DEBUG_COND2(ego)) {
            std::cout << SIMTIME << " getLeadersOnConsecutive lane=" << getID() << " nextLane=" << nextLane->getID() << " leaders=" << leaders.toString() << "\n";
        }
#endif
        // @todo check alignment issues if the lane width changes
        const int iMax = MIN2(leaders.numSublanes(), result.numSublanes());
        for (int i = 0; i < iMax; ++i) {
            const MSVehicle* veh = leaders[i];
            if (veh != 0) {
#ifdef DEBUG_CONTEXT
                if (DEBUG_COND2(ego)) std::cout << "   lead=" << veh->getID()
                                                    << " seen=" << seen
                                                    << " minGap=" << ego->getVehicleType().getMinGap()
                                                    << " backPos=" << veh->getBackPositionOnLane(nextLane)
                                                    << " gap=" << seen - ego->getVehicleType().getMinGap() + veh->getBackPositionOnLane(nextLane)
                                                    << "\n";
#endif
                result.addLeader(veh, seen - ego->getVehicleType().getMinGap() + veh->getBackPositionOnLane(nextLane), 0, i);
            }
        }

        if (nextLane->getVehicleMaxSpeed(ego) < speed) {
            dist = ego->getCarFollowModel().brakeGap(nextLane->getVehicleMaxSpeed(ego));
        }
        seen += nextLane->getLength();
        if (seen <= dist) {
            // delaying the update of arrivalTime and making it conditional to avoid possible integer overflows
            arrivalTime += TIME2STEPS(nextLane->getLength() / MAX2(speed, NUMERICAL_EPS));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    }
}



MSVehicle*
MSLane::getPartialBehind(const MSVehicle* ego) const {
    for (VehCont::const_reverse_iterator i = myPartialVehicles.rbegin(); i != myPartialVehicles.rend(); ++i) {
        MSVehicle* veh = *i;
        if (veh->isFrontOnLane(this)
                && veh != ego
                && veh->getPositionOnLane() <= ego->getPositionOnLane()) {
#ifdef DEBUG_CONTEXT
            if (DEBUG_COND2(ego)) {
                std::cout << SIMTIME << " getPartialBehind lane=" << getID() << " ego=" << ego->getID() << " found=" << veh->getID() << "\n";
            }
#endif
            return veh;
        }
    }
#ifdef DEBUG_CONTEXT
    if (DEBUG_COND2(ego)) {
        std::cout << SIMTIME << " getPartialBehind lane=" << getID() << " ego=" << ego->getID() << " nothing found. partials=" << toString(myPartialVehicles) << "\n";
    }
#endif
    return 0;
}


MSLane*
MSLane::getOpposite() const {
    if (myNeighs.size() == 1) {
        return dictionary(myNeighs[0]);
    }
    return 0;
}


SUMOReal
MSLane::getOppositePos(SUMOReal pos) const {
    MSLane* opposite = getOpposite();
    if (opposite == 0) {
        assert(false);
        throw ProcessError("Lane '" + getID() + "' cannot compute oppositePos as there is no opposite lane.");
    }
    // XXX transformations for curved geometries
    return MAX2((SUMOReal)0, opposite->getLength() - pos);

}

std::pair<MSVehicle* const, SUMOReal>
MSLane::getFollower(const MSVehicle* ego, SUMOReal egoPos, SUMOReal dist, bool ignoreMinorLinks) const {
    for (AnyVehicleIterator first = anyVehiclesUpstreamBegin(); first != anyVehiclesUpstreamEnd(); ++first) {
        // XXX refactor leaderInfo to use a const vehicle all the way through the call hierarchy
        MSVehicle* pred = (MSVehicle*)*first;
#ifdef DEBUG_CONTEXT
        if (DEBUG_COND2(ego)) {
            std::cout << "   getFollower lane=" << getID() << " egoPos=" << egoPos << " pred=" << pred->getID() << " predPos=" << pred->getPositionOnLane(this) << "\n";
        }
#endif
        if (pred->getPositionOnLane(this) < egoPos && pred != ego) {
            return std::pair<MSVehicle* const, SUMOReal>(pred, egoPos - pred->getPositionOnLane(this) - ego->getVehicleType().getLength() - pred->getVehicleType().getMinGap());
        }
    }
    return getFollowerOnConsecutive(
               egoPos - ego->getVehicleType().getLength(),
               ego->getSpeed(), ego->getCarFollowModel().getMaxDecel(), dist, ignoreMinorLinks);
}

std::pair<MSVehicle* const, SUMOReal>
MSLane::getOppositeLeader(const MSVehicle* ego, SUMOReal dist, bool oppositeDir) const {
#ifdef DEBUG_OPPOSITE
    if (DEBUG_COND2(ego)) std::cout << SIMTIME << " getOppositeLeader lane=" << getID()
                                        << " ego=" << ego->getID()
                                        << " pos=" << ego->getPositionOnLane()
                                        << " posOnOpposite=" << getOppositePos(ego->getPositionOnLane())
                                        << " dist=" << dist
                                        << " oppositeDir=" << oppositeDir
                                        << "\n";
#endif
    if (!oppositeDir) {
        return getLeader(ego, getOppositePos(ego->getPositionOnLane()), ego->getBestLanesContinuation(this));
    } else {
        const SUMOReal egoLength = ego->getVehicleType().getLength();
        const SUMOReal egoPos = ego->getLaneChangeModel().isOpposite() ? ego->getPositionOnLane() : getOppositePos(ego->getPositionOnLane());
        std::pair<MSVehicle* const, SUMOReal> result = getFollower(ego, egoPos + egoLength, dist, true);
        result.second -= ego->getVehicleType().getMinGap();
        return result;
    }
}


std::pair<MSVehicle* const, SUMOReal>
MSLane::getOppositeFollower(const MSVehicle* ego) const {
#ifdef DEBUG_OPPOSITE
    if (DEBUG_COND2(ego)) std::cout << SIMTIME << " getOppositeFollower lane=" << getID()
                                        << " ego=" << ego->getID()
                                        << " backPos=" << ego->getBackPositionOnLane()
                                        << " posOnOpposite=" << getOppositePos(ego->getBackPositionOnLane())
                                        << "\n";
#endif
    if (ego->getLaneChangeModel().isOpposite()) {
        std::pair<MSVehicle* const, SUMOReal> result = getFollower(ego, getOppositePos(ego->getPositionOnLane()), -1, true);
        return result;
    } else {
        std::pair<MSVehicle* const, SUMOReal> result = getLeader(ego, getOppositePos(ego->getPositionOnLane() - ego->getVehicleType().getLength()), std::vector<MSLane*>());
        if (result.second > 0) {
            // follower can be safely ignored since it is going the other way
            return std::make_pair(static_cast<MSVehicle*>(0), -1);
        } else {
            return result;
        }
    }
}


void
MSLane::initCollisionOptions(const OptionsCont& oc) {
    const std::string action = oc.getString("collision.action");
    if (action == "none") {
        myCollisionAction = COLLISION_ACTION_NONE;
    } else if (action == "warn") {
        myCollisionAction = COLLISION_ACTION_WARN;
    } else if (action == "teleport") {
        myCollisionAction = COLLISION_ACTION_TELEPORT;
    } else if (action == "remove") {
        myCollisionAction = COLLISION_ACTION_REMOVE;
    } else {
        throw ProcessError("Invalid collision.action '" + action + "'.");
    }
    myCheckJunctionCollisions = oc.getBool("collision.check-junctions");
}


void
MSLane::setPermissions(SVCPermissions permissions, long transientID) {
    if (transientID == CHANGE_PERMISSIONS_PERMANENT) {
        myPermissions = permissions;
        myOriginalPermissions = permissions;
    } else {
        myPermissionChanges[transientID] = permissions;
        resetPermissions(CHANGE_PERMISSIONS_PERMANENT);
    }
}


void
MSLane::resetPermissions(long transientID) {
    myPermissionChanges.erase(transientID);
    if (myPermissionChanges.empty()) {
        myPermissions = myOriginalPermissions;
    } else {
        // combine all permission changes
        myPermissions = SVCAll;
        for (std::map<long, SVCPermissions>::iterator it = myPermissionChanges.begin(); it != myPermissionChanges.end(); ++it) {
            myPermissions &= it->second;
        }
    }
}

/****************************************************************************/

