/****************************************************************************/
/// @file    MSVehicle.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a vehicle in the micro simulation
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

#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSLinkCont.h"
#include "MSVehicleQuitReminded.h"
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <map>
#include "MSMoveReminder.h"
#include <utils/options/OptionsCont.h>
#include "MSLCM_DK2004.h"
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "trigger/MSBusStop.h"
#include <utils/common/DijkstraRouterTT.h>
#include "MSPerson.h"
#include <utils/common/RandHelper.h>
#include "devices/MSDevice_C2C.h"
#include "devices/MSDevice_Routing.h"
#include <microsim/devices/MSDevice_HBEFA.h>

#ifdef _MESSAGES
#include "MSMessageEmitter.h"
#endif

#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include "MSGlobals.h"
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_VEHICLE_GUI_SELECTION 1
#ifdef DEBUG_VEHICLE_GUI_SELECTION
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUILane.h>
#endif

#define BUS_STOP_OFFSET 0.5


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static value definitions
// ===========================================================================
vector<MSLane*> MSVehicle::myEmptyLaneVector;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSVehicle::State
 * ----------------------------------------------------------------------- */
MSVehicle::State::State(const State& state) {
    myPos = state.myPos;
    mySpeed = state.mySpeed;
}


MSVehicle::State&
MSVehicle::State::operator=(const State& state) {
    myPos   = state.myPos;
    mySpeed = state.mySpeed;
    return *this;
}


bool
MSVehicle::State::operator!=(const State& state) {
    return (myPos   != state.myPos ||
            mySpeed != state.mySpeed);
}


SUMOReal
MSVehicle::State::pos() const {
    return myPos;
}


MSVehicle::State::State(SUMOReal pos, SUMOReal speed) :
        myPos(pos), mySpeed(speed) {}


/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle() throw() {
    // remove move reminder
    for (QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    // delete the route
    if (!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    }
    // delete values in CORN
    if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE)!=myPointerCORNMap.end()) {
        ReplacedRoutesVector *v = (ReplacedRoutesVector*) myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE];
        for (ReplacedRoutesVector::iterator i=v->begin(); i!=v->end(); ++i) {
            delete(*i).route;
        }
        delete v;
    }
    if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_DEPART_INFO)!=myPointerCORNMap.end()) {
        DepartArrivalInformation *i = (DepartArrivalInformation*) myPointerCORNMap[MSCORN::CORN_P_VEH_DEPART_INFO];
        delete i;
    }
    if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_ARRIVAL_INFO)!=myPointerCORNMap.end()) {
        DepartArrivalInformation *i = (DepartArrivalInformation*) myPointerCORNMap[MSCORN::CORN_P_VEH_ARRIVAL_INFO];
        delete i;
    }
    //
    delete myParameter;
    delete myLaneChangeModel;
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        delete(*dev);
    }
    myDevices.clear();
#ifndef NO_TRACI
    myTraciEdgeWeights.clear();
#endif
    // persons
    if (hasCORNPointerValue(MSCORN::CORN_P_VEH_PASSENGER)) {
        std::vector<MSPerson*> *persons = (std::vector<MSPerson*>*) myPointerCORNMap[MSCORN::CORN_P_VEH_PASSENGER];
        for (std::vector<MSPerson*>::iterator i=persons->begin(); i!=persons->end(); ++i) {
            (*i)->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
        }
        delete persons;
    }
}


MSVehicle::MSVehicle(SUMOVehicleParameter* pars,
                     const MSRoute* route,
                     const MSVehicleType* type,
                     int vehicleIndex) :
#ifdef HAVE_MESOSIM
        MEVehicle(0, 0),
#endif
        myLastLaneChangeOffset(0),
        myTarget(0),
        myWaitingTime(0),
        myParameter(pars),
        myRoute(route),
        myState(0, 0), //
        myIndividualMaxSpeed(0.0),
        myHasIndividualMaxSpeed(false),
        myReferenceSpeed(-1.0),
        myLane(0),
        myType(type),
        myLastBestLanesEdge(0),
        myCurrEdge(myRoute->begin()),
        myAllowedLanes(0),
        myMoveReminders(0),
        myOldLaneMoveReminders(0),
        myOldLaneMoveReminderOffsets(0),
        myArrivalPos(pars->arrivalPos),
        myPreDawdleAcceleration(0)
#ifndef NO_TRACI
        ,myNeedReroute(false),
        adaptingSpeed(false),
        isLastAdaption(false),
        speedBeforeAdaption(0),
        timeBeforeAdaption(0),
        speedReduction(0),
        adaptDuration(0),
        timeBeforeLaneChange(0),
        laneChangeStickyTime(0),
        laneChangeConstraintActive(false),
        myDestinationLane(0)
#endif
{
#ifdef _MESSAGES
    myLCMsgEmitter = MSNet::getInstance()->getMsgEmitter("lanechange");
    myBMsgEmitter = MSNet::getInstance()->getMsgEmitter("break");
    myHBMsgEmitter = MSNet::getInstance()->getMsgEmitter("heartbeat");
#endif
    // build arrival definition
    SUMOReal lastLaneLength = (*(myRoute->getLastEdge()->getLanes()))[0]->length();
    if (myArrivalPos < 0) {
        myArrivalPos += lastLaneLength; // !!! validate!
    }
    if (myArrivalPos<0) {
        myArrivalPos = 0;
    }
    if (myArrivalPos>lastLaneLength) {
        myArrivalPos = lastLaneLength;
    }
    MSDevice_Routing::buildVehicleDevices(*this, myDevices);
    rebuildAllowedLanes();
    myLaneChangeModel = new MSLCM_DK2004(*this);
    // init devices
    MSDevice_C2C::buildVehicleDevices(*this, myDevices);
    MSDevice_HBEFA::buildVehicleDevices(*this, myDevices);
    // init CORN containers
    if (MSCORN::wished(MSCORN::CORN_VEH_WAITINGTIME)) {
        myIntCORNMap[MSCORN::CORN_VEH_WAITINGTIME] = 0;
    }
}



// ------------ Retrieval of CORN values
int
MSVehicle::getCORNIntValue(MSCORN::Function f) const throw() {
    return myIntCORNMap.find(f)->second;
}


void *
MSVehicle::getCORNPointerValue(MSCORN::Pointer p) const throw() {
    return myPointerCORNMap.find(p)->second;
}


bool
MSVehicle::hasCORNIntValue(MSCORN::Function f) const throw() {
    return myIntCORNMap.find(f)!=myIntCORNMap.end();
}


bool
MSVehicle::hasCORNPointerValue(MSCORN::Pointer p) const throw() {
    return myPointerCORNMap.find(p)!=myPointerCORNMap.end();
}




// ------------ Interaction with move reminders
SUMOReal
MSVehicle::getPositionOnActiveMoveReminderLane(const MSLane * const searchedLane) const throw() {
    if (searchedLane==myLane) {
        return myState.myPos;
    }
    vector< MSMoveReminder* >::const_iterator rem = myOldLaneMoveReminders.begin();
    std::vector<SUMOReal>::const_iterator off = myOldLaneMoveReminderOffsets.begin();
    for (; rem!=myOldLaneMoveReminders.end()&&off!=myOldLaneMoveReminderOffsets.end(); ++rem, ++off) {
        if ((*rem)->getLane()==searchedLane) {
            return (*off) + myState.myPos;
        }
    }
    return -1;
}


void
MSVehicle::workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (vector< MSMoveReminder* >::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!(*rem)->isStillActive(*this, oldPos, newPos, newSpeed)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
    OffsetVector::iterator off=myOldLaneMoveReminderOffsets.begin();
    for (vector< MSMoveReminder* >::iterator rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end();) {
        SUMOReal oldLaneLength = *off;
        if (!(*rem)->isStillActive(*this, oldLaneLength+oldPos, oldLaneLength+newPos, newSpeed)) {
            rem = myOldLaneMoveReminders.erase(rem);
            off = myOldLaneMoveReminderOffsets.erase(off);
        } else {
            ++rem;
            ++off;
        }
    }
}


void
MSVehicle::adaptLaneEntering2MoveReminder(const MSLane &enteredLane) throw() {
    // save the old work reminders, patching the position information
    //  add the information about the new offset to the old lane reminders
    SUMOReal oldLaneLength = myLane->length();
    OffsetVector::iterator i;
    for (i=myOldLaneMoveReminderOffsets.begin(); i!=myOldLaneMoveReminderOffsets.end(); ++i) {
        (*i) += oldLaneLength;
    }
    for (size_t j=0; j<myMoveReminders.size(); j++) {
        myOldLaneMoveReminderOffsets.push_back(oldLaneLength);
    }
    copy(myMoveReminders.begin(), myMoveReminders.end(), back_inserter(myOldLaneMoveReminders));
    assert(myOldLaneMoveReminders.size()==myOldLaneMoveReminderOffsets.size());
    // get new move reminder
    myMoveReminders = enteredLane.getMoveReminders();
}


