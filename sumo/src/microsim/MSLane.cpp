/****************************************************************************/
/// @file    MSLane.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
MSLane::DictType MSLane::myDict;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSLane::PosGreater
 * ----------------------------------------------------------------------- */
MSLane::PosGreater::result_type
MSLane::PosGreater::operator()(first_argument_type veh1,
                               second_argument_type veh2) const {
    return veh1->getPositionOnLane() > veh2->getPositionOnLane();
}


/* -------------------------------------------------------------------------
 * methods of MSLane
 * ----------------------------------------------------------------------- */
MSLane::MSLane(const std::string &id, SUMOReal maxSpeed, SUMOReal length, MSEdge * const edge,
               unsigned int numericalID, const Position2DVector &shape,
               const std::vector<SUMOVehicleClass> &allowed,
               const std::vector<SUMOVehicleClass> &disallowed) throw()
        : myLastState(10000, 10000), myShape(shape), myID(id), myNumericalID(numericalID),
        myVehicles(), myLength(length), myEdge(edge), myMaxSpeed(maxSpeed),
        myAllowedClasses(allowed), myNotAllowedClasses(disallowed),
        myFirstUnsafe(0), myVehicleLengthSum(0) {
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
        SUMOReal frontGapNeeded = veh.getSecureGap(speed, leader->getSpeed(), *leader);
        if (leaderPos-frontGapNeeded>=0) {
            SUMOReal tspeed = MIN2(veh.getVehicleType().ffeV(mspeed, frontGapNeeded, leader->getSpeed()), mspeed);
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
        MSVehicle *leader = predIt!=myVehicles.end()-1 ? *(predIt+1) : 0;
        MSVehicle *follower = *predIt;

        // patch speed if allowed
        SUMOReal speed = mspeed;
        if (adaptableSpeed&&leader!=0) {
            speed = MIN2(leader->getSpeed(), mspeed);
        }

        // compute the space needed to not collide with leader
        SUMOReal leaderPos = leader!=0 ? leader->getPositionOnLane() - leader->getVehicleType().getLength() : -1;
        SUMOReal frontGapNeeded = leader!=0 ? veh.getSecureGap(speed, leader->getSpeed(), *leader) : -1;
        SUMOReal frontMax = leader!=0 ? leaderPos - frontGapNeeded - leader->getVehicleType().getLength() : length();

        // compute the space needed to not let the follower collide
        SUMOReal followPos = follower->getPositionOnLane();
        SUMOReal backGapNeeded = follower->getSecureGap(follower->getSpeed(), speed, veh);
        SUMOReal backMin = followPos + backGapNeeded + veh.getVehicleType().getLength();

        // check whether there is enough room
        if (frontMax>0 && backMin<frontMax) {
            // try emit vehicle (should be always ok)
            if (isEmissionSuccess(&veh, speed, backMin, adaptableSpeed)) {
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
        speed = RandHelper::rand(MIN2(veh.getMaxSpeed(), maxSpeed()));
        patchSpeed = true; // !!!(?)
        break;
    case DEPART_SPEED_MAX:
        speed = MIN2(veh.getMaxSpeed(), maxSpeed());
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
            pos = pars.departPos + length();
        }
        break;
    case DEPART_POS_RANDOM:
        pos = RandHelper::rand(length());
        break;
    case DEPART_POS_RANDOM_FREE: {
        unsigned int i=0;
        // we will try many random positions ...
        while (i <= 10) {
            pos = RandHelper::rand(length());
            if (isEmissionSuccess(&veh, speed, pos, patchSpeed)) return true;
            else i++;
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
    const std::vector<MSLane*> &bestLaneConts = aVehicle->getBestLanesContinuation(this);
    std::vector<MSLane*>::const_iterator ri = bestLaneConts.begin();
    SUMOReal seen = length() - pos;
    SUMOReal dist = aVehicle->getVehicleType().brakeGap(speed);
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
            // check leader on next lane
            MSVehicle * leader = nextLane->getLastVehicle();
            if (leader!=0) {
                SUMOReal nspeed = aVehicle->ffeV(speed, seen+leader->getPositionOnLane()-leader->getVehicleType().getLength(), leader->getSpeed());
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = aVehicle->getVehicleType().brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            }
            // check next lane's maximum velocity
            SUMOReal nspeed = nextLane->maxSpeed();
            if (nspeed<speed) {
                // patch speed if needed
                if (patchSpeed) {
                    speed = MIN2(aVehicle->getVehicleType().ffeV(speed, seen, nspeed), speed);
                    dist = aVehicle->getVehicleType().brakeGap(speed);
                } else {
                    // we may not drive with the given velocity - we would be too fast on the next lane
                    return false;
                }
            }
            // check traffic on next junctions
            if ((*link)->hasApproachingFoe()) {
                SUMOReal nspeed = aVehicle->ffeV(speed, seen, 0);
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = aVehicle->getVehicleType().brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            } else {
                // we can only drive to the end of the current lane...
                SUMOReal nspeed = aVehicle->ffeV(speed, seen, 0);
                if (nspeed<speed) {
                    if (patchSpeed) {
                        speed = MIN2(nspeed, speed);
                        dist = aVehicle->getVehicleType().brakeGap(speed);
                    } else {
                        // we may not drive with the given velocity - we crash into the leader
                        return false;
                    }
                }
            }
            seen += nextLane->length();
            ++ce;
            ++ri;
            currentLane = nextLane;
        }
    }
    if (seen<dist) {
        SUMOReal nspeed = aVehicle->ffeV(speed, seen, 0);
        if (nspeed<speed) {
            if (patchSpeed) {
                speed = MIN2(nspeed, speed);
                dist = aVehicle->getVehicleType().brakeGap(speed);
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
        SUMOReal frontGapNeeded = aVehicle->getSecureGap(speed, leader->getSpeed(), *leader);
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
        SUMOReal backGapNeeded = follower->getSecureGap(follower->getSpeed(), speed, *aVehicle);
        SUMOReal gap = MSVehicle::gap(pos, aVehicle->getVehicleType().getLength(), follower->getPositionOnLane());
        if (gap<backGapNeeded) {
            // too close to the follower on this lane
            return false;
        }
    } else {
        // check approaching vehicle (consecutive follower)
        SUMOReal speed = maxSpeed();
        // in order to look back, we'd need the minimum breaking ability of vehicles in the net...
        //  we'll assume it to be 4m/s^2
        //   !!!revisit
        SUMOReal dist = speed * speed * SUMOReal(1./2.*4.) + SPEED2DIST(speed);
        std::pair<const MSVehicle * const, SUMOReal> approaching = getFollowerOnConsecutive(dist, 0, speed);
        if (approaching.first!=0) {
            const MSVehicle *const follower = approaching.first;
            SUMOReal backGapNeeded = follower->getSecureGap(follower->getSpeed(), speed, *aVehicle);
            SUMOReal gap = approaching.second - pos - aVehicle->getVehicleType().getLength();
            if (gap<backGapNeeded) {
                // too close to the consecutive follower
                return false;
            }
        }
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


bool
MSLane::moveNonCritical() {
    assert(myVehicles.size()!=0);
    // Set the information about the last vehicle
    myLastState = (*myVehicles.begin())->getState();
    myFirstUnsafe = 0;
    myLeftVehLength = myVehicleLengthSum;

    // Move vehicles except first and all vehicles that may reach the end of the lane
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;

    VehCont::iterator neighIt, neighEnd;
    bool useNeigh = false;
    if (myFirstNeigh!=myLastNeigh) {
        if ((*myFirstNeigh)->myVehicles.size()!=0) {
            useNeigh = true;
            neighIt = (*myFirstNeigh)->myVehicles.begin();
            neighEnd = (*myFirstNeigh)->myVehicles.end();
        }
    }

    vector<MSVehicle*> collisions;
    for (veh = myVehicles.begin(); !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;) {
        myLeftVehLength -= (*veh)->getVehicleType().getLength();
        // get the leader
        //  ... there must be one, because the first vehicle is moved
        //   using moveCritical ...
        VehCont::const_iterator pred(veh + 1);
        bool hadCollision = false;
        // do we have to regard the neighbor?
        if (useNeigh) {
            // get the neighbor
            while (neighIt!=neighEnd&&(*neighIt)->getPositionOnLane()<(*veh)->getPositionOnLane())  {
                ++neighIt;
            }
            if (neighIt!=neighEnd) {
                // move vehicle regarding the neighbor
                hadCollision = (*veh)->move(this, *pred, *neighIt);
            } else {
                // move vehicle without regarding the neighbor
                hadCollision = (*veh)->move(this, *pred, 0);
                // no further neighbors
                useNeigh = false;
            }
        } else {
            // move vehicle without regarding the neighbor
            hadCollision = (*veh)->move(this, *pred, 0);
        }
        if (hadCollision) {
            collisions.push_back(*veh);
        }
        ++veh;
        ++myFirstUnsafe;
    }
    // deal with collisions
    for (vector<MSVehicle*>::iterator i=collisions.begin(); i!=collisions.end(); ++i) {
        (*i)->leaveLaneAtLaneChange();
        (*i)->onTripEnd();
        MSVehicleTransfer::getInstance()->addVeh((*i));
        myVehicles.erase(find(myVehicles.begin(), myVehicles.end(), *i));
        --myFirstUnsafe;
    }
    return myVehicles.size()==0;
}


bool
MSLane::moveCritical() {
    assert(myVehicles.size()!=0);
    vector<MSVehicle*> collisions;
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;
    // Move all next vehicles beside the first
    for (veh=myVehicles.begin()+myFirstUnsafe; veh != lastBeforeEnd;) {
        myLeftVehLength -= (*veh)->getVehicleType().getLength();
        VehCont::const_iterator pred(veh + 1);
        if ((*veh)->moveRegardingCritical(this, *pred, 0, myLeftVehLength)) {
            collisions.push_back(*veh);
        }
        ++veh;
    }
    myLeftVehLength -= (*veh)->getVehicleType().getLength();
    if ((*veh)->moveRegardingCritical(this, 0, 0, myLeftVehLength)) {
        collisions.push_back(*veh);
    }
    assert((*veh)->getPositionOnLane() <= myLength);
    assert(&(*veh)->getLane()==this);
    // deal with collisions
    for (vector<MSVehicle*>::iterator i=collisions.begin(); i!=collisions.end(); ++i) {
        (*i)->leaveLaneAtLaneChange();
        (*i)->onTripEnd();
        MSVehicleTransfer::getInstance()->addVeh((*i));
        myVehicles.erase(find(myVehicles.begin(), myVehicles.end(), *i));
    }
    return myVehicles.size()==0;
}


void
MSLane::detectCollisions(SUMOTime timestep) {
    if (myVehicles.size() < 2) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for (VehCont::iterator veh = myVehicles.begin(); veh != lastVeh;) {
        VehCont::iterator pred = veh + 1;
        SUMOReal gap = (*pred)->getPositionOnLane() - (*pred)->getVehicleType().getLength() - (*veh)->getPositionOnLane();
        if (gap < 0) {
            MSVehicle *predV = *pred;
            MSVehicle *vehV = *veh;
            MsgHandler *handler = 0;
            if (!OptionsCont::getOptions().getBool("quit-on-accident")) {
                handler = MsgHandler::getWarningInstance();
            } else {
                handler = MsgHandler::getErrorInstance();
            }
            handler->inform("MSLane::detectCollision: Collision of " + vehV->getID() + " with " + predV->getID() + " on MSLane " + myID +" during timestep " + toString<int>(timestep));
            if (OptionsCont::getOptions().getBool("quit-on-accident")) {
                throw ProcessError();
            } else {
                vehV->leaveLaneAtLaneChange();
                vehV->onTripEnd();
                MSVehicleTransfer::getInstance()->addVeh(vehV);
            }
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
    MSLane *currentLane = (*(*next)->getLanes())[0];
    SUMOReal seen = currentLane->length() - pos;
    SUMOReal dist = SPEED2DIST(speed) + veh.getVehicleType().brakeGap(speed);
    SUMOReal tspeed = speed;
    while (seen<dist&&next!=veh.getRoute().end()-1) {
        ++next;
        MSLane *nextLane = (*(*next)->getLanes())[0];
        tspeed = MIN2(veh.getVehicleType().ffeV(tspeed, seen, nextLane->maxSpeed()), nextLane->maxSpeed());
        dist = SPEED2DIST(tspeed) + veh.getVehicleType().brakeGap(tspeed);
        seen += nextLane->maxSpeed();
    }
    return tspeed;
}


bool
MSLane::setCritical(std::vector<MSLane*> &into) {
    // move critical vehicles
    int to_pop = 0;
    int to_pop2 = 0;
    int running = 1;
    bool lastPopped = false;
    bool hadProblem = false;
    bool hadPopped = false;
    VehCont::iterator i;
    for (i=myVehicles.begin() + myFirstUnsafe; i!=myVehicles.end();) {
        (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if (target!=this) {
            hadPopped = true;
            if (!(to_pop==0||lastPopped)) {
                hadProblem = true;
            }
            lastPopped = true;
            to_pop++;
            to_pop2 = running;//++;
        } else {
            lastPopped = false;
        }
        ++i;
        if (hadPopped) {
            ++running;
        }
    }
    if (to_pop!=to_pop2) {
        hadProblem = true;
    }
    for (int j = 0; j<to_pop2; j++) {
        MSVehicle *v = *(myVehicles.end() - 1);
        MSVehicle *p = pop();
        assert(v==p);
        MSLane *target = p->getTargetLane();
        if (target==this) {
            MsgHandler::getWarningInstance()->inform("Vehicle '" + v->getID() + "' will be teleported due to false leaving order.");
            v->leaveLaneAtLaneChange();
            v->onTripEnd();
            MSVehicleTransfer::getInstance()->addVeh(v);
            hadProblem = true;
            continue;
        }
        if (target!=0&&p->isOnRoad()) {
            target->push(p);
            into.push_back(target);
        }
    }
    // check whether the lane is free
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
    if (myVehicles.size()>0) {
        if (MSGlobals::gTimeToGridlock>0
                && !(*(myVehicles.end()-1))->isStopped()
                &&
                (*(myVehicles.end()-1))->getWaitingTime()>MSGlobals::gTimeToGridlock) {

            MSVehicleTransfer *vt = MSVehicleTransfer::getInstance();
            MSVehicle *veh = removeFirstVehicle();
#ifdef HAVE_BOYOM_C2C
            if (veh->isEquipped()) {
                veh->getEdge()->removeEquippedVehicle(veh->getID());
            }
#endif
            vt->addVeh(veh);
        }
    }
    // check for vehicle removal
    for (VehCont::iterator veh = myVehicles.begin(); veh != myVehicles.end();) {
        MSVehicle *vehV = *veh;
        if (vehV->ends()) {
            myVehicleLengthSum -= vehV->getVehicleType().getLength();
            vehV->leaveLaneAtLaneChange();
            vehV->onTripEnd();
            MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(vehV);
            veh = myVehicles.erase(veh); // remove current vehicle
        } else {
            ++veh;
        }
    }
    return myVehicles.size()==0;
}


bool
MSLane::dictionary(string id, MSLane* ptr) {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSLane*
MSLane::dictionary(string id) {
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


bool
MSLane::push(MSVehicle* veh) {
    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    // check whether the vehicle has ended his route
    // Add to mean data (edge/lane state dump)
    if (! veh->destReached(myEdge)) {     // adjusts vehicles routeIterator
        myVehBuffer.push_back(veh);
        veh->enterLaneAtMove(this, SPEED2DIST(veh->getSpeed()) - veh->getPositionOnLane());
        SUMOReal pspeed = veh->getSpeed();
        SUMOReal oldPos = veh->getPositionOnLane() - SPEED2DIST(veh->getSpeed());
        veh->workOnMoveReminders(oldPos, veh->getPositionOnLane(), pspeed);
        return false;
    } else {
        veh->onTripEnd(this);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
}


MSVehicle*
MSLane::pop() {
    assert(! myVehicles.empty());
    MSVehicle* first = myVehicles.back();
    first->leaveLaneAtMove(SPEED2DIST(first->getSpeed())/* - first->pos()*/);
    myVehicles.pop_back();
    myVehicleLengthSum -= first->getVehicleType().getLength();
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
    }
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
MSLane::integrateNewVehicle() {
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
MSLane::succLinkSec(const MSVehicle& veh, unsigned int nRouteSuccs,
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
            if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleClass())) {
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
        if ((*link)->getLane()!=0 && (*link)->getLane()->myEdge == nRouteEdge && (*link)->getLane()->allowsVehicleClass(veh.getVehicleClass())) {
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
    const MSEdge::LaneCont *next_allowed = nRouteEdge->allowedLanes(*nRouteEdge2, veh.getVehicleClass());
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


const std::string &
MSLane::getID() const {
    return myID;
}


void
MSLane::releaseVehicles() {}


const MSLane::VehCont &
MSLane::getVehiclesSecure() {
    throw 1;
}


void
MSLane::swapAfterLaneChange() {
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
    if (myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
}




GUILaneWrapper *
MSLane::buildLaneWrapper(GUIGlObjectStorage &) {
    throw "Only within the gui-version";
}


void
MSLane::init(MSEdgeControl &, MSEdge::LaneCont::const_iterator firstNeigh, MSEdge::LaneCont::const_iterator lastNeigh) {
    myFirstNeigh = firstNeigh;
    myLastNeigh = lastNeigh;
}


unsigned int
MSLane::getVehicleNumber() const {
    return (unsigned int) myVehicles.size();
}


MSVehicle *
MSLane::removeFirstVehicle() {
    MSVehicle *veh = *(myVehicles.end()-1);
    veh->leaveLaneAtLaneChange();
    myVehicles.erase(myVehicles.end()-1);
    myVehicleLengthSum -= veh->getVehicleType().getLength();
    return veh;
}


MSVehicle *
MSLane::removeVehicle(MSVehicle * remVehicle) {
    for (MSLane::VehCont::iterator it = myVehicles.begin();
            it < myVehicles.end();
            it++) {
        if (remVehicle->getID() == (*it)->getID()) {
            remVehicle->leaveLaneAtLaneChange();
            myVehicles.erase(it);
            myVehicleLengthSum -= remVehicle->getVehicleType().getLength();
            break;
        }
    }
    return remVehicle;
}


size_t
MSLane::getNumericalID() const {
    return myNumericalID;
}


SUMOReal
MSLane::getDensity() const {
    return myVehicleLengthSum / myLength;
}


SUMOReal
MSLane::getVehLenSum() const {
    return myVehicleLengthSum;
}


MSLane * const
    MSLane::getLeftLane() const {
    return myEdge->leftLane(this);
}


MSLane * const
    MSLane::getRightLane() const {
    return myEdge->rightLane(this);
}


const std::vector<SUMOVehicleClass> &
MSLane::getAllowedClasses() const {
    return myAllowedClasses;
}


const std::vector<SUMOVehicleClass> &
MSLane::getNotAllowedClasses() const {
    return myNotAllowedClasses;
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
    ili.length = lane->length();
    myIncomingLanes.push_back(ili);
}

class by_second_sorter {
public:
    inline int operator()(const std::pair<const MSVehicle * , SUMOReal> &p1, const std::pair<const MSVehicle * , SUMOReal> &p2) const {
        return p1.second<p2.second;
    }
};

std::pair<MSVehicle * const, SUMOReal>
MSLane::getFollowerOnConsecutive(SUMOReal dist, SUMOReal seen, SUMOReal leaderSpeed) const {
    // ok, a vehicle has not noticed the lane about itself;
    //  iterate as long as necessary to search for an approaching one
    set<MSLane*> visited;
    visited.insert((MSLane*) this);
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
                SUMOReal igap = v->interactionGap(v->getSpeed(), myMaxSpeed, leaderSpeed);
                if (igap>(*i).length-v->getPositionOnLane()+seen) {
                    possible.push_back(make_pair(v, (*i).length-v->getPositionOnLane()+seen));
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
        }
        if (nextLane->maxSpeed()<speed) {
            dist = veh.getVehicleType().brakeGap(nextLane->maxSpeed());
        }
        seen += nextLane->length();
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


SUMOReal
MSLane::getMeanSpeed() const {
    if (myVehicles.size()==0) {
        return myMaxSpeed;
    }
    SUMOReal v = 0;
    for (VehCont::const_iterator i=myVehicles.begin(); i!=myVehicles.end(); ++i) {
        v += (*i)->getSpeed();
    }
    return v / (SUMOReal) myVehicles.size();
}



/****************************************************************************/

