/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
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
MSLane::MSLane(const std::string &id, SUMOReal maxSpeed, SUMOReal length, MSEdge * const edge,
               unsigned int numericalID, const Position2DVector &shape,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed) throw()
        : myShape(shape), myID(id), myNumericalID(numericalID),
        myVehicles(), myLength(length), myEdge(edge), myMaxSpeed(maxSpeed),
        myAllowedClasses(allowed), myNotAllowedClasses(disallowed),
        myVehicleLengthSum(0), myInlappingVehicleEnd(10000), myInlappingVehicle(0) {
}


MSLane::~MSLane() throw() {
    for (MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); ++i) {
        delete *i;
    }
}


void
MSLane::initialize(MSLinkCont* links) {
    myLinks = *links;
    delete links;
}


// ------ interaction with MSMoveReminder ------
void
MSLane::addMoveReminder(MSMoveReminder* rem) throw() {
    myMoveReminders.push_back(rem);
}



// ------ Vehicle emission ------
bool
MSLane::freeEmit(MSVehicle& veh, SUMOReal mspeed) throw() {
    bool adaptableSpeed = true;
    if (myVehicles.size()==0) {
        if (isEmissionSuccess(&veh, mspeed, 0, adaptableSpeed)) {
            return true;
        }
    } else {
        // check whether the vehicle can be put behind the last one if there is such
        MSVehicle *leader = *myVehicles.begin();
        SUMOReal leaderPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
        SUMOReal speed = mspeed;
        if (adaptableSpeed) {
            speed = leader->getSpeed();
        }
		SUMOReal frontGapNeeded = veh.getCarFollowModel().getSecureGap(speed, leader->getCarFollowModel().getSpeedAfterMaxDecel(leader->getSpeed()));
        if (leaderPos-frontGapNeeded>=0) {
            SUMOReal tspeed = MIN2(veh.getCarFollowModel().ffeV(&veh, mspeed, frontGapNeeded, leader->getSpeed()), mspeed);
            // check whether we can emit in behind the last vehicle on the lane
            if (isEmissionSuccess(&veh, tspeed, 0, adaptableSpeed)) {
                return true;
            }
        }
    }
    // go through the lane, look for free positions (starting after the last vehicle)
    MSLane::VehCont::iterator predIt = myVehicles.begin();
    while (predIt!=myVehicles.end()) {
        // get leader (may be zero) and follower
        const MSVehicle *leader = predIt!=myVehicles.end()-1 ? *(predIt+1) : getPartialOccupator();
        const MSVehicle *follower = *predIt;

        // patch speed if allowed
        SUMOReal speed = mspeed;
        if (adaptableSpeed&&leader!=0) {
            speed = MIN2(leader->getSpeed(), mspeed);
        }

        // compute the space needed to not collide with leader
        SUMOReal frontMax = getLength();
        if (leader!=0) {
            SUMOReal leaderRearPos = leader->getPositionOnLane() - leader->getVehicleType().getLength();
            if (leader == getPartialOccupator()) {
                leaderRearPos = getPartialOccupatorEnd();
            }
            frontMax = leaderRearPos - veh.getCarFollowModel().getSecureGap(speed, leader->getCarFollowModel().getSpeedAfterMaxDecel(leader->getSpeed()));
        }
        // compute the space needed to not let the follower collide
        const SUMOReal followPos = follower->getPositionOnLane();
        const SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), veh.getCarFollowModel().getSpeedAfterMaxDecel(speed));
        const SUMOReal backMin = followPos + backGapNeeded + veh.getVehicleType().getLength();

        // check whether there is enough room (given some extra space for rounding errors)
        if (frontMax>0 && backMin+POSITION_EPS<frontMax) {
            // try emit vehicle (should be always ok)
            if (isEmissionSuccess(&veh, speed, backMin+POSITION_EPS, adaptableSpeed)) {
                return true;
            }
        }
        ++predIt;
    }
    // first check at lane's begin
    return false;
}