void
MSVehicle::activateRemindersByEmitOrLaneChange(bool isEmit) throw() {
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (vector< MSMoveReminder* >::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!(*rem)->isActivatedByEmitOrLaneChange(*this, isEmit)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
}


// ------------
const MSEdge*
MSVehicle::succEdge(unsigned int nSuccs) const throw() {
    if (hasSuccEdge(nSuccs)) {
        return *(myCurrEdge + nSuccs);
    } else {
        return 0;
    }
}


bool
MSVehicle::destReached(const MSEdge* targetEdge) throw() {
    // vaporizing edge?
    if (targetEdge->isVaporizing()) {
        // yep, let's do the vaporization...
        setWasVaporized(false);
        return true;
    }
    // internal edge?
    if (targetEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        // yep, let's continue driving
        return false;
    }
    // search for the target in the vehicle's route. Usually there is
    // only one iteration. Only for very short edges a vehicle can
    // "jump" over one ore more edges in one timestep.
    MSRouteIterator edgeIt = myCurrEdge;
    while (*edgeIt != targetEdge) {
        ++edgeIt;
        assert(edgeIt != myRoute->end());
    }
    myCurrEdge = edgeIt;
    // Check if destination-edge is reached. Update allowedLanes makes
    // only sense if destination isn't reached.
    MSRouteIterator destination = myRoute->end() - 1;
    if (myCurrEdge == destination && myState.myPos > myArrivalPos - POSITION_EPS) {
        return true;
    } else {
        rebuildAllowedLanes(false);
        return false;
    }
}


bool
MSVehicle::ends() const throw() {
    return myCurrEdge==myRoute->end()-1 && myState.myPos > myArrivalPos - POSITION_EPS;
}


bool
MSVehicle::addStop(const Stop &stop) throw() {
    MSRouteIterator stopEdge = myRoute->find(stop.lane->getEdge(), myCurrEdge);
    if (myCurrEdge > stopEdge || (myCurrEdge == stopEdge && myState.myPos > stop.pos - myType->brakeGap(myState.mySpeed))) {
        // do not add the stop if the vehicle is already behind it or cannot break
        return false;
    }
    // check whether the stop lies at the end of a route
    std::list<Stop>::iterator iter = myStops.begin();
    MSRouteIterator last = myRoute->begin();
    if (myStops.size()>0) {
        last = myRoute->find(myStops.back().lane->getEdge());
        last = myRoute->find(stop.lane->getEdge(), last);
        if (last!=myRoute->end()) {
            iter = myStops.end();
            stopEdge = last;
        }
    }
    while ((iter != myStops.end())
            && (myRoute->find(iter->lane->getEdge()) <= stopEdge)) {
        iter++;
    }
    while ((iter != myStops.end())
            && (stop.pos > iter->pos)
            && (myRoute->find(iter->lane->getEdge()) == stopEdge)) {
        iter++;
    }
    myStops.insert(iter, stop);
    return true;
}


bool
MSVehicle::isStopped() {
    return !myStops.empty() && myStops.begin()->reached
           && myStops.begin()->duration>0;
}


SUMOReal
MSVehicle::processNextStop(SUMOReal currentVelocity) throw() {
    if (myStops.empty()) {
        // no stops; pass
        return currentVelocity;
    }
    if (myStops.begin()->reached) {
        // ok, we have already reached the next stop
        if (myStops.begin()->duration==0) {
            // ... and have waited as long as needed
            if (myStops.begin()->busstop!=0) {
                // inform bus stop about leaving it
                myStops.begin()->busstop->leaveFrom(this);
            }
            // the current stop is no longer valid
            myStops.pop_front();
            // maybe the next stop is on the same edge; let's rebuild best lanes
            getBestLanes(true);
            // continue as wished...
        } else {
            // we have to wait some more time
            myStops.begin()->duration--;
            myTarget = myLane;
            return 0;
        }
    } else {
        // is the next stop on the current lane?
        if (myStops.begin()->lane==myLane) {
            Stop &bstop = *myStops.begin();
            // get the stopping position
            SUMOReal endPos = bstop.pos;
            bool busStopsMustHaveSpace = true;
            if (bstop.busstop!=0) {
                // on bus stops, we have to wait for free place if they are in use...
                endPos = bstop.busstop->getLastFreePos();
                if (endPos-5.<bstop.busstop->getBeginLanePosition()) { // !!! explicite offset
                    busStopsMustHaveSpace = false;
                }
            }
            if (myState.pos()>=endPos-BUS_STOP_OFFSET&&busStopsMustHaveSpace) {
                // ok, we may stop (have reached the stop)
                bstop.reached = true;
                // compute stopping time
                if (bstop.until>=0) {
                    if (bstop.duration==-1) {
                        bstop.duration = bstop.until - MSNet::getInstance()->getCurrentTimeStep();
                    } else {
                        bstop.duration = MAX2(bstop.duration, bstop.until - MSNet::getInstance()->getCurrentTimeStep());
                    }
                }
                if (bstop.busstop!=0) {
                    // let the bus stop know the vehicle
                    bstop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                }
            }
            // decelerate
            // !!! should not v be 0 when we have reached the stop?
            return myType->ffeS(myState.mySpeed, endPos-myState.pos());
        }
    }
    return currentVelocity;
}


bool
MSVehicle::move(const MSLane * const lane, const MSVehicle * const pred, const MSVehicle * const neigh) throw() {
    // reset move information
    myTarget = 0;
    // save old v for optional acceleration computation
    SUMOReal oldV = myState.mySpeed;
    // compute gap to use
    SUMOReal gap = gap2pred(*pred);
    if (MSGlobals::gCheck4Accidents && gap<0) {
        // collision occured!
        return true;
    }
    // security check for too low gaps
    if (gap<0.1) {
        gap = 0;
    }
    //
    SUMOReal vSafe  = myType->ffeV(myState.mySpeed, gap, pred->getSpeed());
    if (neigh!=0&&neigh->getSpeed()>60./3.6) {
        SUMOReal mgap = MAX2((SUMOReal) 0, neigh->getPositionOnLane()-neigh->getVehicleType().getLength()-getPositionOnLane());
        SUMOReal nVSafe = myType->ffeV(myState.mySpeed, mgap, neigh->getSpeed());
        vSafe = MIN2(vSafe, nVSafe);
    }
    // take stops into account
    vSafe = MIN2(vSafe, processNextStop(vSafe));

    SUMOReal maxNextSpeed = myType->maxNextSpeed(myState.mySpeed);
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    myPreDawdleAcceleration = SPEED2ACCEL(vSafe-oldV);

    SUMOReal vNext = myType->dawdle(MIN3(lane->maxSpeed(), myType->maxNextSpeed(myState.mySpeed), vSafe));
    vNext =
        myLaneChangeModel->patchSpeed(
            MAX2((SUMOReal) 0, myState.mySpeed-(SUMOReal)ACCEL2SPEED(myType->getMaxDecel())), //!!! reverify
            vNext,
            MIN3(vSafe, myLane->maxSpeed(), maxNextSpeed),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    vNext = MIN4(vNext, vSafe, myLane->maxSpeed(), maxNextSpeed);//vaccel(myState.mySpeed, myLane->maxSpeed()));

    SUMOReal predDec = pred->getSpeedAfterMaxDecel(pred->getSpeed()); //!!!!q//-decelAbility() /* !!! decelAbility of leader! */);
    if (myType->brakeGap(vNext)+vNext*myType->getTau() > myType->brakeGap(predDec) + gap) {

        vNext = MIN2(vNext, (SUMOReal) DIST2SPEED(gap));
    }

    vNext = MAX3((SUMOReal) 0, vNext, myType->getSpeedAfterMaxDecel(oldV));
    if (vNext<=0.1) {
        myWaitingTime += DELTA_T;
        if (MSCORN::wished(MSCORN::CORN_VEH_WAITINGTIME)) {
            myIntCORNMap[MSCORN::CORN_VEH_WAITINGTIME] = myIntCORNMap[MSCORN::CORN_VEH_WAITINGTIME] + 1;
        }
    } else {
        myWaitingTime = 0;
    }

    vNext = MIN2(vNext, getMaxSpeed());

    // call reminders after vNext is set
    workOnMoveReminders(myState.myPos, myState.myPos + SPEED2DIST(vNext), vNext);
#ifdef _MESSAGES
    if (myHBMsgEmitter != 0) {
        if (isOnRoad()) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myHBMsgEmitter->writeHeartBeatEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
    if (myBMsgEmitter!=0) {
        if (vNext < oldV) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myBMsgEmitter->writeBreakEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
#endif
    // update position and speed
    myState.myPos += SPEED2DIST(vNext);
    assert(myState.myPos < lane->length());
    myState.mySpeed = vNext;
    //@ to be optimized (move to somewhere else)
    if (hasCORNIntValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
    //
    setBlinkerInformation();
    return false;
}


bool
MSVehicle::moveRegardingCritical(const MSLane* const lane,
                                 const MSVehicle * const pred,
                                 const MSVehicle * const neigh,
                                 SUMOReal lengthsInFront) throw() {
#ifdef _MESSAGES
    if (myHBMsgEmitter != 0) {
        if (isOnRoad()) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myHBMsgEmitter->writeHeartBeatEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
#endif
    myLFLinkLanes.clear();
    // check whether the vehicle is not on an appropriate lane
    if (!myLane->appropriate(this)) {
        // decelerate to lane end when yes
        SUMOReal vWish = MIN2(myType->ffeS(myState.mySpeed, myLane->length()-myState.myPos), myLane->maxSpeed());
        if (pred!=0) {
            SUMOReal gap = gap2pred(*pred);
            if (MSGlobals::gCheck4Accidents && gap<0) {
                // collision occured!
                return true;
            }
            vWish = MIN2(vWish, myType->ffeV(myState.mySpeed, gap2pred(*pred), pred->getSpeed()));
        }
        // !!! check whether the vehicle wants to stop somewhere
        if (!myStops.empty()&&myStops.begin()->lane->getEdge()==lane->getEdge()) {
            SUMOReal seen = lane->length() - myState.pos();
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(lane->length()-myStops.begin()->pos));
            vWish = MIN2(vWish, vsafeStop);
        }
        vWish = MAX2((SUMOReal) 0, vWish);
        myLFLinkLanes.push_back(DriveProcessItem(0, vWish, vWish, false));
    } else {
        // compute other values as in move
        SUMOReal vBeg = MIN2(myType->maxNextSpeed(myState.mySpeed), lane->maxSpeed());//vaccel( myState.mySpeed, lane->maxSpeed() );
        if (pred!=0) {
            SUMOReal gap = gap2pred(*pred);
            if (MSGlobals::gCheck4Accidents && gap<0) {
                // collision occured!
                return true;
            }
            SUMOReal vSafe = myType->ffeV(myState.mySpeed, gap, pred->getSpeed());
            //  the vehicle is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            vBeg = MIN2(vBeg, vSafe);
        }
        vBeg = MAX2(vBeg, myType->getSpeedAfterMaxDecel(myState.mySpeed));
        // check whether the driver wants to let someone in
        // set next links, computing possible speeds
        vsafeCriticalCont(vBeg, lengthsInFront);
    }
    //@ to be optimized (move to somewhere else)
    if (hasCORNIntValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
    return false;
}


void
MSVehicle::moveFirstChecked() {
    myTarget = 0;
    // save old v for optional acceleration computation
    SUMOReal oldV = myState.mySpeed;
    // get vsafe
    SUMOReal vSafe = 0;

    assert(myLFLinkLanes.size()!=0);
    DriveItemVector::iterator i;
    bool cont = true;
    for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()&&cont; ++i) {
        MSLink *link = (*i).myLink;
        bool onLinkEnd = link==0;
        // the vehicle must change the lane on one of the next lanes
        if (!onLinkEnd) {
            if (link->havePriority()&&link->opened()) {
                vSafe = (*i).myVLinkPass;
            } else {
                if (link->opened()) {
                    vSafe = (*i).myVLinkPass;
                } else {
                    bool yellow = link->getState()==MSLink::LINKSTATE_TL_YELLOW_MAJOR||link->getState()==MSLink::LINKSTATE_TL_YELLOW_MINOR;
                    if (vSafe<getSpeedAfterMaxDecel(myState.mySpeed)&&yellow) {
                        vSafe = (*i).myVLinkPass;
                    } else {
                        vSafe = (*i).myVLinkWait;
                        cont = false;
                    }
                }
            }
        } else {
            vSafe = (*i).myVLinkWait;
            cont = false;
            break;
        }
    }

    // take stops into account
    vSafe = MIN2(vSafe, processNextStop(vSafe));
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    myPreDawdleAcceleration = SPEED2ACCEL(vSafe-oldV);
    // compute vNext in considering dawdling
    SUMOReal vNext = myType->dawdle(vSafe);
    vNext =
        myLaneChangeModel->patchSpeed(
            getSpeedAfterMaxDecel(myState.mySpeed),
            vNext,
            MIN3(vSafe, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed()), //vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    vNext = MAX3(vNext, myType->getSpeedAfterMaxDecel(oldV), (SUMOReal) 0.);
    // visit waiting time
    if (vNext<=0.1) {
        myWaitingTime += DELTA_T;
        if (MSCORN::wished(MSCORN::CORN_VEH_WAITINGTIME)) {
            myIntCORNMap[MSCORN::CORN_VEH_WAITINGTIME] = myIntCORNMap[MSCORN::CORN_VEH_WAITINGTIME] + 1;
        }
    } else {
        myWaitingTime = 0;
    }
    // call reminders after vNext is set
    SUMOReal pos = myState.myPos;

    vNext = MIN2(vNext, getMaxSpeed());

#ifdef _MESSAGES
    if (myHBMsgEmitter != 0) {
        if (isOnRoad()) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myHBMsgEmitter->writeHeartBeatEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
    if (myBMsgEmitter!=0) {
        if (vNext < oldV) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myBMsgEmitter->writeBreakEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
#endif
    // update position
    SUMOReal oldPos = myState.myPos;
    myState.myPos += SPEED2DIST(vNext);
    // update speed
    myState.mySpeed = vNext;
    MSLane *approachedLane = myLane;

    // move the vehicle forward
    size_t no = 0;
    SUMOReal driven =
        myState.myPos>approachedLane->length()
        ? approachedLane->length() - pos
        : myState.myPos - pos;
    SUMOReal tmpPos = approachedLane->length() + myType->brakeGap(myState.mySpeed);
    for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()
            &&
            myState.myPos>approachedLane->length();
            ++i) {

        if (approachedLane!=myLane) {
            leaveLaneAtMove(driven);
        }
        MSLink *link = (*i).myLink;
        // check whether the vehicle was allowed to enter lane
        //  otherwise it is decelareted and we do not need to test for it's
        //  approach on the following lanes when a lane changing is performed
        assert(approachedLane!=0);
        myState.myPos -= approachedLane->length();
        oldPos -= approachedLane->length();
        tmpPos -= approachedLane->length();
        assert(myState.myPos>0);
        if (approachedLane!=myLane) {
            enterLaneAtMove(approachedLane, driven);
            driven += approachedLane->length();
        }
        // proceed to the next lane
        if (link!=0/*approachedLane->isLinkEnd(link)*/) {
#ifdef HAVE_INTERNAL_LANES
            approachedLane = link->getViaLane();
            if (approachedLane==0) {
                approachedLane = link->getLane();
            }
#else
            approachedLane = link->getLane();
#endif
        }
        // set information about approaching
        no++;
    }

    // enter lane herein if no push occures (otherwise, do it there)
    if (no==0) {
        workOnMoveReminders(pos, pos + SPEED2DIST(vNext), vNext);
    }
    myTarget = approachedLane;
    assert(myTarget!=0);
    assert(myTarget->length()>=myState.myPos);
    setBlinkerInformation();
}

void
MSVehicle::checkRewindLinkLanes(SUMOReal lengthsInFront) throw() {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
    }
#endif
#ifdef HAVE_INTERNAL_LANES
    if (MSGlobals::gUsingInternalLanes) {
        int removalBegin = -1;
        bool hadVehicle = false;
        SUMOReal seenLanes = 0;
        SUMOReal seenSpace = -lengthsInFront;
        MSLane *nextSeenNonInternal = 0;
        MSLane *approachedLane = myLane;
        int lastLinkToInternal = -1;
        MSVehicle *leader = 0;

        for (unsigned int i=0; i<myLFLinkLanes.size()&&removalBegin<0; ++i) {
            // skip unset links
            DriveProcessItem &item = myLFLinkLanes[i];
            if (item.myLink==0) {
                continue;
            }
            if (approachedLane->getEdge()->getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
                lastLinkToInternal = i;
            }

            //

            // get the next lane, determine whether it is an internal lane
            approachedLane = item.myLink->getViaLane();
            bool nextIsInternal = true;
            if (approachedLane==0) {
                approachedLane = item.myLink->getLane();
                nextIsInternal = false;
            }
            MSEdge::EdgeBasicFunction ef = approachedLane->getEdge()->getPurpose();
            hadVehicle |= approachedLane->getVehicleNumber()!=0;
            nextIsInternal &= item.myLink->hasAnyFoes();
            //
            if (nextIsInternal) {
                // the free place on internal lanes is not counted - vehicles must keep them free
                seenSpace = seenSpace - approachedLane->getVehLenSum();
                if (leader==0&&approachedLane->getLastVehicle()!=0) {
                    leader = approachedLane->getLastVehicle();
                    seenSpace += leader->getVehicleType().brakeGap(leader->getSpeed());
                }
            } else {
                // the free space on plain lanes is counted
                // !!!: on longer lanes, only the place some meters in front... (next extension)
                seenSpace = seenSpace + approachedLane->length() - approachedLane->getVehLenSum();
                if (leader==0&&approachedLane->getLastVehicle()!=0) {
                    leader = approachedLane->getLastVehicle();
                    seenSpace += leader->getVehicleType().brakeGap(leader->getSpeed());
                }
                seenLanes += approachedLane->length();
                // we also do not want the vehicle continue if there is not enough place
                //  behind the last vehicle on the approached lane (and we are currently standing)
                bool nextDisallows = /*myState.mySpeed<.1 &&*/ approachedLane->getLastVehicle()!=0 && approachedLane->getLastVehicle()->getSpeed()<.1;
                if (nextDisallows) {
                    nextDisallows &= approachedLane->getLastVehicle()->getPositionOnLane() < approachedLane->getLastVehicle()->getVehicleType().getLength();
                }
                if (nextDisallows&&lastLinkToInternal>=0) {
                    removalBegin = lastLinkToInternal;
                }
                nextSeenNonInternal = approachedLane;
            }
            // now we check whether the vehicle should not continue because
            //  there is no space on approached lanes
            // - we must have seen at least one vehicle
            // - we must have seen at least as much place as the vehicle would need
            // - the seen space must be enough for our vehicle
            // - we should have seen at least one non-internal lane before
            SUMOReal impatienceCorrection = MAX2(SUMOReal(0), SUMOReal(SUMOReal(myWaitingTime)));
            if (hadVehicle&&seenLanes>getVehicleType().getLength()&&seenSpace<getVehicleType().getLength()-impatienceCorrection/10.&&nextSeenNonInternal!=0) {
                removalBegin = lastLinkToInternal;
            }
        }
        if (removalBegin!=-1) {
            myLFLinkLanes[removalBegin].myVLinkPass = myLFLinkLanes[removalBegin].myVLinkWait;
            myLFLinkLanes[removalBegin].mySetRequest = false;
            if (removalBegin+1<(int)myLFLinkLanes.size()) {
                myLFLinkLanes.erase(myLFLinkLanes.begin()+removalBegin+1, myLFLinkLanes.end());
            }
        }
    }
#endif
    for (DriveItemVector::iterator i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        if ((*i).myLink!=0&&(*i).mySetRequest) {
            (*i).myLink->setApproaching(this);
        }
    }
}



void
MSVehicle::vsafeCriticalCont(SUMOReal boundVSafe, SUMOReal lengthsInFront) {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
    }
#endif
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    SUMOReal seen = myLane->length() - myState.myPos;
    MSLane *nextLane = myLane;
    // compute the way the vehicle would drive if it would use the current speed and then
    //  decelerate
    SUMOReal maxV = myType->maxNextSpeed(myState.mySpeed);
    SUMOReal dist = SPEED2DIST(maxV) + myType->brakeGap(maxV);//myState.mySpeed);
    SUMOReal vLinkPass = boundVSafe;
    SUMOReal vLinkWait = vLinkPass;
    const std::vector<MSLane*> &bestLaneConts = getBestLanesContinuation();
#ifdef HAVE_INTERNAL_LANES
    bool hadNonInternal = false;
#endif

    unsigned int view = 1;
    // loop over following lanes
    while (true) {
        // process stops
        if (!myStops.empty()&&myStops.begin()->lane->getEdge()==nextLane->getEdge()) {
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(nextLane->length()-myStops.begin()->pos));
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
        }

        // get the next link used
        MSLinkCont::const_iterator link = myLane->succLinkSec(*this, view, *nextLane, bestLaneConts);
        // and the length of the currently investigated lane
        SUMOReal laneLength = nextLane->length();

        // check whether the lane is a dead end
        //  (should be valid only on further loop iterations
        if (nextLane->isLinkEnd(link)) {
            // the vehicle will not drive further
            SUMOReal laneEndVSafe = myType->ffeS(myState.mySpeed, seen);
            myLFLinkLanes.push_back(
                DriveProcessItem(0, MIN2(vLinkPass, laneEndVSafe), MIN2(vLinkPass, laneEndVSafe), false));
            checkRewindLinkLanes(lengthsInFront);
            return;
        }
        // the link was passed
        vLinkWait = vLinkPass;


        // !!! optimize this - make this optional
        //  needed to let vehicles wait for all overlapping vehicles in front
        const MSLinkCont &lc = nextLane->getLinkCont();

        // get the following lane
#ifdef HAVE_INTERNAL_LANES
        bool nextInternal = false;
        nextLane = (*link)->getViaLane();
        if (nextLane==0) {
            nextLane = (*link)->getLane();
            hadNonInternal = true;
        } else {
            nextInternal = true;
        }
#else
        nextLane = (*link)->getLane();
#endif

        // compute the velocity to use when the link is not blocked by oter vehicles
        // the vehicle shall be not fastern when reaching the next lane than allowed
        SUMOReal vmaxNextLane =
            MAX2(myType->ffeV(myState.mySpeed, seen, nextLane->maxSpeed()), nextLane->maxSpeed());

        // the vehicle shall keep a secure distance to its predecessor
        //  (or approach the lane end if the predeccessor is too near)
        SUMOReal vsafePredNextLane = 100000;

        // !!! optimize this - make this optional
        SUMOReal r_dist2Pred = seen;
        if (nextLane->getLastVehicle()!=0) {
            // there is a leader -> compute distance to him
            r_dist2Pred = r_dist2Pred + nextLane->myLastState.pos() - nextLane->getLastVehicle()->getVehicleType().getLength();
        } else {
            // no, no leader; we'll look until the currently investigated lane's end
            r_dist2Pred = r_dist2Pred + nextLane->length();
        }

#ifdef HAVE_INTERNAL_LANES
        if (MSGlobals::gUsingInternalLanes) {
            for (size_t j=0; j<lc.size(); ++j) {
                if (lc[j]->getDirection()==MSLink::LINKDIR_TURN) {
                    // hack - opposite traffic causes jams
                    continue;
                }
                MSLane *nl = lc[j]->getViaLane();
                if (nl==0) {
                    nl = lc[j]->getLane();
                }
                if (nl==0) {
                    continue;
                }

                const State &nextLanePred = nl->myLastState;
                SUMOReal dist2Pred = seen;
                if (nl->getLastVehicle()!=0) {
                    dist2Pred = dist2Pred + nextLanePred.pos() - nl->getLastVehicle()->getVehicleType().getLength();
                } else {
                    dist2Pred = dist2Pred + nl->length();
                }
                if (dist2Pred>=0) {
                    // leading vehicle is not overlapping
                    vsafePredNextLane =
                        MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                    SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-(SUMOReal)ACCEL2SPEED(myType->decelAbility()) /* !!! decelAbility of leader! */);
                    if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                        vsafePredNextLane = MIN2(vsafePredNextLane, (SUMOReal)DIST2SPEED(dist2Pred));
                        // !!! break? next vehicle was already seen!
                    }
                } else {
                    // leading vehicle is overlapping (stands within the junction)
                    vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0)); // ==0?
                    // we have to wait in any case
                    break;
                }

                const MSLinkCont &lc2 = nl->getLinkCont();
                for (size_t j2=0; j2<lc2.size(); ++j2) {
                    if (lc2[j2]->getDirection()==MSLink::LINKDIR_TURN) {
                        // hack - opposite traffic causes jams
                        continue;
                    }
                    MSLane *nl2 = lc2[j2]->getViaLane();
                    if (nl2==0) {
                        nl2 = lc2[j2]->getLane();
                    }
                    if (nl2==0) {
                        continue;
                    }
                    const State &nextLanePred2 = nl2->myLastState;
                    SUMOReal dist2Pred2 = seen + nl->length();// @!!! the real length of the car
                    if (nl2->getLastVehicle()!=0) {
                        dist2Pred2 = dist2Pred2 + nextLanePred2.pos() - nl2->getLastVehicle()->getVehicleType().getLength();
                    } else {
                        dist2Pred2 = dist2Pred2 + nl2->length();
                    }
                    if (dist2Pred2>=0) {
                        // leading vehicle is not overlapping
                        vsafePredNextLane =
                            MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred2, nextLanePred2.speed()));
                        SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred2.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                        if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred2) {

                            vsafePredNextLane = MIN2(vsafePredNextLane, (SUMOReal)DIST2SPEED(dist2Pred2));
                        }
                    } else {
                        // leading vehicle is overlapping (stands within the junction)
                        vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0)); // ==0?
                        break;
                    }
                }
            }
        } else {
#endif
            const State &nextLanePred = nextLane->myLastState;
            SUMOReal dist2Pred = seen;
            if (nextLane->getLastVehicle()!=0) {
                SUMOReal nextVehicleLength = nextLane->getLastVehicle()->getVehicleType().getLength();
                if (nextLanePred.pos() - nextVehicleLength < 0) {
                    // the end of pred is beyond his lane's end
                    if (nextVehicleLength>=(*link)->getLength()) {
                        // the end is on our lane -> we may drive up to the min of
                        // a) pred's end
                        // b) lane's end
                        dist2Pred = MIN2(seen, dist2Pred + nextLanePred.pos() - nextVehicleLength + (*link)->getLength());
                    }
                } else {
                    // the end of pred is on his lane
                    dist2Pred = dist2Pred + nextLanePred.pos() - nextVehicleLength;
                }
                /*
                if(nextLanePred.pos() - nextVehicleLength < 0 && nextVehicleLength<(*link)->getLength()) {
                    // the end of pred is beyond his lane's end, but not yet on our
                    //  we may drive until the end of the lane
                    dist2Pred = -1;//dist2Pred;//MAX2(dist2Pred, dist2Pred + nextLanePred.pos() - nextVehicleLength + (*link)->getLength());
                } else {
                    // either pred's end is not beyond his lane, or,
                    //  it is beyond his lane and
                    dist2Pred = dist2Pred + nextLanePred.pos() - nextVehicleLength;
                }
                */
            } else {
                dist2Pred = dist2Pred + nextLane->length();// + (*link)->getLength();
            }

            if (dist2Pred>=0) {
                // leading vehicle is not overlapping
                vsafePredNextLane =
                    MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                if (myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                    vsafePredNextLane = MIN2(vsafePredNextLane, (SUMOReal)DIST2SPEED(dist2Pred));
                }
            } else {
                // leading vehicle is overlapping (stands within the junction)
                vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
            }
#ifdef HAVE_INTERNAL_LANES
        }
#endif
#ifdef DEBUG_VEHICLE_GUI_SELECTION
        if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
            int bla = 0;
        }
#endif

        // compute the velocity to use when the link may be used
        vLinkPass =
            MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane/*, vsafeNextLaneEnd*/);
        vLinkPass = MAX2(vLinkPass, myType->getSpeedAfterMaxDecel(myState.mySpeed)); // should not be necessary !!!

        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait = MIN3(vLinkPass, vLinkWait, myType->ffeS(myState.mySpeed, seen));
        vLinkWait = MAX2(vLinkWait, myType->getSpeedAfterMaxDecel(myState.mySpeed));

        bool yellow = (*link)->getState()==MSLink::LINKSTATE_TL_YELLOW_MAJOR||(*link)->getState()==MSLink::LINKSTATE_TL_YELLOW_MINOR;
        if (yellow&&SPEED2DIST(vLinkWait)+myState.myPos<laneLength) {
            myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkWait, vLinkWait, false));
            checkRewindLinkLanes(lengthsInFront);
            return;
        }
        // valid, when a vehicle is not on a priorised lane
        bool setRequest = false;
        if (!(*link)->havePriority()) {
            // if it has already decelerated to let priorised vehicles pass
            //  and when the distance to the vehicle on the next lane allows moving
            //  (the check whether other incoming vehicles may stop this one is done later)
            // then let it pass
            if ((myState.mySpeed<ACCEL2SPEED(myType->getMaxDecel())||seen<myType->approachingBrakeGap(myState.mySpeed))&&r_dist2Pred>0) {
                vLinkPass = MIN3(vLinkPass, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed());
                setRequest = true;
            } else {
                // let it wait in the other cases
                vLinkPass = vLinkWait;
            }
        }
        // process stops
        if (!myStops.empty()&&myStops.begin()->lane->getEdge()==nextLane->getEdge()) {
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen+myStops.begin()->pos);
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
            setRequest = false;
        } else {
            setRequest |= (vLinkPass>0&&dist-seen>0);
        }
        myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkPass, vLinkWait, setRequest));
        if (vLinkPass>0&&dist-seen>0) {
        } else if (hadNonInternal) {
            checkRewindLinkLanes(lengthsInFront);
            return;
        }
        seen += nextLane->length();
        if (seen>dist&&hadNonInternal) {
            checkRewindLinkLanes(lengthsInFront);
            return;
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


Position2D
MSVehicle::getPosition() const {
    if (myLane==0) {
        return Position2D(-1000, -1000);
    }
    return myLane->getShape().positionAtLengthPosition(myState.pos());
}


const string &
MSVehicle::getID() const throw() {
    return myParameter->id;
}


bool
MSVehicle::onAllowed(const MSLane* lane) const {
    if (lane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if (!lane->allowsVehicleClass(myType->getVehicleClass())) {
        return false;
    }
    assert(myAllowedLanes.size()!=0);
    return (find(myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), lane) != myAllowedLanes[0]->end());
}


bool
MSVehicle::onAllowed() const {
    return onAllowed(myLane);
}


void
MSVehicle::enterLaneAtMove(MSLane* enteredLane, SUMOReal driven) {
#ifndef NO_TRACI
    // remove all Stops that were added by Traci and were not reached for any reason
    while (!myStops.empty()&&myStops.begin()->lane==myLane) {
        myStops.pop_front();
    }
#endif
    // move mover reminder one lane further
    adaptLaneEntering2MoveReminder(*enteredLane);
    // set the entered lane as the current lane
    myLane = enteredLane;
    myTarget = enteredLane;
    // proceed in route
    const MSEdge * const enteredEdge = enteredLane->getEdge();
    // internal edges are not a part of the route...
    if (enteredEdge->getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
        // we may have to skip edges, as the vehicle may have past them in one step
        //  (and, of course, at least one edge is passed)
        MSRouteIterator edgeIt = myCurrEdge;
        while (*edgeIt != enteredEdge) {
            ++edgeIt;
            assert(edgeIt != myRoute->end());
        }
        myCurrEdge = edgeIt;
    }

    // may be optimized: compute only, if the current or the next have more than one lane...!!!
    getBestLanes(true);
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->enterLaneAtMove(enteredLane, driven);
    }

#ifndef NO_TRACI
    checkForLaneChanges();
#endif
}