bool
MSLane::emit(MSVehicle& veh) throw() {
    SUMOReal pos = 0;
    SUMOReal speed = 0;
    bool patchSpeed = true; // whether the speed shall be adapted to infrastructure/traffic in front

    // determine the speed
    const SUMOVehicleParameter &pars = veh.getParameter();
    switch (pars.departSpeedProcedure) {
    case DEPART_SPEED_GIVEN:
        speed = pars.departSpeed;
        patchSpeed = false;
        break;
    case DEPART_SPEED_RANDOM:
        speed = RandHelper::rand(MIN2(veh.getMaxSpeed(), getMaxSpeed()));
        patchSpeed = true; // !!!(?)
        break;
    case DEPART_SPEED_MAX:
        speed = MIN2(veh.getMaxSpeed(), getMaxSpeed());
        patchSpeed = true; // !!!(?)
        break;
    case DEPART_SPEED_DEFAULT:
    default:
        // speed = 0 was set before
        patchSpeed = false; // !!!(?)
        break;
    }

    // determine the position
    switch (pars.departPosProcedure) {
    case DEPART_POS_GIVEN:
        if (pars.departPos >= 0.) {
            pos = pars.departPos;
        } else {
            pos = pars.departPos + getLength();
        }
        break;
    case DEPART_POS_RANDOM:
        pos = RandHelper::rand(getLength());
        break;
    case DEPART_POS_RANDOM_FREE: {
        for (unsigned int i=0; i < 10; i++) {
            // we will try some random positions ...
            pos = RandHelper::rand(getLength());
            if (isEmissionSuccess(&veh, speed, pos, patchSpeed)) {
                return true;
            }
        }
        // ... and if that doesn't work, we put the vehicle to the free position
        return freeEmit(veh, speed);
    }
    break;
    case DEPART_POS_FREE:
        return freeEmit(veh, speed);
    case DEPART_POS_DEFAULT:
    default:
        // pos = 0 was set before
        break;
    }

    // try to emit
    return isEmissionSuccess(&veh, speed, pos, patchSpeed);
}