void
MSVehicle::enterLaneAtLaneChange(MSLane* enteredLane) {
#ifdef _MESSAGES
    if (myLCMsgEmitter!=0) {
        SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
        myLCMsgEmitter->writeLaneChangeEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), enteredLane, getPosition().x(), getPosition().y());
    }
#endif
    myLane = enteredLane;
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    myMoveReminders = enteredLane->getMoveReminders();
    rebuildAllowedLanes();
    activateRemindersByEmitOrLaneChange(false);
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->enterLaneAtLaneChange(enteredLane);
    }

#ifndef NO_TRACI
    // check if further changes are necessary
    checkForLaneChanges();
#endif
}


void
MSVehicle::enterLaneAtEmit(MSLane* enteredLane, SUMOReal pos, SUMOReal speed) {
    myState = State(pos, speed);
    assert(myState.myPos >= 0);
    assert(myState.mySpeed >= 0);
    myWaitingTime = 0;
    myLane = enteredLane;
    // set and activate the new lane's reminders
    myMoveReminders = enteredLane->getMoveReminders();
    activateRemindersByEmitOrLaneChange(true);
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->enterLaneAtEmit(enteredLane, myState);
    }


}


void
MSVehicle::leaveLaneAtMove(SUMOReal driven) {
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->leaveLaneAtMove(driven);
    }
    if (!myAllowedLanes.empty()) {
        myAllowedLanes.pop_front();
    }
}