bool
MSLane::isEmissionSuccess(MSVehicle* aVehicle,
                          SUMOReal speed, SUMOReal pos,
                          bool patchSpeed) throw() {
    //  and the speed is not too high (vehicle should decelerate)
    // try to get a leader on consecutive lanes
    //  we have to do this even if we have found a leader on our lane because it may
    //  be driving into another direction
    aVehicle->getBestLanes(true, this);
	const MSCFModel &cfModel = aVehicle->getCarFollowModel();
    const std::vector<MSLane*> &bestLaneConts = aVehicle->getBestLanesContinuation(this);
    std::vector<MSLane*>::const_iterator ri = bestLaneConts.begin();
    SUMOReal seen = getLength() - pos;
    SUMOReal dist = cfModel.brakeGap(speed);
    const MSRoute &r = aVehicle->getRoute();
    MSRouteIterator ce = r.begin();
    MSLane *currentLane = this;
    MSLane *nextLane = this;
    while (seen<dist&&ri!=bestLaneConts.end()&&nextLane!=0/*&&ce!=r.end()*/) {
        // get the next link used...
        MSLinkCont::const_iterator link = currentLane->succLinkSec(*aVehicle, 1, *currentLane, bestLaneConts);
        // ...and the next used lane (including internal)
        if (!currentLane->isLinkEnd(link) && (*link)->havePriority() && (*link)->getState()!=MSLink::LINKSTATE_TL_RED) { // red may have priority?
#ifdef HAVE_INTERNAL_LANES
            bool nextInternal = false;
            nextLane = (*link)->getViaLane();
            if (nextLane==0) {
                nextLane = (*link)->getLane();
            } else {
                nextInternal = true;
            }
#else
            nextLane = (*link)->getLane();
#endif
        } else {
            nextLane = 0;
        }
        // check how next lane effects the journey
        if (nextLane!=0) {
            SUMOReal gap = 0;
            MSVehicle * leader = currentLane->getPartialOccupator();
            if (leader!=0) {
                gap = getPartialOccupatorEnd();
            } else {
                // check leader on next lane
                leader = nextLane->getLastVehicle();
                if (leader!=0) {
                    gap = seen+leader->getPositionOnLane()-leader->getVehicleType().getLength();
                }
            }
            if (leader!=0) {
                SUMOReal nspeed = gap>=0 ? cfModel.ffeV(aVehicle, speed, gap, leader->getSpeed()) : 0;
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = cfModel.brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            }
            // check next lane's maximum velocity
            SUMOReal nspeed = nextLane->getMaxSpeed();
            if (nspeed<speed) {
                // patch speed if needed
                if (patchSpeed) {
                    speed = MIN2(cfModel.ffeV(aVehicle, speed, seen, nspeed), speed);
                    dist = cfModel.brakeGap(speed);
                } else {
                    // we may not drive with the given velocity - we would be too fast on the next lane
                    return false;
                }
            }
            // check traffic on next junctions
			SUMOTime arrivalTime = MSNet::getInstance()->getCurrentTimeStep() + seen / speed;
#ifdef HAVE_INTERNAL_LANES
		    SUMOTime leaveTime = (*link)->getViaLane()==0 ? arrivalTime + (*link)->getLength() * speed : arrivalTime + (*link)->getViaLane()->getLength() * speed;
#else
		    SUMOTime leaveTime = arrivalTime + (*link)->getLength() * speed;
#endif
            if ((*link)->hasApproachingFoe(arrivalTime, leaveTime)) {
                SUMOReal nspeed = cfModel.ffeV(aVehicle, speed, seen, 0);
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = cfModel.brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            } else {
                // we can only drive to the end of the current lane...
                SUMOReal nspeed = cfModel.ffeV(aVehicle, speed, seen, 0);
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = cfModel.brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            }
            seen += nextLane->getLength();
            ++ce;
            ++ri;
            currentLane = nextLane;
        }
    }
    if (seen<dist) {
        SUMOReal nspeed = cfModel.ffeV(aVehicle, speed, seen, 0);
        if (nspeed<speed) {
            if (patchSpeed) {
                speed = MIN2(nspeed, speed);
                dist = cfModel.brakeGap(speed);
            } else {
                // we may not drive with the given velocity - we crash into the leader
                MsgHandler::getErrorInstance()->inform("Vehicle '" + aVehicle->getID() + "' will not be able to emit using given velocity!");
                // !!! we probably should do something else...
                return false;
            }
        }
    }

    // get the pointer to the vehicle next in front of the given position
    MSLane::VehCont::iterator predIt =
        find_if(myVehicles.begin(), myVehicles.end(), bind2nd(VehPosition(), pos));
    if (predIt != myVehicles.end()) {
        // ok, there is one (a leader)
        MSVehicle* leader = *predIt;
        SUMOReal frontGapNeeded = aVehicle->getCarFollowModel().getSecureGap(speed, leader->getCarFollowModel().getSpeedAfterMaxDecel(leader->getSpeed()));
        SUMOReal gap = MSVehicle::gap(leader->getPositionOnLane(), leader->getVehicleType().getLength(), pos);
        if (gap<frontGapNeeded) {
            // too close to the leader on this lane
            return false;
        }
    }

    // check back vehicle
    if (predIt!=myVehicles.begin()) {
        // there is direct follower on this lane
        MSVehicle *follower = *(predIt-1);
        SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), aVehicle->getCarFollowModel().getSpeedAfterMaxDecel(speed));
        SUMOReal gap = MSVehicle::gap(pos, aVehicle->getVehicleType().getLength(), follower->getPositionOnLane());
        if (gap<backGapNeeded) {
            // too close to the follower on this lane
            return false;
        }
    } else {
        // check approaching vehicle (consecutive follower)
        SUMOReal lspeed = getMaxSpeed();
        // in order to look back, we'd need the minimum braking ability of vehicles in the net...
        //  we'll assume it to be 4m/s^2
        //   !!!revisit
        SUMOReal dist = lspeed * lspeed * SUMOReal(1./2.*4.) + SPEED2DIST(lspeed);
        std::pair<const MSVehicle * const, SUMOReal> approaching = getFollowerOnConsecutive(dist, 0, speed, pos - aVehicle->getVehicleType().getLength());
        if (approaching.first!=0) {
            const MSVehicle *const follower = approaching.first;
            SUMOReal backGapNeeded = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), aVehicle->getCarFollowModel().getSpeedAfterMaxDecel(speed));
            SUMOReal gap = approaching.second - pos - aVehicle->getVehicleType().getLength();
            if (gap<backGapNeeded) {
                // too close to the consecutive follower
                return false;
            }
        }
    }

    // may got negative while adaptation
    if (speed<0) {
        return false;
    }
    // enter
    aVehicle->enterLaneAtEmit(this, pos, speed);
    bool wasInactive = myVehicles.size()==0;
    if (predIt==myVehicles.end()) {
        // vehicle will be the first on the lane
        myVehicles.push_back(aVehicle);
    } else {
        myVehicles.insert(predIt, aVehicle);
    }
    myVehicleLengthSum += aVehicle->getVehicleType().getLength();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
    return true;
}


// ------ Handling vehicles lapping into lanes ------
SUMOReal
MSLane::setPartialOccupation(MSVehicle *v, SUMOReal leftVehicleLength) throw() {
    myInlappingVehicle = v;
    if (leftVehicleLength>myLength) {
        myInlappingVehicleEnd = 0;
    } else {
        myInlappingVehicleEnd = myLength-leftVehicleLength;
    }
    return myLength;
}


void
MSLane::resetPartialOccupation(MSVehicle *v) throw() {
    if (v==myInlappingVehicle) {
        myInlappingVehicleEnd = 10000;
    }
    myInlappingVehicle = 0;
}


std::pair<MSVehicle*, SUMOReal>
MSLane::getLastVehicleInformation() const throw() {
    if (myVehicles.size()!=0) {
        // the last vehicle is the one in scheduled by this lane
        MSVehicle *last = *myVehicles.begin();
		SUMOReal pos = MAX2(SUMOReal(0), last->getPositionOnLane()-last->getVehicleType().getLength());
        return std::make_pair(last, pos);
    }
    if (myInlappingVehicle!=0) {
        // the last one is a vehicle extending into this lane
        return std::make_pair(myInlappingVehicle, myInlappingVehicleEnd);
    }
    return std::make_pair<MSVehicle*, SUMOReal>(0, 0);
}


// ------  ------
bool
MSLane::moveCritical(SUMOTime t) {
    myLeftVehLength = myVehicleLengthSum;
    assert(myVehicles.size()!=0);
    std::vector<MSVehicle*> collisions;
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;
    // Move all next vehicles beside the first
    for (veh=myVehicles.begin(); veh != lastBeforeEnd;) {
        myLeftVehLength -= (*veh)->getVehicleType().getLength();
        VehCont::const_iterator pred(veh + 1);
        if ((*veh)->moveRegardingCritical(t, this, *pred, 0, myLeftVehLength)) {
            collisions.push_back(*veh);
        }
        ++veh;
    }
    myLeftVehLength -= (*veh)->getVehicleType().getLength();
    if ((*veh)->moveRegardingCritical(t, this, 0, 0, myLeftVehLength)) {
        collisions.push_back(*veh);
    }
    assert((*veh)->getPositionOnLane() <= myLength);
    assert(&(*veh)->getLane()==this);
    // deal with collisions
    for (std::vector<MSVehicle*>::iterator i=collisions.begin(); i!=collisions.end(); ++i) {
        MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + (*i)->getID() + "'; collision, lane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
		myVehicleLengthSum -= (*i)->getVehicleType().getLength();
        MSVehicleTransfer::getInstance()->addVeh((*i));
        myVehicles.erase(find(myVehicles.begin(), myVehicles.end(), *i));
    }
    return myVehicles.size()==0;
}


void
MSLane::detectCollisions(SUMOTime) {
    if (myVehicles.size() < 2) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin(); veh != lastVeh;) {
        VehCont::iterator pred = veh + 1;
        SUMOReal gap = (*pred)->getPositionOnLane() - (*pred)->getVehicleType().getLength() - (*veh)->getPositionOnLane();
        if (gap < 0) {
            MSVehicle *vehV = *veh;
            MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + vehV->getID() + "'; collision, lane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSVehicleTransfer::getInstance()->addVeh(vehV);
            veh = myVehicles.erase(veh); // remove current vehicle
            lastVeh = myVehicles.end() - 1;
            myVehicleLengthSum -= (*veh)->getVehicleType().getLength();
            if (veh==myVehicles.end()) {
                break;
            }
        } else {
            ++veh;
        }
    }
}