void
MSVehicle::leaveLaneAtLaneChange(void) {
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->leaveLaneAtLaneChange();
    }
    // dismiss the old lane's reminders
    SUMOReal savePos = myState.myPos; // have to do this due to SUMOReal-precision errors
    vector< MSMoveReminder* >::iterator rem;
    for (rem=myMoveReminders.begin(); rem != myMoveReminders.end(); ++rem) {
        (*rem)->dismissByLaneChange(*this);
    }
    std::vector<SUMOReal>::iterator off = myOldLaneMoveReminderOffsets.begin();
    for (rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end(); ++rem, ++off) {
        myState.myPos += (*off);
        (*rem)->dismissByLaneChange(*this);
        myState.myPos -= (*off);
    }
    myState.myPos = savePos; // have to do this due to SUMOReal-precision errors
    myMoveReminders.clear();
    myOldLaneMoveReminders.clear();
    myOldLaneMoveReminderOffsets.clear();
}


const MSEdge * const
    MSVehicle::getEdge() const {
    return *myCurrEdge;
}


bool
MSVehicle::reachingCritical(SUMOReal laneLength) const {
    // check whether the vehicle will run over the lane when accelerating
    return (laneLength - myState.myPos - myType->brakeGap(myState.mySpeed)) <= 0;
}


MSLane *
MSVehicle::getTargetLane() const {
    return myTarget;
}


const MSLane &
MSVehicle::getLane() const {
    return *myLane;
}


MSVehicle *
MSVehicle::getNextPeriodical() const {
    // check whether another one shall be repated
    if (myParameter->repetitionNumber<=0) {
        return 0;
    }
    const MSRoute *route = myRoute;
    // in the case the vehicle was rerouted, give the next one the original route
    if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLD_REPETITION_ROUTE)!=myPointerCORNMap.end()) {
        route = (MSRoute*) myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLD_REPETITION_ROUTE)->second;
    }
    MSVehicleControl &vc = MSNet::getInstance()->getVehicleControl();
    SUMOVehicleParameter* p = new SUMOVehicleParameter(*myParameter);
    p->id = StringUtils::version1(p->id);
    while (vc.getVehicle(p->id)!=0) {
        p->id = StringUtils::version1(p->id);
    }
    p->depart += p->repetitionOffset;
    p->repetitionNumber--;
    MSVehicle *ret = vc.buildVehicle(p, route, myType);
    for (std::list<Stop>::const_iterator i=myStops.begin(); i!=myStops.end(); ++i) {
        ret->myStops.push_back(*i);
    }
    return ret;
}


MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() {
    return *myLaneChangeModel;
}


const MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() const {
    return *myLaneChangeModel;
}


unsigned int
MSVehicle::getWaitingTime() const {
    return myWaitingTime;
}


void
MSVehicle::onTripEnd(const MSLane * const lane) {
    if (lane!=0) {
        adaptLaneEntering2MoveReminder(*lane);
    }
    // check whether the vehicle's verbose arrival information shall be saved
    if (MSCORN::wished(MSCORN::CORN_VEH_ARRIVAL_INFO)) {
        DepartArrivalInformation *i = new DepartArrivalInformation();
        i->time = MSNet::getInstance()->getCurrentTimeStep();
        i->lane = myLane;
        i->pos = myState.pos();
        i->speed = myState.speed();
        myPointerCORNMap[MSCORN::CORN_P_VEH_ARRIVAL_INFO] = (void*) i;
    }
    SUMOReal pspeed = myState.mySpeed;
    SUMOReal pos = myState.myPos;
    SUMOReal oldPos = pos - SPEED2DIST(pspeed);
    // process reminder
    workOnMoveReminders(oldPos, pos, pspeed);
    // remove from structures to be informed about it
    for (QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    myQuitReminded.clear();
    for (vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->onTripEnd();
    }
}


void
MSVehicle::onDepart() {
    // check whether the vehicle's departure time shall be saved
    if (MSCORN::wished(MSCORN::CORN_VEH_DEPART_TIME)) {
        myIntCORNMap[MSCORN::CORN_VEH_DEPART_TIME] = (int) MSNet::getInstance()->getCurrentTimeStep();
    }
    // check whether the vehicle's verbose departure information shall be saved
    if (MSCORN::wished(MSCORN::CORN_VEH_DEPART_INFO)) {
        DepartArrivalInformation *i = new DepartArrivalInformation();
        i->time = MSNet::getInstance()->getCurrentTimeStep();
        i->lane = myLane;
        i->pos = myState.pos();
        i->speed = myState.speed();
        myPointerCORNMap[MSCORN::CORN_P_VEH_DEPART_INFO] = (void*) i;
    }
    // inform the vehicle control
    MSNet::getInstance()->getVehicleControl().vehicleEmitted(*this);
}


void
MSVehicle::quitRemindedEntered(MSVehicleQuitReminded *r) {
    myQuitReminded.push_back(r);
}


void
MSVehicle::quitRemindedLeft(MSVehicleQuitReminded *r) {
    QuitRemindedVector::iterator i = find(myQuitReminded.begin(), myQuitReminded.end(), r);
    if (i!=myQuitReminded.end()) {
        myQuitReminded.erase(i);
    }
}


const MSRoute &
MSVehicle::getRoute() const {
    return *myRoute;
}


const MSRoute &
MSVehicle::getRoute(int index) const {
    if (index==0) {
        return *myRoute;
    }
    --index; // only prior routes are stored
    std::map<MSCORN::Pointer, void*>::const_iterator i = myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE);
    assert(i!=myPointerCORNMap.end());
    const ReplacedRoutesVector * const v = (const ReplacedRoutesVector * const)(*i).second;
    assert((int) v->size()>index);
    return *((*v)[index].route);
}


bool
MSVehicle::replaceRoute(const MSEdgeVector &edges, SUMOTime simTime) {
    // assert the vehicle may continue (must not be "teleported" or whatever to another position)
    if (find(edges.begin(), edges.end(), *myCurrEdge)==edges.end()) {
        return false;
    }

    // build a new one
    // build a new id, first
    string id = getID();
    if (id[0]!='!') {
        id = "!" + id;
    }
    if (myRoute->getID().find("!var#")!=string::npos) {
        id = myRoute->getID().substr(0, myRoute->getID().rfind("!var#")+4) + toString(myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1);
    } else {
        id = id + "!var#1";
    }
    // build the route
    MSRoute *newRoute = new MSRoute(id, edges, false);
    // and add it to the container (!!!what for? It will never be used again!?)
    if (!MSRoute::dictionary(id, newRoute)) {
        delete newRoute;
        return false;
    }

    // save information about the current edge
    const MSEdge *currentEdge = *myCurrEdge;

    // ... maybe the route information shall be saved for output?
    if (MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        RouteReplaceInfo rri(*myCurrEdge, simTime, new MSRoute(*myRoute));//new MSRoute("!", myRoute->getEdges(), false));
        if (myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE)==myPointerCORNMap.end()) {
            myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE] = new ReplacedRoutesVector();
        }
        ((ReplacedRoutesVector*) myPointerCORNMap[MSCORN::CORN_P_VEH_OLDROUTE])->push_back(rri);
    }

    // check whether the old route may be deleted (is not used by anyone else)
    if (!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    } else {
        myPointerCORNMap[MSCORN::CORN_P_VEH_OLD_REPETITION_ROUTE] = (void*) myRoute;
    }

    // assign new route
    myRoute = newRoute;
    // rebuild in-vehicle route information
    myCurrEdge = myRoute->find(currentEdge);
    myLastBestLanesEdge = 0;
    // update arrival definition
    myArrivalPos = myParameter->arrivalPos;
    SUMOReal lastLaneLength = (*(myRoute->getLastEdge()->getLanes()))[0]->length();
    if (myArrivalPos < 0) {
        myArrivalPos += lastLaneLength; // !!! validate!
    }
    if (myArrivalPos<0) {
        myArrivalPos = 0;
    }
    if (myArrivalPos>lastLaneLength) {
        myArrivalPos = lastLaneLength;
    }
    // save information that the vehicle was rerouted
    //  !!! refactor the CORN-stuff
    myIntCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
    myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] = myIntCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
    // recheck stops
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end();) {
        if (find(edges.begin(), edges.end(), iter->lane->getEdge())==edges.end()) {
            iter = myStops.erase(iter);
        } else {
            ++iter;
        }
    }
    rebuildAllowedLanes();
    return true;
}


const MSVehicleType &
MSVehicle::getVehicleType() const {
    return *myType;
}


void
MSVehicle::rebuildAllowedLanes(bool reinit) {
    if (reinit) {
        myAllowedLanes.clear();
    }
    SUMOReal dist = 0;
    // check what was already computed
    for (NextAllowedLanes::const_iterator i=myAllowedLanes.begin(); i!=myAllowedLanes.end(); ++i) {
        dist += ((*(*i))[0])->length();
    }
    // compute next allowed lanes up to 1000m into the future
    SUMOReal MIN_DIST = 1000;
    if (dist<MIN_DIST) {
        unsigned int pos = (unsigned int) (distance(myRoute->begin(), myCurrEdge) + myAllowedLanes.size());
        if (pos>=myRoute->size()-1) {
            return;
        }
        const MSEdge::LaneCont *al = (*myRoute)[pos]->allowedLanes(*(*myRoute)[pos+1], myType->getVehicleClass());
        while (al!=0&&dist<MIN_DIST&&pos<myRoute->size()-1) {
            assert(al!=0);
            myAllowedLanes.push_back(al);
            pos++;
            if (pos<myRoute->size()-1) {
                dist += ((*al)[0])->length();
                al = (*myRoute)[pos]->allowedLanes(*(*myRoute)[pos+1], myType->getVehicleClass());
            }
        }
    }
    if (myAllowedLanes.size()==0&&myCurrEdge!=myRoute->end()) {
        unsigned int pos = (unsigned int) distance(myRoute->begin(), myCurrEdge);
        throw ProcessError("Route of vehicle '" + getID() + "' is invalid:\nCould not find a valid connection between edges '" + (*myRoute)[pos]->getID() + "' and '" + (*myRoute)[pos+1]->getID() + "'.");
    }
}