SUMOReal
getMaxSpeedRegardingNextLanes(MSVehicle& veh, SUMOReal speed, SUMOReal pos) {
    MSRouteIterator next = veh.getRoute().begin();
	const MSCFModel &cfModel = veh.getCarFollowModel();
    MSLane *currentLane = (*next)->getLanes()[0];
    SUMOReal seen = currentLane->getLength() - pos;
    SUMOReal dist = SPEED2DIST(speed) + cfModel.brakeGap(speed);
    SUMOReal tspeed = speed;
    while (seen<dist&&next!=veh.getRoute().end()-1) {
        ++next;
        MSLane *nextLane = (*next)->getLanes()[0];
        tspeed = MIN2(cfModel.ffeV(&veh, tspeed, seen, nextLane->getMaxSpeed()), nextLane->getMaxSpeed());
        dist = SPEED2DIST(tspeed) + cfModel.brakeGap(tspeed);
        seen += nextLane->getMaxSpeed();
    }
    return tspeed;
}


bool
MSLane::setCritical(SUMOTime t, std::vector<MSLane*> &into) {
    // move critical vehicles
    int first2pop = -1;
    int curr = 0;
    bool hadProblem = false;
    VehCont::iterator i;
    for (i=myVehicles.begin(); i!=myVehicles.end(); ++i, ++curr) {
        (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if (target!=0&&first2pop<0) {
            first2pop = curr;
        }
    }
    if (first2pop>=0) {
        const int remove = (int)myVehicles.size() - first2pop;
        for (int j = 0; j<remove; ++j) {
            MSVehicle *v = *(myVehicles.end() - 1);
            MSVehicle *p = pop(t);
            assert(v==p);
            MSLane *target = p->getTargetLane();
            if (target==0||p->getPositionOnLane()>target->getLength()) {
                if (target==0) {
                    MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + v->getID() + "'; false leaving order, targetLane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                } else if (p->getPositionOnLane()>target->getLength()) {
                    MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + v->getID() + "'; beyond lane (1), targetLane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                }
                MSVehicleTransfer::getInstance()->addVeh(v);
                hadProblem = true;
                continue;
            }
            if (target!=0&&p->isOnRoad()) {
                target->push(p);
                into.push_back(target);
            }
        }
    }
    if (myVehicles.size()>0) {
        if (MSGlobals::gTimeToGridlock>0
                && !(*(myVehicles.end()-1))->isStopped()
                &&
                (*(myVehicles.end()-1))->getWaitingTime()>MSGlobals::gTimeToGridlock) {

            MSVehicleTransfer *vt = MSVehicleTransfer::getInstance();
            MSVehicle *veh = removeFirstVehicle();
            MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + veh->getID() + "'; waited too long, lane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            vt->addVeh(veh);
        }
    }
    // check for vehicle removal
    for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end();) {
        MSVehicle *vehV = *veh;
        if (vehV->getPositionOnLane()>getLength()) {
            MsgHandler::getWarningInstance()->inform("Teleporting vehicle '" + vehV->getID() + "'; beyond lane (2), targetLane='" + getID() + "', time=" + toString(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSVehicleTransfer::getInstance()->addVeh(vehV);
            veh = myVehicles.erase(veh); // remove current vehicle
        } else if (vehV->ends()) {
            myVehicleLengthSum -= vehV->getVehicleType().getLength();
            vehV->onRemovalFromNet(false);
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(vehV);
            veh = myVehicles.erase(veh); // remove current vehicle
        } else {
            ++veh;
        }
    }
    return myVehicles.size()==0;
}


bool
MSLane::dictionary(std::string id, MSLane* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSLane*
MSLane::dictionary(std::string id) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSLane::clear() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSLane::insertIDs(std::vector<std::string> &into) throw() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


bool
MSLane::push(MSVehicle* veh) {
    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    // check whether the vehicle has ended his route
    // Add to mean data (edge/lane state dump)
    if (! veh->moveRoutePointer(myEdge)) {     // adjusts vehicles routeIterator
        myVehBuffer.push_back(veh);
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        return false;
    } else {
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        veh->onRemovalFromNet(false);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
}


MSVehicle*
MSLane::pop(SUMOTime) {
    assert(! myVehicles.empty());
    MSVehicle* first = myVehicles.back();
    first->leaveLaneAtMove(SPEED2DIST(first->getSpeed())/* - first->pos()*/);
    myVehicles.pop_back();
    myVehicleLengthSum -= first->getVehicleType().getLength();
    return first;
}


bool
MSLane::appropriate(const MSVehicle *veh) {
    if (myEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    MSLinkCont::const_iterator link = succLinkSec(*veh, 1, *this, veh->getBestLanesContinuation());
    return (link != myLinks.end());
}


bool
MSLane::integrateNewVehicle(SUMOTime) {
    bool wasInactive = myVehicles.size()==0;
    sort(myVehBuffer.begin(), myVehBuffer.end(), vehicle_position_sorter());
    for (std::vector<MSVehicle*>::const_iterator i=myVehBuffer.begin(); i!=myVehBuffer.end(); ++i) {
        MSVehicle *veh = *i;
        myVehicles.push_front(veh);
        myVehicleLengthSum += veh->getVehicleType().getLength();
    }
    myVehBuffer.clear();
    return wasInactive&&myVehicles.size()!=0;
}


bool
MSLane::isLinkEnd(MSLinkCont::const_iterator &i) const {
    return i==myLinks.end();
}


bool
MSLane::isLinkEnd(MSLinkCont::iterator &i) {
    return i==myLinks.end();
}


MSVehicle * const
    MSLane::getLastVehicle() const {
    if (myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}


const MSVehicle * const
    MSLane::getFirstVehicle() const {
    if (myVehicles.size()==0) {
        return 0;
    }
    return *(myVehicles.end()-1);
}


MSLinkCont::const_iterator
MSLane::succLinkSec(const SUMOVehicle& veh, unsigned int nRouteSuccs,
                    const MSLane& succLinkSource, const std::vector<MSLane*> &conts) const {
    const MSEdge* nRouteEdge = veh.succEdge(nRouteSuccs);
    // check whether the vehicle tried to look beyond its route
    if (nRouteEdge==0) {
        // return end (no succeeding link) if so
        return succLinkSource.myLinks.end();
    }
    // a link may be used if
    //  1) there is a destination lane ((*link)->getLane()!=0)
    //  2) the destination lane belongs to the next edge in route ((*link)->getLane()->myEdge == nRouteEdge)
    //  3) the destination lane allows the vehicle's class ((*link)->getLane()->allowsVehicleClass(veh.getVehicleClass()))

    // at first, we'll assume we have the continuations of our route in "conts" (built in "getBestLanes")
    //  "conts" stores the best continuations of our current lane
    MSLinkCont::const_iterator link;
    if (nRouteSuccs>0&&conts.size()>=nRouteSuccs&&nRouteSuccs>0) {
        // we go through the links in our list and return the matching one
        for (link=succLinkSource.myLinks.begin(); link!=succLinkSource.myLinks.end() ; ++link) {
            if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
                // we should use the link if it connects us to the best lane
                if ((*link)->getLane()==conts[nRouteSuccs-1]) {
                    return link;
                }
            }
        }
    }

    // ok, we were not able to use the conts for any reason
    //  we will now collect allowed links, at first
    // collect allowed links
    std::vector<MSLinkCont::const_iterator> valid;
    for (link=succLinkSource.myLinks.begin(); link!=succLinkSource.myLinks.end() ; ++link) {
        if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleType().getVehicleClass())) {
            valid.push_back(link);
        }
    }
    // if no valid link was found...
    if (valid.size()==0) {
        // ... return end (no succeeding link)
        return succLinkSource.myLinks.end();
    }
    // if there is only one valid link, let's use it...
    if (valid.size()==1) {
        return *(valid.begin());
    }
    // if the next edge is the route end, then we may return an arbitary link
    // also, if there is no allowed lane on the edge following the current one (recheck?)
    const MSEdge* nRouteEdge2 = veh.succEdge(nRouteSuccs+1);
    const std::vector<MSLane*> *next_allowed = nRouteEdge->allowedLanes(*nRouteEdge2, veh.getVehicleType().getVehicleClass());
    if (nRouteEdge2==0||next_allowed==0) {
        return *(valid.begin());
    }
    // now let's determine which link is the best
    // in fact, we do not know it, here...
    for (std::vector<MSLinkCont::const_iterator>::iterator i=valid.begin(); i!=valid.end(); ++i) {
        if (find(next_allowed->begin(), next_allowed->end(), (**i)->getLane())!=next_allowed->end()) {
            return *i;
        }
    }
    return *(valid.begin());
}



const MSLinkCont &
MSLane::getLinkCont() const {
    return myLinks;
}


void
MSLane::swapAfterLaneChange(SUMOTime) {
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
}




GUILaneWrapper *
MSLane::buildLaneWrapper(GUIGlObjectStorage &) {
    throw "Only within the gui-version";
}


void
MSLane::init(MSEdgeControl &, std::vector<MSLane*>::const_iterator firstNeigh, std::vector<MSLane*>::const_iterator lastNeigh) {
    myFirstNeigh = firstNeigh;
    myLastNeigh = lastNeigh;
}


MSVehicle *
MSLane::removeFirstVehicle() {
    MSVehicle *veh = *(myVehicles.end()-1);
    veh->leaveLaneAtMove(0);
    myVehicles.erase(myVehicles.end()-1);
    myVehicleLengthSum -= veh->getVehicleType().getLength();
    return veh;
}


MSVehicle *
MSLane::removeVehicle(MSVehicle * remVehicle) {
    for (MSLane::VehCont::iterator it = myVehicles.begin(); it < myVehicles.end(); it++) {
        if (remVehicle->getID() == (*it)->getID()) {
            remVehicle->leaveLane(true);
            myVehicles.erase(it);
            myVehicleLengthSum -= remVehicle->getVehicleType().getLength();
            break;
        }
    }
    return remVehicle;
}


MSLane * const
    MSLane::getLeftLane() const {
    return myEdge->leftLane(this);
}


MSLane * const
    MSLane::getRightLane() const {
    return myEdge->rightLane(this);
}


bool
MSLane::allowsVehicleClass(SUMOVehicleClass vclass) const {
    if (vclass==SVC_UNKNOWN) {
        return true;
    }
    if (myAllowedClasses.size()==0&&myNotAllowedClasses.size()==0) {
        return true;
    }
    if (find(myAllowedClasses.begin(), myAllowedClasses.end(), vclass)!=myAllowedClasses.end()) {
        return true;
    }
    if (myAllowedClasses.size()!=0) {
        return false;
    }
    if (find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), vclass)!=myNotAllowedClasses.end()) {
        return false;
    }
    return true;
}


void
MSLane::addIncomingLane(MSLane *lane, MSLink *viaLink) {
    IncomingLaneInfo ili;
    ili.lane = lane;
    ili.viaLink = viaLink;
    ili.length = lane->getLength();
    myIncomingLanes.push_back(ili);
}

class by_second_sorter {
public:
    inline int operator()(const std::pair<const MSVehicle * , SUMOReal> &p1, const std::pair<const MSVehicle * , SUMOReal> &p2) const {
        return p1.second<p2.second;
    }
};

std::pair<MSVehicle * const, SUMOReal>
MSLane::getFollowerOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal leaderSpeed, SUMOReal backOffset) const {
    // ok, a vehicle has not noticed the lane about itself;
    //  iterate as long as necessary to search for an approaching one
    std::set<MSLane*> visited;
    std::vector<std::pair<MSVehicle *, SUMOReal> > possible;
    std::vector<MSLane::IncomingLaneInfo> newFound;
    std::vector<MSLane::IncomingLaneInfo> toExamine = myIncomingLanes;
    while (toExamine.size()!=0) {
        for (std::vector<MSLane::IncomingLaneInfo>::iterator i=toExamine.begin(); i!=toExamine.end(); ++i) {
            if ((*i).viaLink->getState()==MSLink::LINKSTATE_TL_RED) {
                continue;
            }
            MSLane *next = (*i).lane;
            if (next->getFirstVehicle()!=0) {
                MSVehicle * v = (MSVehicle*) next->getFirstVehicle();
                SUMOReal agap = (*i).length - v->getPositionOnLane() + backOffset;
				if (!v->getCarFollowModel().hasSafeGap(v->getCarFollowModel().maxNextSpeed(v->getSpeed()), agap, leaderSpeed, v->getLane().getMaxSpeed())) {
                    possible.push_back(std::make_pair(v, (*i).length-v->getPositionOnLane()+seen));
                }
            } else {
                if ((*i).length+seen<dist) {
                    const std::vector<MSLane::IncomingLaneInfo> &followers = next->getIncomingLanes();
                    for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j=followers.begin(); j!=followers.end(); ++j) {
                        if (visited.find((*j).lane)==visited.end()) {
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
    if (possible.size()==0) {
        return std::pair<MSVehicle * const, SUMOReal>(0, -1);
    }
    sort(possible.begin(), possible.end(), by_second_sorter());
    return *(possible.begin());
}


std::pair<MSVehicle * const, SUMOReal>
MSLane::getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal speed, const MSVehicle &veh,
                               const std::vector<MSLane*> &bestLaneConts) const {
    if (seen>dist) {
        return std::pair<MSVehicle * const, SUMOReal>(0, -1);
    }
    unsigned int view = 1;
    // loop over following lanes
    const MSLane * targetLane = this;
    MSVehicle *leader = targetLane->getPartialOccupator();
    if (leader!=0) {
        return std::pair<MSVehicle * const, SUMOReal>(leader, seen-targetLane->getPartialOccupatorEnd());
    }
    const MSLane * nextLane = targetLane;
    while (true) {
        // get the next link used
        MSLinkCont::const_iterator link = targetLane->succLinkSec(veh, view, *nextLane, bestLaneConts);
        if (nextLane->isLinkEnd(link) || !(*link)->havePriority() || (*link)->getState()==MSLink::LINKSTATE_TL_RED) {
            return std::pair<MSVehicle * const, SUMOReal>(0, -1);
        }
#ifdef HAVE_INTERNAL_LANES
        bool nextInternal = false;
        nextLane = (*link)->getViaLane();
        if (nextLane==0) {
            nextLane = (*link)->getLane();
        } else {
            nextInternal = true;
        }
#else
        nextLane = (*link)->getLane();
#endif
        if (nextLane==0) {
            return std::pair<MSVehicle * const, SUMOReal>(0, -1);
        }
        MSVehicle * leader = nextLane->getLastVehicle();
        if (leader!=0) {
            return std::pair<MSVehicle * const, SUMOReal>(leader, seen+leader->getPositionOnLane()-leader->getVehicleType().getLength());
        } else {
            leader = nextLane->getPartialOccupator();
            if (leader!=0) {
                return std::pair<MSVehicle * const, SUMOReal>(leader, seen+nextLane->getPartialOccupatorEnd());
            }
        }
        if (nextLane->getMaxSpeed()<speed) {
            dist = veh.getCarFollowModel().brakeGap(nextLane->getMaxSpeed());
        }
        seen += nextLane->getLength();
        if (seen>dist) {
            return std::pair<MSVehicle * const, SUMOReal>(0, -1);
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


void
MSLane::leftByLaneChange(MSVehicle *v) {
    myVehicleLengthSum -= v->getVehicleType().getLength();
}


void
MSLane::enteredByLaneChange(MSVehicle *v) {
    myVehicleLengthSum += v->getVehicleType().getLength();
}


// ------------ Current state retrieval
SUMOReal
MSLane::getOccupancy() const throw() {
    return myVehicleLengthSum / myLength;
}


SUMOReal
MSLane::getVehLenSum() const throw() {
    return myVehicleLengthSum;
}


SUMOReal
MSLane::getMeanSpeed() const throw() {
    if (myVehicles.size()==0) {
        return myMaxSpeed;
    }
    SUMOReal v = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        v += (*i)->getSpeed();
    }
    SUMOReal ret = v / (SUMOReal) myVehicles.size();
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_CO2Emissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_CO2Emissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_COEmissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_COEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_PMxEmissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_PMxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_NOxEmissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_NOxEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_HCEmissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_HCEmissions();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHBEFA_FuelConsumption() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        ret += (*i)->getHBEFA_FuelConsumption();
    }
    releaseVehicles();
    return ret;
}


SUMOReal
MSLane::getHarmonoise_NoiseEmissions() const throw() {
    SUMOReal ret = 0;
    const MSLane::VehCont &vehs = getVehiclesSecure();
    if (vehs.size()==0) {
        releaseVehicles();
        return 0;
    }
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        SUMOReal sv = (*i)->getHarmonoise_NoiseEmissions();
        ret += (SUMOReal) pow(10., (sv/10.));
    }
    releaseVehicles();
    return HelpersHarmonoise::sum(ret);
}


/****************************************************************************/