void
MSVehicle::rebuildContinuationsFor(LaneQ &oq, MSLane *l, MSRouteIterator ce, int seen) const {
    // check whether the end of iteration was reached
    ++ce;
    // we end if one of the following cases is true:
    // a) we have examined the next edges for 3000m (foresight distance)
    //     but only if we have at least examined the next edge
    // b) if we have examined 8 edges in front (!!! this may be shorted)
    // c) if the route does not continue after the seen edges
    if ((seen>4 && oq.length+l->length()>3000) || seen>8 || ce==myRoute->end()) {
        // ok, we have rebuilt this so far... do not have to go any further
        return;
    }
    // we must go further...
    // get the list of allowed lanes
    const MSEdge::LaneCont *allowed = 0;
    if (ce!=myRoute->end()&&ce+1!=myRoute->end()) {
        allowed = (*ce)->allowedLanes(**(ce+1), myType->getVehicleClass());
    }
    // determined recursively what the best lane is
    //  save the best lane for later usage
    LaneQ best;
    best.length = 0;
    const MSEdge::LaneCont * const lanes = (*ce)->getLanes();
    const MSLinkCont &lc = l->getLinkCont();
    bool gotOne = false;
    // we go through all connections of the lane to examine
    for (MSLinkCont::const_iterator k=lc.begin(); k!=lc.end(); ++k) {
        // prese values
        LaneQ q;
        MSLane *qqq = (*k)->getLane();
        if (qqq==0) {
            q.occupied = 0;
            q.length = 0;
            continue;
        }
        q.occupied = qqq->getVehLenSum();
        q.length = qqq->length();
        q.joined.push_back(qqq);


        if (!myStops.empty()&&myStops.front().lane->getEdge()==qqq->getEdge()) {
            if (myStops.front().lane==qqq) {
                gotOne = true;
                if (allowed==0||find(allowed->begin(), allowed->end(), (*k)->getLane())!=allowed->end()) {
                    rebuildContinuationsFor(q, qqq, ce, seen+1);
                }
            } else {
                q.occupied = qqq->getVehLenSum();
                const Stop &s = myStops.front();
                SUMOReal endPos = s.pos;
                if (s.busstop!=0) {
                    // on bus stops, we have to wait for free place if they are in use...
                    endPos = s.busstop->getLastFreePos();
                }
                q.length = endPos;
            }
        } else {
            // check whether the lane is allowed for route continuation (has a link to the next
            //  edge in route)
            if (allowed==0||find(allowed->begin(), allowed->end(), (*k)->getLane())!=allowed->end()) {
                // yes -> compute the best lane combination for consecutive lanes
                gotOne = true;
                rebuildContinuationsFor(q, qqq, ce, seen+1);
            } else {
                // no -> if the lane belongs to an edge not in our route,
                //  reset values to zero (otherwise the lane but not its continuations)
                //  will still be regarded
                if ((*k)->getLane()->getEdge()!=*(ce)) {
                    q.occupied = 0;
                    q.length = 0;
                }
            }
        }
        // set best lane information
        if (q.length>best.length) {
            best = q;
        }
    }
    // check whether we need to change the lane on this edge in any case
    if (!gotOne) {
        // yes, that's the case - we are on an edge on which we have to change
        //  the lane no matter which lanes we are using so far.
        // we have to tell the vehicle the best lane so far...
        // the assumption is that we only have to find the first one
        //  - because the vehicle has to change lanes, it will do this into
        //  the proper direction as the lanes moving the the proper edge are
        //  lying side by side
        const MSEdge::LaneCont * const lanes = (*ce)->getLanes();
        bool oneFound = false;
        int bestPos = 0;
        MSLane *next = 0;
        // we go over the next edge's lanes and determine the first that may be used
        for (MSEdge::LaneCont::const_iterator i=lanes->begin(); !oneFound&&i!=lanes->end();) {
            if (allowed!=0 && find(allowed->begin(), allowed->end(), *i)!=allowed->end()) {
                oneFound = true;
                next = *i;
            } else {
                ++i;
                ++bestPos;
            }
        }
        // ... it is now stored in next and its position in bestPos
        if (oneFound) {
            // ok, we have found a best lane
            //  (in fact, this should be the case if the route is valid, nonetheless...)
            // now let's say that the best lane is the nearest one to the found
            int bestDistance = -100;
            MSLane *bestL = 0;
            // go over all lanes of current edge
            const MSEdge::LaneCont * const clanes = l->getEdge()->getLanes();
            for (MSEdge::LaneCont::const_iterator i=clanes->begin(); i!=clanes->end(); ++i) {
                // go over all connected lanes
                for (MSLinkCont::const_iterator k=lc.begin(); k!=lc.end(); ++k) {
                    if ((*k)->getLane()==0) {
                        continue;
                    }
                    // the best lane must be on the proper edge
                    if ((*k)->getLane()->getEdge()==*(ce)) {
                        MSEdge::LaneCont::const_iterator l=find(lanes->begin(), lanes->end(), (*k)->getLane());
                        if (l!=lanes->end()) {
                            int pos = (int)distance(lanes->begin(), l);
                            int cdist = abs(pos-bestPos);
                            if (bestDistance==-100||bestDistance>cdist) {
                                bestDistance = cdist;
                                bestL = *i;
                            }
                        }
                    }
                }
            }
            if (bestL==l) {
                best.occupied = next->getVehLenSum();
                best.length = next->length();
            } else {
                best.occupied = 0;
                best.length = 0;
                best.joined.clear();
            }
        }
    }
    oq.length += best.length;
    oq.occupied += best.occupied;
    copy(best.joined.begin(), best.joined.end(), back_inserter(oq.joined));
}



const std::vector<MSVehicle::LaneQ> &
MSVehicle::getBestLanes(bool forceRebuild, MSLane *startLane) const throw() {
    if (startLane==0) {
        startLane = myLane;
    }
    if (myLastBestLanesEdge==startLane->getEdge()&&!forceRebuild) {
        std::vector<LaneQ> &lanes = *myBestLanes.begin();
        std::vector<LaneQ>::iterator i;
        for (i=lanes.begin(); i!=lanes.end(); ++i) {
            SUMOReal v = 0;
            for (std::vector<MSLane*>::const_iterator j=(*i).joined.begin(); j!=(*i).joined.end(); ++j) {
                v += (*j)->getVehLenSum();
            }
            (*i).v = v;
            if ((*i).lane==startLane) {
                myCurrentLaneInBestLanes = i;
            }
        }
        return *myBestLanes.begin();
    }
    myLastBestLanesEdge = startLane->getEdge();
    myBestLanes.clear();
    myBestLanes.push_back(vector<LaneQ>());
    const MSEdge::LaneCont * const lanes = (*myCurrEdge)->getLanes();
    MSRouteIterator ce = myCurrEdge;
    int seen = 0;
    const MSEdge::LaneCont *allowed = 0;
    if (ce!=myRoute->end()&&ce+1!=myRoute->end()) {
        allowed = (*ce)->allowedLanes(**(ce+1), myType->getVehicleClass());
    }
    for (MSEdge::LaneCont::const_iterator i=lanes->begin(); i!=lanes->end(); ++i) {
        LaneQ q;
        q.lane = *i;
        q.length = 0;//q.lane->length();
        q.occupied = 0;//q.lane->getVehLenSum();
        if (!myStops.empty()&&myStops.front().lane->getEdge()==q.lane->getEdge()) {
            if (myStops.front().lane==q.lane) {
                q.allowsContinuation = allowed==0||find(allowed->begin(), allowed->end(), q.lane)!=allowed->end();
                q.length += q.lane->length();
                q.occupied += q.lane->getVehLenSum();
            } else {
                q.allowsContinuation = false;
                q.occupied = q.lane->getVehLenSum();
                const Stop &s = myStops.front();
                SUMOReal endPos = s.pos;
                if (s.busstop!=0) {
                    // on bus stops, we have to wait for free place if they are in use...
                    endPos = s.busstop->getLastFreePos();
                }
                q.length = endPos;
            }
        } else {
            q.allowsContinuation = allowed==0||find(allowed->begin(), allowed->end(), q.lane)!=allowed->end();
        }
        myBestLanes[0].push_back(q);
    }
    if (ce!=myRoute->end()) {
        for (std::vector<MSVehicle::LaneQ>::iterator i=myBestLanes.begin()->begin(); i!=myBestLanes.begin()->end(); ++i) {
            if ((*i).allowsContinuation) {
                rebuildContinuationsFor((*i), (*i).lane, ce, seen);
                (*i).length += (*i).lane->length();
                (*i).occupied += (*i).lane->getVehLenSum();
            }
        }
    }
    SUMOReal best = 0;
    int index = 0;
    int run = 0;
    for (std::vector<MSVehicle::LaneQ>::iterator i=myBestLanes.begin()->begin(); i!=myBestLanes.begin()->end(); ++i, ++run) {
        if (best<(*i).length) {
            best = (*i).length;
            index = run;
        }
        if ((*i).lane==startLane) {
            myCurrentLaneInBestLanes = i;
        }
    }
    run = 0;
    for (std::vector<MSVehicle::LaneQ>::iterator i=myBestLanes.begin()->begin(); i!=myBestLanes.begin()->end(); ++i, ++run) {
        (*i).bestLaneOffset =  index - run;
    }

    return *myBestLanes.begin();
}


void
MSVehicle::writeXMLRoute(OutputDevice &os, int index) const {
    // check if a previous route shall be written
    if (index>=0) {
        os << "      <replaced_route";
        std::map<MSCORN::Pointer, void*>::const_iterator i = myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE);
        assert(i!=myPointerCORNMap.end());
        const ReplacedRoutesVector *v = (const ReplacedRoutesVector *)(*i).second;
        assert((int) v->size()>index);
        // write edge on which the vehicle was when the route was valid
        os << " replacedOnEdge=\"" << (*v)[index].edge->getID() << "\" ";
        // write the time at which the route was replaced
        os << " replacedAtTime=\"" << (*v)[index].time << "\" edges=\"";
        // get the route
        for (int i=0; i<index; ++i) {
            (*v)[i].route->writeEdgeIDs(os, (*v)[i].edge);
        }
        (*v)[index].route->writeEdgeIDs(os);
        os << "\"/>\n";
    } else {
        os << "      <route edges=\"";
        if (hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            int noReroutes = getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
            std::map<MSCORN::Pointer, void*>::const_iterator it = myPointerCORNMap.find(MSCORN::CORN_P_VEH_OLDROUTE);
            assert(it!=myPointerCORNMap.end());
            const ReplacedRoutesVector *v = (const ReplacedRoutesVector *)(*it).second;
            assert((int) v->size()==noReroutes);
            for (int i=0; i<noReroutes; ++i) {
                (*v)[i].route->writeEdgeIDs(os, (*v)[i].edge);
            }
        }
        myRoute->writeEdgeIDs(os);
        os << "\"/>\n";
    }
}


void
MSVehicle::saveState(std::ostream &os) {
    FileHelpers::writeString(os, myParameter->id);
    FileHelpers::writeInt(os, myLastLaneChangeOffset);
    FileHelpers::writeUInt(os, myWaitingTime);
    FileHelpers::writeInt(os, myParameter->repetitionNumber);
    FileHelpers::writeInt(os, myParameter->repetitionOffset);
    FileHelpers::writeString(os, myRoute->getID());
    FileHelpers::writeUInt(os, myParameter->depart); // !!! SUMOTime
    FileHelpers::writeString(os, myType->getID());
    FileHelpers::writeUInt(os, myRoute->posInRoute(myCurrEdge));
    if (hasCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME)) {
        FileHelpers::writeInt(os, getCORNIntValue(MSCORN::CORN_VEH_DEPART_TIME));
    } else {
        FileHelpers::writeInt(os, -1);
    }
#ifdef HAVE_MESOSIM
    // !!! several things may be missing
    if (mySegment==0) {
        FileHelpers::writeUInt(os, 0);
    } else {
        FileHelpers::writeUInt(os, mySegment->getIndex());
    }
    FileHelpers::writeFloat(os, myEventTime);
    FileHelpers::writeFloat(os, myLastEntryTime);
#endif
}




void
MSVehicle::removeOnTripEnd(MSVehicle *veh) throw() {
    quitRemindedLeft(veh);
}



bool
MSVehicle::willPass(const MSEdge * const edge) const {
    return find(myCurrEdge, myRoute->end(), edge)!=myRoute->end();
}


void
MSVehicle::reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle> &router) {
    // check whether to reroute
    std::vector<const MSEdge*> edges;
    router.compute(*myCurrEdge, myRoute->getLastEdge(), (const MSVehicle * const) this,
                   MSNet::getInstance()->getCurrentTimeStep(), edges);
    // check whether the new route is the same as the prior
    MSRouteIterator ri = myCurrEdge;
    std::vector<const MSEdge*>::iterator ri2 = edges.begin();
    while (ri!=myRoute->end()&&ri2!=edges.end()&&*ri==*ri2) {
        ++ri;
        ++ri2;
    }
    if (ri!=myRoute->end()||ri2!=edges.end()) {
        replaceRoute(edges, MSNet::getInstance()->getCurrentTimeStep());
    }
}

SUMOReal
MSVehicle::getEffort(const MSEdge * const e, SUMOTime t) const {
#ifndef NO_TRACI
    if (myTraciEdgeWeights.find(e)!=myTraciEdgeWeights.end()) {
        return myTraciEdgeWeights.find(e)->second;
    }
#endif
    for (vector< MSDevice* >::const_iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        MSDevice_C2C *c2cd = dynamic_cast<MSDevice_C2C*>(*dev);
        if (c2cd!=0) {
            SUMOReal deviceEffort = c2cd->getEffort(e, this, t);
            if (deviceEffort >= 0) {
                return deviceEffort; // the first device wins
            }
        }
    }
    return -1;
}


const std::vector<MSLane*> &
MSVehicle::getBestLanesContinuation() const throw() {
    if (myBestLanes.empty()||myBestLanes[0].empty()||myLane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return myEmptyLaneVector;
    }
    return (*myCurrentLaneInBestLanes).joined;
}


const std::vector<MSLane*> &
MSVehicle::getBestLanesContinuation(const MSLane * const l) const throw() {
    for (std::vector<std::vector<LaneQ> >::const_iterator i=myBestLanes.begin(); i!=myBestLanes.end(); ++i) {
        if ((*i).size()!=0&&(*i)[0].lane==l) {
            return (*i)[0].joined;
        }
    }
    return myEmptyLaneVector;
}



SUMOReal
MSVehicle::getDistanceToPosition(SUMOReal destPos, const MSEdge* destEdge) {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    SUMOReal distance = 1000000.;
#else
    SUMOReal distance = std::numeric_limits<SUMOReal>::max();
#endif
    if (isOnRoad() && destEdge != NULL) {
        if (myLane->getEdge() == (*myCurrEdge)) {
            // vehicle is on a normal edge
            distance = myRoute->getDistanceBetween(getPositionOnLane(), destPos, *myCurrEdge, destEdge);
        } else {
            // vehicle is on inner junction edge
            distance = myLane->length() - getPositionOnLane();
            distance += myRoute->getDistanceBetween(0, destPos, *(myCurrEdge+1), destEdge);
        }
    }
    return distance;
}

void
MSVehicle::setWasVaporized(bool onDepart) {
    if (MSCORN::wished(MSCORN::CORN_VEH_VAPORIZED)) {
        myIntCORNMap[MSCORN::CORN_VEH_VAPORIZED] = onDepart ? 1 : 0;
    }
}




#ifndef NO_TRACI

void
MSVehicle::checkReroute(SUMOTime t) {

#ifdef HAVE_INTERNAL_LANES
    // delay any rerouting while we're on an internal lane
    // otherwise, we'd mess up our route, plus there's not much we could do anyway
    if (myLane != 0 && myLane->getEdge() != (*myCurrEdge)) return;
#endif
    if (myNeedReroute && myStops.size()==0) {
        myNeedReroute = false;
        DijkstraRouterTT_Direct<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >
        router(MSEdge::dictSize(), true, &MSEdge::getVehicleEffort);
        reroute(t, router);
    }
}

bool
MSVehicle::changeEdgeWeightLocally(MSEdge* edge, SUMOReal travelTime) {
    SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    SUMOReal effortBefore = edge->getVehicleEffort(this, currentTime);
    if (travelTime < 0) {
        EdgeWeightMap::iterator oldWeight = myTraciEdgeWeights.find(edge);
        if (oldWeight == myTraciEdgeWeights.end()) {
            return false;
        }
        myTraciEdgeWeights.erase(oldWeight);
    } else {
        myTraciEdgeWeights[edge] = travelTime;
    }
    SUMOReal effortAfter = edge->getVehicleEffort(this, currentTime);
    if (!myNeedReroute && (effortBefore != effortAfter)) {
        // there is only a need to reroute if either the weight decreases or the edge is on our current route
        myNeedReroute = (effortBefore > effortAfter) ^ willPass(edge);
    }
    return true;
}


bool
MSVehicle::startSpeedAdaption(float newSpeed, SUMOTime duration, SUMOTime currentTime) {
    if (newSpeed < 0 || duration <= 0/* || newSpeed >= getSpeed()*/) {
        return false;
    }
    speedBeforeAdaption = getSpeed();
    timeBeforeAdaption = currentTime;
    adaptDuration = duration;
    speedReduction = MAX2((SUMOReal) 0.0f, (SUMOReal)(speedBeforeAdaption - newSpeed));
    adaptingSpeed = true;
    return true;
}


void
MSVehicle::adaptSpeed() {
    SUMOReal maxSpeed = 0;
    SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    if (!adaptingSpeed) {
        return;
    }
    if (isLastAdaption) {
        unsetIndividualMaxSpeed();
        adaptingSpeed = false;
        isLastAdaption = false;
        return;
    }
    if (currentTime <= timeBeforeAdaption + adaptDuration) {
        maxSpeed = speedBeforeAdaption - (speedReduction / adaptDuration)
                   * (currentTime - timeBeforeAdaption);
    } else {
        maxSpeed = speedBeforeAdaption - speedReduction;
        isLastAdaption = true;
    }
    setIndividualMaxSpeed(maxSpeed);
}


void
MSVehicle::checkLaneChangeConstraint(SUMOTime time) {
    if (!laneChangeConstraintActive) {
        return;
    }
    if ((time - timeBeforeLaneChange) >= laneChangeStickyTime) {
        laneChangeConstraintActive = false;
    }
}


void
MSVehicle::startLaneChange(unsigned lane, SUMOTime stickyTime) {
    if (lane < 0) {
        return;
    }
    timeBeforeLaneChange = MSNet::getInstance()->getCurrentTimeStep();
    laneChangeStickyTime = stickyTime;
    myDestinationLane = lane;
    laneChangeConstraintActive = true;
    checkForLaneChanges();
}


void
MSVehicle::checkForLaneChanges() {
    MSLane* tmpLane;
    unsigned currentLaneIndex = 0;
    if (!laneChangeConstraintActive) {
        myLaneChangeModel->requestLaneChange(REQUEST_NONE);
        return;
    }
    if ((*myCurrEdge)->nLanes() <= myDestinationLane) {
        laneChangeConstraintActive = false;
        return;
    }
    tmpLane = myLane;
    while ((tmpLane =tmpLane->getRightLane()) != NULL) {
        currentLaneIndex++;
    }
    if (currentLaneIndex > myDestinationLane) {
        myLaneChangeModel->requestLaneChange(REQUEST_RIGHT);
    } else if (currentLaneIndex < myDestinationLane) {
        myLaneChangeModel->requestLaneChange(REQUEST_LEFT);
    } else {
        myLaneChangeModel->requestLaneChange(REQUEST_HOLD);
    }
}


void
MSVehicle::processTraCICommands(SUMOTime time) {
    // try to reroute in case of previous "changeRoute" messages
    checkReroute(time);
    // check for applied lane changing constraints
    checkLaneChangeConstraint(time);
    // change speed in case of previous "slowDown" command
    adaptSpeed();
}


bool
MSVehicle::addTraciStop(MSLane* lane, SUMOReal pos, SUMOReal radius, SUMOTime duration) {
    //if the stop exists update the duration
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end(); iter++) {
        if (iter->lane == lane && fabs(iter->pos - pos) < POSITION_EPS) {
            if (duration == 0 && !iter->reached) {
                myStops.erase(iter);
            } else {
                iter->duration = duration;
            }
            return true;
        }
    }

    Stop newStop;
    newStop.lane = lane;
    newStop.pos = pos;
    newStop.duration = duration;
    newStop.until = -1;
    newStop.reached = false;
    newStop.busstop = MSNet::getInstance()->getBusStop(lane, pos);
    return addStop(newStop);
}


#endif


/****************************************************************************/

