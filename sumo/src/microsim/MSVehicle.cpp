/****************************************************************************/
/// @file    MSVehicle.cpp
/// @author  Christian Roessel
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a vehicle in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include "MSPersonControl.h"
#include <utils/common/RandHelper.h>
#include "devices/MSDevice_Person.h"
#include "MSEdgeWeightsStorage.h"
#include <utils/common/HelpersHBEFA.h>
#include <utils/common/HelpersHarmonoise.h>

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
// static value definitions
// ===========================================================================
std::vector<MSLane*> MSVehicle::myEmptyLaneVector;


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
 * methods of MSVehicle::Influencer
 * ----------------------------------------------------------------------- */
MSVehicle::Influencer::Influencer() 
            : mySpeedAdaptationStarted(true)
{}


MSVehicle::Influencer::~Influencer()
{}


void 
MSVehicle::Influencer::setSpeedTimeLine(const std::vector<std::pair<SUMOTime, SUMOReal> > &speedTimeLine) {
    mySpeedAdaptationStarted = true;
    mySpeedTimeLine = speedTimeLine;
}


void 
MSVehicle::Influencer::setLaneTimeLine(const std::vector<std::pair<SUMOTime, int> > &laneTimeLine) {
    myLaneTimeLine = laneTimeLine;
}


void 
MSVehicle::Influencer::influenceSpeed(SUMOTime currentTime, SUMOReal &speed) {
    // keep original speed
    myOriginalSpeed = speed;
    // remove leading commands which are no longer valid
    while(mySpeedTimeLine.size()==1 || (mySpeedTimeLine.size()>1&&currentTime>mySpeedTimeLine[1].first)) {
        mySpeedTimeLine.erase(mySpeedTimeLine.begin());
    }
    // do nothing if the time line does not apply for the current time
    if(mySpeedTimeLine.size()<2||currentTime<mySpeedTimeLine[0].first) {
        return;
    }
    // compute and set new speed
    if(!mySpeedAdaptationStarted) {
        mySpeedTimeLine[0].second = speed;
        mySpeedAdaptationStarted = true;
    } else {
        const SUMOReal td = STEPS2TIME(currentTime - mySpeedTimeLine[0].first) / STEPS2TIME(mySpeedTimeLine[1].first - mySpeedTimeLine[0].first);
        speed = mySpeedTimeLine[0].second - (mySpeedTimeLine[0].second-mySpeedTimeLine[1].second) * td;
    }
}


void 
MSVehicle::Influencer::influenceLane(SUMOTime currentTime, int &lane) {
}



/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle() throw() {
    delete myLaneChangeModel;
    // other
    delete myEdgeWeights;
    for (std::vector<MSLane*>::iterator i=myFurtherLanes.begin(); i!=myFurtherLanes.end(); ++i) {
        (*i)->resetPartialOccupation(this);
    }
    for (DriveItemVector::iterator i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        if ((*i).myLink!=0) {
            (*i).myLink->removeApproaching(this);
        }
    }
    myFurtherLanes.clear();
    //
    if (myType->amVehicleSpecific()) {
        delete myType;
    }
}


MSVehicle::MSVehicle(SUMOVehicleParameter* pars,
                     const MSRoute* route,
                     const MSVehicleType* type,
                     int /*vehicleIndex*/) throw(ProcessError) :
        MSBaseVehicle(pars, route, type),
        myLastLaneChangeOffset(0),
        myTarget(0),
        myWaitingTime(0),
        myState(0, 0), //
        myLane(0),
        myLastBestLanesEdge(0),
        myPreDawdleAcceleration(0),
        myEdgeWeights(0),
        mySignals(0),
        myPersonDevice(0),
        myAmOnNet(false),
        myAmRegisteredAsWaitingForPerson(false)
#ifndef NO_TRACI
        ,myInfluencer(0),
        timeBeforeLaneChange(0),
        laneChangeStickyTime(0),
        laneChangeConstraintActive(false),
        myDestinationLane(0)
#endif
{
    for (std::vector<SUMOVehicleParameter::Stop>::iterator i=pars->stops.begin(); i!=pars->stops.end(); ++i) {
        if (!addStop(*i)) {
            throw ProcessError("Stop for vehicle '" + pars->id +
                               "' on lane '" + i->lane + "' is not downstream the current route.");
        }
    }
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i=route->getStops().begin(); i!=route->getStops().end(); ++i) {
        if (!addStop(*i)) {
            throw ProcessError("Stop for vehicle '" + pars->id +
                               "' on lane '" + i->lane + "' is not downstream the current route.");
        }
    }
    const MSLane* const depLane = (*myCurrEdge)->getDepartLane(*this);
    if (depLane == 0) {
        throw ProcessError("Invalid departlane definition for vehicle '" + pars->id + "'.");
    }
    if (pars->departSpeedProcedure == DEPART_SPEED_GIVEN && pars->departSpeed > depLane->getMaxSpeed()) {
        throw ProcessError("Departure speed for vehicle '" + pars->id +
                           "' is too high for the departure lane '" + depLane->getID() + "'.");
    }
#ifdef _MESSAGES
    myLCMsgEmitter = MSNet::getInstance()->getMsgEmitter("lanechange");
    myBMsgEmitter = MSNet::getInstance()->getMsgEmitter("break");
    myHBMsgEmitter = MSNet::getInstance()->getMsgEmitter("heartbeat");
#endif
    myLaneChangeModel = new MSLCM_DK2004(*this);
}


void
MSVehicle::onRemovalFromNet(const MSMoveReminder::Notification reason) throw() {
    workOnMoveReminders(myState.myPos - SPEED2DIST(myState.mySpeed), myState.myPos, myState.mySpeed);
    for (DriveItemVector::iterator i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        if ((*i).myLink!=0) {
            (*i).myLink->removeApproaching(this);
        }
    }
    leaveLane(reason);
}


// ------------ interaction with the route
bool
MSVehicle::ends() const throw() {
    return myCurrEdge==myRoute->end()-1 && myState.myPos > myArrivalPos - POSITION_EPS;
}


bool
MSVehicle::replaceRoute(const MSRoute* newRoute, bool onInit) throw() {
    const MSEdgeVector &edges = newRoute->getEdges();
    // assert the vehicle may continue (must not be "teleported" or whatever to another position)
    if (!onInit && !newRoute->contains(*myCurrEdge)) {
        return false;
    }

    // rebuild in-vehicle route information
    if (onInit) {
        myCurrEdge = newRoute->begin();
    } else {
        myCurrEdge = find(edges.begin(), edges.end(), *myCurrEdge);
    }
    // check whether the old route may be deleted (is not used by anyone else)
    newRoute->addReference();
    myRoute->release();
    // assign new route
    myRoute = newRoute;
    myLastBestLanesEdge = 0;
    // update arrival definition
    calculateArrivalPos();
    // save information that the vehicle was rerouted
    myNumberReroutes++;
    MSNet::getInstance()->informVehicleStateListener(this, MSNet::VEHICLE_STATE_NEWROUTE);
    // recheck stops
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end();) {
        if (find(edges.begin(), edges.end(), &iter->lane->getEdge())==edges.end()) {
            iter = myStops.erase(iter);
        } else {
            ++iter;
        }
    }
    return true;
}


bool
MSVehicle::willPass(const MSEdge * const edge) const throw() {
    return find(myCurrEdge, myRoute->end(), edge)!=myRoute->end();
}


MSEdgeWeightsStorage &
MSVehicle::getWeightsStorage() throw() {
    if (myEdgeWeights==0) {
        myEdgeWeights = new MSEdgeWeightsStorage();
    }
    return *myEdgeWeights;
}


// ------------ Interaction with move reminders
void
MSVehicle::workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderCont::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!rem->first->notifyMove(*this, oldPos+rem->second, newPos+rem->second, newSpeed)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
}


void
MSVehicle::adaptLaneEntering2MoveReminder(const MSLane &enteredLane) throw() {
    // save the old work reminders, patching the position information
    //  add the information about the new offset to the old lane reminders
    const SUMOReal oldLaneLength = myLane->getLength();
    for (MoveReminderCont::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end(); ++rem) {
        rem->second += oldLaneLength;
    }
    for (std::vector< MSMoveReminder* >::const_iterator rem=enteredLane.getMoveReminders().begin(); rem!=enteredLane.getMoveReminders().end(); ++rem) {
        addReminder(*rem);
    }
}


void
MSVehicle::activateReminders(const MSMoveReminder::Notification reason) throw() {
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderCont::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (rem->first->getLane() != 0 && rem->first->getLane() != myLane) {
            ++rem;
        } else {
            if (!rem->first->notifyEnter(*this, reason)) {
                rem = myMoveReminders.erase(rem);
            } else {
                ++rem;
            }
        }
    }
}


// ------------ Other getter methods
Position2D
MSVehicle::getPosition() const throw() {
    if (myLane==0) {
        return Position2D(-1000, -1000);
    }
    return myLane->getShape().positionAtLengthPosition(myState.pos());
}


SUMOReal
MSVehicle::getAngle() const throw() {
    Position2D p1 = myLane->getShape().positionAtLengthPosition(myState.pos());
    Position2D p2 = myFurtherLanes.size()>0
                    ? myFurtherLanes.front()->getShape().positionAtLengthPosition(myFurtherLanes.front()->getPartialOccupatorEnd())
                    : myLane->getShape().positionAtLengthPosition(myState.pos()-myType->getLength());
    if (p1!=p2) {
        return atan2(p1.x()-p2.x(), p2.y()-p1.y())*180./PI;
    } else {
        return -myLane->getShape().rotationDegreeAtLengthPosition(getPositionOnLane());
    }
}


// ------------
bool
MSVehicle::addStop(const SUMOVehicleParameter::Stop &stopPar, SUMOTime untilOffset) throw() {
    Stop stop;
    stop.lane = MSLane::dictionary(stopPar.lane);
    stop.busstop = MSNet::getInstance()->getBusStop(stopPar.busstop);
    stop.startPos = stopPar.startPos;
    stop.endPos = stopPar.endPos;
    stop.duration = stopPar.duration;
    stop.until = stopPar.until;
    if (stop.until != -1) {
        stop.until += untilOffset;
    }
    stop.triggered = stopPar.triggered;
    stop.parking = stopPar.parking;
    stop.reached = false;
    if (stop.startPos < 0 || stop.endPos > stop.lane->getLength()) {
        return false;
    }
    stop.edge = find(myCurrEdge, myRoute->end(), &stop.lane->getEdge());
    MSRouteIterator prevStopEdge = myCurrEdge;
    SUMOReal prevStopPos = myState.myPos;
    // where to insert the stop
    std::list<Stop>::iterator iter = myStops.begin();
    if (stopPar.index == STOP_INDEX_END || stopPar.index >= static_cast<int>(myStops.size())) {
        if (myStops.size() > 0) {
            prevStopEdge = myStops.back().edge;
            prevStopPos = myStops.back().endPos;
            iter = myStops.end();
            stop.edge = find(prevStopEdge, myRoute->end(), &stop.lane->getEdge());
        }
    } else {
        if (stopPar.index == STOP_INDEX_FIT) {
            while (iter != myStops.end() && (iter->edge < stop.edge ||
                                             (iter->endPos < stop.endPos && iter->edge == stop.edge))) {
                prevStopEdge = iter->edge;
                prevStopPos = iter->endPos;
                ++iter;
            }
        } else {
            int index = stopPar.index;
            while (index > 0) {
                prevStopEdge = iter->edge;
                prevStopPos = iter->endPos;
                ++iter;
                --index;
            }
            stop.edge = find(prevStopEdge, myRoute->end(), &stop.lane->getEdge());
        }
    }
    if (stop.edge == myRoute->end() || prevStopEdge > stop.edge || prevStopEdge == stop.edge && prevStopPos > stop.endPos) {
        return false;
    }
    if (myCurrEdge == stop.edge && myState.myPos > stop.endPos - getCarFollowModel().brakeGap(myState.mySpeed)) {
        return false;
    }
    myStops.insert(iter, stop);
    return true;
}


bool
MSVehicle::isStopped() const {
    return !myStops.empty() && myStops.begin()->reached;
}


bool
MSVehicle::isParking() const {
    return isStopped() && myStops.begin()->parking;
}


SUMOReal
MSVehicle::processNextStop(SUMOReal currentVelocity) throw() {
    if (myStops.empty()) {
        // no stops; pass
        return currentVelocity;
    }
    Stop &stop = myStops.front();
    if (stop.reached) {
        // ok, we have already reached the next stop
        // any waiting persons may board now
        bool boarded = MSNet::getInstance()->getPersonControl().boardAnyWaiting(&myLane->getEdge(), this);
        if (boarded) {
            // the triggering condition has been fulfilled. Maybe we want to wait a bit longer for additional riders (car pooling)
            stop.triggered = false;
            if (myAmRegisteredAsWaitingForPerson) {
                MSNet::getInstance()->getVehicleControl().unregisterOneWaitingForPerson();
                myAmRegisteredAsWaitingForPerson = false;
            }
        }
        if (stop.duration<=0 && !stop.triggered) {
            // we have waited long enough and fulfilled any passenger-requirements
            if (stop.busstop!=0) {
                // inform bus stop about leaving it
                stop.busstop->leaveFrom(this);
            }
            // the current stop is no longer valid
            MSNet::getInstance()->getVehicleControl().removeWaiting(&myLane->getEdge(), this);
            myStops.pop_front();
            // do not count the stopping time towards gridlock time.
            // Other outputs use an independent counter and are not affected.
            myWaitingTime = 0;
            // maybe the next stop is on the same edge; let's rebuild best lanes
            getBestLanes(true);
            // continue as wished...
        } else {
            // we have to wait some more time
            if (stop.triggered && !myAmRegisteredAsWaitingForPerson) {
                // we can only register after waiting for one step. otherwise we might falsely signal a deadlock
                MSNet::getInstance()->getVehicleControl().registerOneWaitingForPerson();
                myAmRegisteredAsWaitingForPerson = true;
            }
            stop.duration -= DELTA_T;
            return 0;
        }
    } else {
        // is the next stop on the current lane?
        if (stop.edge==myCurrEdge) {
            // get the stopping position
            SUMOReal endPos = stop.endPos;
            bool busStopsMustHaveSpace = true;
            if (stop.busstop!=0) {
                // on bus stops, we have to wait for free place if they are in use...
                endPos = stop.busstop->getLastFreePos();
                if (endPos-5.<stop.busstop->getBeginLanePosition()) { // !!! explicite offset
                    busStopsMustHaveSpace = false;
                }
            }
            if (myState.pos()>=endPos-BUS_STOP_OFFSET && busStopsMustHaveSpace) {
                // ok, we may stop (have reached the stop)
                stop.reached = true;
                MSNet::getInstance()->getVehicleControl().addWaiting(&myLane->getEdge(), this);
                // compute stopping time
                if (stop.until>=0) {
                    if (stop.duration==-1) {
                        stop.duration = stop.until - MSNet::getInstance()->getCurrentTimeStep();
                    } else {
                        stop.duration = MAX2(stop.duration, stop.until - MSNet::getInstance()->getCurrentTimeStep());
                    }
                }
                if (stop.busstop!=0) {
                    // let the bus stop know the vehicle
                    stop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                }
            }
            // decelerate
            // !!! should not v be 0 when we have reached the stop?
            return getCarFollowModel().ffeS(this, endPos-myState.pos());
        }
    }
    return currentVelocity;
}


bool
MSVehicle::moveRegardingCritical(SUMOTime t, const MSLane* const lane,
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
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
    }
#endif
    myTarget = 0;
    for (DriveItemVector::iterator i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        if ((*i).myLink!=0) {
            (*i).myLink->removeApproaching(this);
        }
    }
    myLFLinkLanes.clear();
    const MSCFModel &cfModel = getCarFollowModel();
    // check whether the vehicle is not on an appropriate lane
    if (!myLane->appropriate(this)) {
        // decelerate to lane end when yes
        SUMOReal vWish = MIN2(cfModel.ffeS(this, myLane->getLength() - myState.myPos), myLane->getMaxSpeed());
        if (myCurrEdge == myRoute->end() - 1) {
            if (myParameter->arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                vWish = MIN2(cfModel.ffeV(this, myArrivalPos - myState.myPos, myParameter->arrivalSpeed), myLane->getMaxSpeed());
            } else {
                vWish = myLane->getMaxSpeed();
            }
        }
        if (pred!=0) {
            // interaction with leader if one exists on same lane
            SUMOReal gap = gap2pred(*pred);
            if (MSGlobals::gCheck4Accidents && gap < 0/*-POSITION_EPS*/) {
                // collision occured!
                return true;
            }
            vWish = MIN2(vWish, cfModel.ffeV(this, pred));
        } else {
            // (potential) interaction with a vehicle extending partially into this lane
            MSVehicle *predP = myLane->getPartialOccupator();
            if (predP!=0) {
                SUMOReal gap = myLane->getPartialOccupatorEnd() - myState.myPos;
                if (MSGlobals::gCheck4Accidents && gap < 0/*-POSITION_EPS*/) {
                    // collision occured!
                    return true;
                }
                vWish = MIN2(vWish, cfModel.ffeV(this, gap, predP->getSpeed()));
            }
        }
        // interaction with left-lane leader (do not overtake right)
        cfModel.leftVehicleVsafe(this, neigh, vWish);
        // check whether the vehicle wants to stop somewhere
        if (!myStops.empty()&& &myStops.begin()->lane->getEdge()==&lane->getEdge()) {
            const Stop &stop = *myStops.begin();
            SUMOReal stopPos = stop.busstop==0
                               ? stop.endPos
                               : stop.busstop->getLastFreePos()-POSITION_EPS;
            SUMOReal seen = lane->getLength() - myState.pos();
            SUMOReal vsafeStop = cfModel.ffeS(this, seen-(lane->getLength()-stopPos));
            vWish = MIN2(vWish, vsafeStop);
        }
        vWish = MAX2((SUMOReal) 0, vWish);
        myLFLinkLanes.push_back(DriveProcessItem(0, vWish, vWish, false, 0, 0, myLane->getLength()-myState.myPos));
    } else {
        // compute other values as in move
        SUMOReal vBeg = MIN2(cfModel.maxNextSpeed(myState.mySpeed), lane->getMaxSpeed());//vaccel( myState.mySpeed, lane->maxSpeed() );
        if (pred!=0) {
            SUMOReal gap = gap2pred(*pred);
            if (MSGlobals::gCheck4Accidents && gap < 0/*-POSITION_EPS*/) {
                // collision occured!
                return true;
            }
            SUMOReal vSafe = cfModel.ffeV(this, gap, pred->getSpeed());
            //  the vehicle is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            vBeg = MIN2(vBeg, vSafe);
        } else {
            // (potential) interaction with a vehicle extending partially into this lane
            MSVehicle *predP = myLane->getPartialOccupator();
            if (predP!=0) {
                SUMOReal gap = myLane->getPartialOccupatorEnd() - myState.myPos;
                if (MSGlobals::gCheck4Accidents && gap < 0/*-POSITION_EPS*/) {
                    // collision occured!
                    return true;
                }
                vBeg = MIN2(vBeg, cfModel.ffeV(this, gap, predP->getSpeed()));
            }
        }
        cfModel.leftVehicleVsafe(this, neigh, vBeg); // from left-lane leader (do not overtake right)
        // check whether the driver wants to let someone in
        // set next links, computing possible speeds
        vsafeCriticalCont(t, vBeg);
    }
    //@todo to be optimized (move to somewhere else)
    checkRewindLinkLanes(lengthsInFront);
    return false;
}


bool
MSVehicle::moveFirstChecked() {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
    }
#endif
    myTarget = 0;
    // get vsafe
    SUMOReal vSafe = 0;

    assert(myLFLinkLanes.size()!=0);
    DriveItemVector::iterator i;
    bool braking = false;
    bool lastWasGreenCont = false;
    for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        MSLink *link = (*i).myLink;
        // the vehicle must change the lane on one of the next lanes
        if (link!=0&&(*i).mySetRequest) {
            const MSLink::LinkState ls = link->getState();
            // vehicles should brake when running onto a yellow light if the distance allows to halt in front
            const bool yellow = ls==MSLink::LINKSTATE_TL_YELLOW_MAJOR||ls==MSLink::LINKSTATE_TL_YELLOW_MINOR;
            if (yellow&&(*i).myDistance>getCarFollowModel().getSpeedAfterMaxDecel(myState.mySpeed)) {
                vSafe = (*i).myVLinkWait;
                braking = true;
                lastWasGreenCont = false;
                link->removeApproaching(this);
                break;
            }
            //
            const bool opened = link->opened((*i).myArrivalTime, (*i).myArrivalSpeed, getVehicleType().getLength());
            // vehicles should decelerate when approaching a minor link
            if (opened&&!lastWasGreenCont&&!link->havePriority()&&(*i).myDistance>getCarFollowModel().getMaxDecel()) {
                vSafe = (*i).myVLinkWait;
                braking = true;
                lastWasGreenCont = false;
                if (ls==MSLink::LINKSTATE_EQUAL) {
                    link->removeApproaching(this);
                }
                break; // could be revalidated
            }
            // have waited; may pass if opened...
            if (opened) {
                vSafe = (*i).myVLinkPass;
                lastWasGreenCont = link->isCont()&&(ls==MSLink::LINKSTATE_TL_GREEN_MAJOR);
            } else {
                lastWasGreenCont = false;
                vSafe = (*i).myVLinkWait;
                braking = true;
                if (ls==MSLink::LINKSTATE_EQUAL) {
                    link->removeApproaching(this);
                }
                break;
            }
        } else {
            vSafe = (*i).myVLinkWait;
            braking = true;
            break;
        }
    }

    SUMOReal vNext = getCarFollowModel().moveHelper(this, myLane, vSafe);
    vNext = MAX2(vNext, (SUMOReal) 0.);
#ifndef NO_TRACI
    if(myInfluencer!=0) {
        myInfluencer->influenceSpeed(MSNet::getInstance()->getCurrentTimeStep(), vNext);
    }
#endif
    // visit waiting time
    if (vNext<=0.1) {
        myWaitingTime += DELTA_T;
        braking = true;
    } else {
        myWaitingTime = 0;
    }
    if (myState.mySpeed<vNext) {
        braking = false;
    }
    if (braking) {
        switchOnSignal(VEH_SIGNAL_BRAKELIGHT);
    } else {
        switchOffSignal(VEH_SIGNAL_BRAKELIGHT);
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
        if (vNext < myState.mySpeed) {
            SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
            myBMsgEmitter->writeBreakEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), getPosition().x(), getPosition().y());
        }
    }
#endif
    // update position and speed
    myState.myPos += SPEED2DIST(vNext);
    myState.mySpeed = vNext;
    myTarget = 0;
    std::vector<MSLane*> passedLanes;
    for (std::vector<MSLane*>::reverse_iterator i=myFurtherLanes.rbegin(); i!=myFurtherLanes.rend(); ++i) {
        passedLanes.push_back(*i);
    }
    if (passedLanes.size()==0||passedLanes.back()!=myLane) {
        passedLanes.push_back(myLane);
    }
    // move on lane(s)
    if (myState.myPos<=myLane->getLength()) {
        // we are staying at our lane
        //  there is no need to go over succeeding lanes
        workOnMoveReminders(pos, pos + SPEED2DIST(vNext), vNext);
    } else {
        // we are moving at least to the next lane (maybe pass even more than one)
        if (myCurrEdge != myRoute->end() - 1) {
            MSLane *approachedLane = myLane;
            // move the vehicle forward
            for (i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end() && myState.myPos>approachedLane->getLength(); ++i) {
                if (approachedLane!=myLane) {
                    leaveLane(MSMoveReminder::NOTIFICATION_JUNCTION);
                }
                MSLink *link = (*i).myLink;
                // check whether the vehicle was allowed to enter lane
                //  otherwise it is decelareted and we do not need to test for it's
                //  approach on the following lanes when a lane changing is performed
                assert(approachedLane!=0);
                myState.myPos -= approachedLane->getLength();
                assert(myState.myPos>0);
                if (approachedLane!=myLane) {
                    if (enterLaneAtMove(approachedLane)) {
                        myLane = approachedLane;
                        return true;
                    }
                }
                // proceed to the next lane
                if (link!=0) {
#ifdef HAVE_INTERNAL_LANES
                    approachedLane = link->getViaLane();
                    if (approachedLane==0) {
                        approachedLane = link->getLane();
                    }
#else
                    approachedLane = link->getLane();
#endif
                }
                passedLanes.push_back(approachedLane);
            }
            myTarget = approachedLane;
        }
    }
    // clear previously set information
    for (std::vector<MSLane*>::iterator i=myFurtherLanes.begin(); i!=myFurtherLanes.end(); ++i) {
        (*i)->resetPartialOccupation(this);
    }
    myFurtherLanes.clear();
    if (myState.myPos-getVehicleType().getLength()<0&&passedLanes.size()>0) {
        SUMOReal leftLength = getVehicleType().getLength()-myState.myPos;
        std::vector<MSLane*>::reverse_iterator i=passedLanes.rbegin() + 1;
        while (leftLength>0&&i!=passedLanes.rend()) {
            myFurtherLanes.push_back(*i);
            leftLength -= (*i)->setPartialOccupation(this, leftLength);
            ++i;
        }
    }
    assert(myTarget==0||myTarget->getLength()>=myState.myPos);
    setBlinkerInformation();
    return false;
}


SUMOReal
MSVehicle::getSpaceTillLastStanding(MSLane *l, bool &foundStopped) throw() {
    SUMOReal lengths = 0;
    const MSLane::VehCont &vehs = l->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
        if ((*i)->getSpeed()<.1) {
            foundStopped = true;
            SUMOReal ret = (*i)->getPositionOnLane() - (*i)->getVehicleType().getLength() - lengths;
            l->releaseVehicles();
            return ret;
        }
        lengths += (*i)->getVehicleType().getLength();
    }
    l->releaseVehicles();
    return l->getLength() - lengths;
}


void
MSVehicle::checkRewindLinkLanes(SUMOReal lengthsInFront) throw() {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
        if (MSNet::getInstance()->getCurrentTimeStep()==152000) {
            bla = 0;
        }
    }
#endif
#ifdef HAVE_INTERNAL_LANES
    if (MSGlobals::gUsingInternalLanes) {
        int removalBegin = -1;
        bool hadVehicle = false;
        SUMOReal seenSpace = -lengthsInFront;

        std::vector<SUMOReal> availableSpace;
        std::vector<bool> hadVehicles;
        bool foundStopped = false;

        for (unsigned int i=0; i<myLFLinkLanes.size(); ++i) {
            // skip unset links
            DriveProcessItem &item = myLFLinkLanes[i];
            if (item.myLink==0||foundStopped) {
                availableSpace.push_back(seenSpace);
                hadVehicles.push_back(hadVehicle);
                continue;
            }
            // get the next lane, determine whether it is an internal lane
            MSLane *approachedLane = item.myLink->getViaLane();
            if (approachedLane!=0) {
                if (item.myLink->isCrossing()&&item.myLink->willHaveBlockedFoe()) {
                    seenSpace = seenSpace - approachedLane->getVehLenSum();
                    hadVehicle |= approachedLane->getVehicleNumber()!=0;
                } else {
                    seenSpace = seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getVehLenSum() + approachedLane->getLength();
                    hadVehicle |= approachedLane->getVehicleNumber()!=0;
                }
                availableSpace.push_back(seenSpace);
                hadVehicles.push_back(hadVehicle);
                continue;
            }
            approachedLane = item.myLink->getLane();
            MSVehicle *last = approachedLane->getLastVehicle();
            if (last==0) {
                last = approachedLane->getPartialOccupator();
                if (last!=0) {
                    SUMOReal m = MAX2(seenSpace, seenSpace + approachedLane->getPartialOccupatorEnd() + last->getCarFollowModel().brakeGap(last->getSpeed()));
                    availableSpace.push_back(m);
                    hadVehicle = true;
                    seenSpace = seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getVehLenSum() + approachedLane->getLength();
                } else {
//                    seenSpace = seenSpace - approachedLane->getVehLenSum() + approachedLane->getLength();
//                    availableSpace.push_back(seenSpace);
                    availableSpace.push_back(seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped));
                    if (!foundStopped) {
                        seenSpace = seenSpace - approachedLane->getVehLenSum() + approachedLane->getLength();
                    } else {
                        seenSpace = availableSpace.back();
                    }
                }
            } else {
                if (last->signalSet(VEH_SIGNAL_BRAKELIGHT)) {
                    SUMOReal lastBrakeGap = last->getCarFollowModel().brakeGap(approachedLane->getLastVehicle()->getSpeed());
                    SUMOReal lastGap = last->getPositionOnLane() - last->getVehicleType().getLength() + lastBrakeGap - last->getSpeed()*last->getCarFollowModel().getTau();
                    SUMOReal m = MAX2(seenSpace, seenSpace + lastGap);
                    availableSpace.push_back(m);
                    seenSpace = seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getVehLenSum() + approachedLane->getLength();
                } else {
//                    seenSpace = seenSpace - approachedLane->getVehLenSum() + approachedLane->getLength();
//                    availableSpace.push_back(seenSpace);
                    availableSpace.push_back(seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped));
                    if (!foundStopped) {
                        seenSpace = seenSpace - approachedLane->getVehLenSum() + approachedLane->getLength();
                    } else {
                        seenSpace = availableSpace.back();
                    }
                }
                hadVehicle = true;
            }
            hadVehicles.push_back(hadVehicle);
        }
#ifdef DEBUG_VEHICLE_GUI_SELECTION
        if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
            int bla = 0;
        }
#endif
        SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
        for (int i= (int)(myLFLinkLanes.size()-1); i>0; --i) {
            DriveProcessItem &item = myLFLinkLanes[i-1];
            if (item.myLink==0||item.myLink->isCont()||item.myLink->opened(t, .1, getVehicleType().getLength())||!hadVehicles[i]) {
                availableSpace[i-1] = availableSpace[i];
            }
        }

        for (unsigned int i=0; hadVehicle&&i<myLFLinkLanes.size()&&removalBegin<0; ++i) {
            // skip unset links
            DriveProcessItem &item = myLFLinkLanes[i];
            if (item.myLink==0) {
                continue;
            }
            if (/*!item.myLink->isCont()
                    &&*/availableSpace[i]-getVehicleType().getLength()<0
                &&item.myLink->willHaveBlockedFoe()) {
                removalBegin = i;
            }
        }
        if (removalBegin!=-1&&!(removalBegin==0&&myLane->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL)) {
            while (removalBegin< (int)(myLFLinkLanes.size())) {
                myLFLinkLanes[removalBegin].myVLinkPass = myLFLinkLanes[removalBegin].myVLinkWait;
                myLFLinkLanes[removalBegin].mySetRequest = false;
                ++removalBegin;
            }
        }
    }
#endif
    for (DriveItemVector::iterator i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); ++i) {
        if ((*i).myLink!=0) {
            (*i).myLink->setApproaching(this, (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).mySetRequest);
        }
    }
}



void
MSVehicle::vsafeCriticalCont(SUMOTime t, SUMOReal boundVSafe) {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
    }
#endif
    const MSCFModel &cfModel = getCarFollowModel();
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    SUMOReal seen = myLane->getLength() - myState.myPos;
    SUMOReal seenNonInternal = 0;
    //
    if (this!=myLane->getFirstVehicle() && seen - cfModel.brakeGap(myState.mySpeed) > 0 && seen - SPEED2DIST(boundVSafe) - ACCEL2DIST(cfModel.getMaxAccel()) > 0) {
        // not "reaching critical"
        myLFLinkLanes.push_back(DriveProcessItem(0, boundVSafe, boundVSafe, false, 0, 0, seen));
        return;
    }

    MSLane *nextLane = myLane;
    // compute the way the vehicle would drive if it would use the current speed and then
    //  decelerate
    SUMOReal maxV = cfModel.maxNextSpeed(myState.mySpeed);
    SUMOReal dist = SPEED2DIST(maxV) + cfModel.brakeGap(maxV);//myState.mySpeed);
    SUMOReal vLinkPass = boundVSafe;
    SUMOReal vLinkWait = vLinkPass;
    const std::vector<MSLane*> &bestLaneConts = getBestLanesContinuation();
#ifdef HAVE_INTERNAL_LANES
    bool hadNonInternal = false;
#else
    bool hadNonInternal = true;
#endif

    unsigned int view = 1;
    // loop over following lanes
    while (true) {
        // process stops
        if (!myStops.empty()&& &myStops.begin()->lane->getEdge()==&nextLane->getEdge()) {
            SUMOReal vsafeStop = cfModel.ffeS(this, seen-(nextLane->getLength()-myStops.begin()->endPos));
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
        }

        // get the next link used
        MSLinkCont::const_iterator link = myLane->succLinkSec(*this, view, *nextLane, bestLaneConts);

        // check whether the lane is a dead end
        //  (should be valid only on further loop iterations
        if (nextLane->isLinkEnd(link)) {
            SUMOReal laneEndVSafe = cfModel.ffeS(this, seen);
            if (myCurrEdge + view == myRoute->end()) {
                if (myParameter->arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                    laneEndVSafe = cfModel.ffeV(this, seen, myParameter->arrivalSpeed);
                } else {
                    laneEndVSafe = vLinkPass;
                }
            }
            // the vehicle will not drive further
            myLFLinkLanes.push_back(DriveProcessItem(0, MIN2(vLinkPass, laneEndVSafe), MIN2(vLinkPass, laneEndVSafe), false, 0, 0, seen));
            return;
        }
        // the link was passed
        vLinkWait = vLinkPass;


        // get the following lane
#ifdef HAVE_INTERNAL_LANES
        nextLane = (*link)->getViaLane();
        if (nextLane==0) {
            nextLane = (*link)->getLane();
            hadNonInternal = true;
            view++;
        }
#else
        nextLane = (*link)->getLane();
        view++;
#endif

        // compute the velocity to use when the link is not blocked by oter vehicles
        //  the vehicle shall be not faster when reaching the next lane than allowed
        SUMOReal vmaxNextLane = MAX2(cfModel.ffeV(this, seen, nextLane->getMaxSpeed()), nextLane->getMaxSpeed());

        // the vehicle shall keep a secure distance to its predecessor
        //  (or approach the lane end if the predeccessor is too near)
        SUMOReal vsafePredNextLane = 100000;
        std::pair<MSVehicle*, SUMOReal> lastOnNext = nextLane->getLastVehicleInformation();
        if (lastOnNext.first!=0) {
            if (seen+lastOnNext.second>=0) {
                vsafePredNextLane = cfModel.ffeV(this, seen+lastOnNext.second, lastOnNext.first->getSpeed());
            } else {
                vsafePredNextLane = cfModel.ffeS(this, seen);
            }
        }
#ifdef DEBUG_VEHICLE_GUI_SELECTION
        if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
            int bla = 0;
        }
#endif
        // compute the velocity to use when the link may be used
        vLinkPass = MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane);

        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait = MIN3(vLinkPass, vLinkWait, cfModel.ffeS(this, seen));

        // behaviour in front of not priorised intersections (waiting for priorised foe vehicles)
        bool setRequest = false;
        // process stops
        if (!myStops.empty()&& &myStops.begin()->lane->getEdge()==&nextLane->getEdge()) {
            const Stop &stop = *myStops.begin();
            const SUMOReal vsafeStop = stop.busstop==0
                                       ? cfModel.ffeS(this, seen+stop.endPos)
                                       : cfModel.ffeS(this, seen+stop.busstop->getLastFreePos()-POSITION_EPS);
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
        }
        // check whether we approach the final edge
        if (myCurrEdge + view == myRoute->end()) {
            if (myParameter->arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
                const SUMOReal vsafe = cfModel.ffeV(this, seen + myArrivalPos, myParameter->arrivalSpeed);
                vLinkPass = MIN2(vLinkPass, vsafe);
                vLinkWait = MIN2(vLinkWait, vsafe);
            }
        }

        setRequest |= ((*link)->getState()!=MSLink::LINKSTATE_TL_RED&&(vLinkPass>0&&dist-seen>0));
        bool yellow = (*link)->getState()==MSLink::LINKSTATE_TL_YELLOW_MAJOR||(*link)->getState()==MSLink::LINKSTATE_TL_YELLOW_MINOR;
        bool red = (*link)->getState()==MSLink::LINKSTATE_TL_RED;
        if ((yellow||red)&&seen>cfModel.brakeGap(myState.mySpeed)-SPEED2DIST(myState.mySpeed)*cfModel.getTau()) { // !!! we should reuse brakeGap with no reaction time...
            vLinkPass = vLinkWait;
            setRequest = false;
            myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkWait, vLinkWait, false, t+TIME2STEPS(seen / vLinkPass), vLinkPass, seen));
        }
        // the next condition matches the previously one used for determining the difference
        //  between critical/non-critical vehicles. Though, one should assume that a vehicle
        //  should want to move over an intersection even though it could brake before it!?
        setRequest &= dist-seen>0;
        myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkPass, vLinkWait, setRequest, t + TIME2STEPS(seen / vLinkPass), vLinkPass, seen));
        seen += nextLane->getLength();
        seenNonInternal += nextLane->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL ? 0 : nextLane->getLength();
        if ((vLinkPass<=0||seen>dist)&&hadNonInternal&&seenNonInternal>50) {
            return;
        }
    }
}


bool
MSVehicle::enterLaneAtMove(MSLane* enteredLane, bool onTeleporting) {
    myAmOnNet = true;
    // vaporizing edge?
    if (enteredLane->getEdge().isVaporizing()) {
        // yep, let's do the vaporization...
        myLane = enteredLane;
        return true;
    }
    if (!onTeleporting) {
        // move mover reminder one lane further
        adaptLaneEntering2MoveReminder(*enteredLane);
        // set the entered lane as the current lane
        myLane = enteredLane;
    }

    // internal edges are not a part of the route...
    if (enteredLane->getEdge().getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
        ++myCurrEdge;
    }
    if (!onTeleporting) {
        // may be optimized: compute only, if the current or the next have more than one lane...!!!
        getBestLanes(true);
        activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION);
#ifndef NO_TRACI
        checkForLaneChanges();
#endif
    }
    return ends();//myCurrEdge == myRoute->end() - 1 && getPositionOnLane() > myArrivalPos - POSITION_EPS;
    //return myCurrEdge==myRoute->end()-1 && myState.myPos > myArrivalPos - POSITION_EPS;
}


void
MSVehicle::enterLaneAtLaneChange(MSLane* enteredLane) {
    myAmOnNet = true;
#ifdef _MESSAGES
    if (myLCMsgEmitter!=0) {
        SUMOReal timeStep = MSNet::getInstance()->getCurrentTimeStep();
        myLCMsgEmitter->writeLaneChangeEvent(myParameter->id, timeStep, myLane, myState.pos(), myState.speed(), enteredLane, getPosition().x(), getPosition().y());
    }
#endif
    myLane = enteredLane;
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    for (std::vector< MSMoveReminder* >::const_iterator rem=enteredLane->getMoveReminders().begin(); rem!=enteredLane->getMoveReminders().end(); ++rem) {
        addReminder(*rem);
    }
    activateReminders(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
    SUMOReal leftLength = myState.myPos-getVehicleType().getLength();
    if (leftLength<0) {
        // we have to rebuild "further lanes"
        const MSRoute &route = getRoute();
        MSRouteIterator i = myCurrEdge;
        MSLane *lane = myLane;
        while (i!=route.begin()&&leftLength>0) {
            const MSEdge * const prev = *(--i);
            const std::vector<MSLane::IncomingLaneInfo> &incomingLanes = lane->getIncomingLanes();
            for (std::vector<MSLane::IncomingLaneInfo>::const_iterator j=incomingLanes.begin(); j!=incomingLanes.end(); ++j) {
                if (&(*j).lane->getEdge()==prev) {
#ifdef HAVE_INTERNAL_LANES
                    (*j).lane->setPartialOccupation(this, leftLength);
#else
                    leftLength -= (*j).length;
                    (*j).lane->setPartialOccupation(this, leftLength);
#endif
                    leftLength -= (*j).lane->getLength();
                    break;
                }
            }
        }
    }
#ifndef NO_TRACI
    // check if further changes are necessary
    checkForLaneChanges();
#endif
}


void
MSVehicle::enterLaneAtInsertion(MSLane* enteredLane, SUMOReal pos, SUMOReal speed, MSMoveReminder::Notification notification) throw(ProcessError) {
    myState = State(pos, speed);
    assert(myState.myPos >= 0);
    assert(myState.mySpeed >= 0);
    myWaitingTime = 0;
    myLane = enteredLane;
    // set and activate the new lane's reminders
    for (std::vector< MSMoveReminder* >::const_iterator rem=enteredLane->getMoveReminders().begin(); rem!=enteredLane->getMoveReminders().end(); ++rem) {
        addReminder(*rem);
    }
    activateReminders(notification);
    std::string msg;
    if (MSGlobals::gCheckRoutes && !hasValidRoute(msg)) {
        throw ProcessError("Vehicle '" + getID() + "' has no valid route. " + msg);
    }
    myAmOnNet = true;
    // build the list of lanes the vehicle is lapping into
    SUMOReal leftLength = myType->getLength() - pos;
    MSLane *clane = enteredLane;
    while (leftLength>0) {
        clane = clane->getLogicalPredecessorLane();
        if (clane==0) {
            break;
        }
        myFurtherLanes.push_back(clane);
        leftLength -= (clane)->setPartialOccupation(this, leftLength);
    }
}


void
MSVehicle::leaveLane(const MSMoveReminder::Notification reason) {
    for (MoveReminderCont::iterator rem=myMoveReminders.begin(); rem!=myMoveReminders.end();) {
        if (!rem->first->notifyLeave(*this, myState.myPos + rem->second, reason)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        for (std::vector<MSLane*>::iterator i=myFurtherLanes.begin(); i!=myFurtherLanes.end(); ++i) {
            (*i)->resetPartialOccupation(this);
        }
        myFurtherLanes.clear();
    }
    if (reason>=MSMoveReminder::NOTIFICATION_TELEPORT) {
        myAmOnNet = false;
    }
}


MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() {
    return *myLaneChangeModel;
}


const MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() const {
    return *myLaneChangeModel;
}


const std::vector<MSVehicle::LaneQ> &
MSVehicle::getBestLanes(bool forceRebuild, MSLane *startLane) const throw() {
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int bla = 0;
        myLastBestLanesEdge = 0;
    }
#endif

    if (startLane==0) {
        startLane = myLane;
    }
    // update occupancy and current lane index, only, if the vehicle has not moved to a new lane
    if (myLastBestLanesEdge==&startLane->getEdge()&&!forceRebuild) {
        std::vector<LaneQ> &lanes = *myBestLanes.begin();
        std::vector<LaneQ>::iterator i;
        for (i=lanes.begin(); i!=lanes.end(); ++i) {
            SUMOReal nextOccupation = 0;
            for (std::vector<MSLane*>::const_iterator j=(*i).bestContinuations.begin()+1; j!=(*i).bestContinuations.end(); ++j) {
                nextOccupation += (*j)->getVehLenSum();
            }
            (*i).nextOccupation = nextOccupation;
            if ((*i).lane==startLane) {
                myCurrentLaneInBestLanes = i;
            }
        }
        return *myBestLanes.begin();
    }
    // start rebuilding
    myLastBestLanesEdge = &startLane->getEdge();
    myBestLanes.clear();

    // get information about the next stop
    const MSEdge *nextStopEdge = 0;
    const MSLane *nextStopLane = 0;
    SUMOReal nextStopPos = 0;
    if (!myStops.empty()) {
        const Stop &nextStop = myStops.front();
        nextStopLane = nextStop.lane;
        nextStopEdge = &nextStopLane->getEdge();
        nextStopPos = nextStop.startPos;
    }
    if (myParameter->arrivalLaneProcedure == ARRIVAL_LANE_GIVEN && nextStopEdge == 0) {
        nextStopEdge = *(myRoute->end() - 1);
        nextStopLane = nextStopEdge->getLanes()[myParameter->arrivalLane];
        nextStopPos = myArrivalPos;
    }

    // go forward along the next lanes;
    int seen = 0;
    SUMOReal seenLength = 0;
    bool progress = true;
    for (MSRouteIterator ce = myCurrEdge; progress;) {
        std::vector<LaneQ> currentLanes;
        const std::vector<MSLane*> *allowed = 0;
        const MSEdge *nextEdge = 0;
        if (ce!=myRoute->end()&&ce+1!=myRoute->end()) {
            nextEdge = *(ce+1);
            allowed = (*ce)->allowedLanes(*nextEdge, myType->getVehicleClass());
        }
        const std::vector<MSLane*> &lanes = (*ce)->getLanes();
        for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
            LaneQ q;
            MSLane *cl = *i;
            q.lane = cl;
            q.bestContinuations.push_back(cl);
            q.bestLaneOffset = 0;
            q.length = cl->getLength();
            q.allowsContinuation = allowed==0||find(allowed->begin(), allowed->end(), cl)!=allowed->end();
            currentLanes.push_back(q);
        }
        //
        if (nextStopEdge==*ce) {
            progress = false;
            for (std::vector<LaneQ>::iterator q=currentLanes.begin(); q!=currentLanes.end(); ++q) {
                if (nextStopLane!=(*q).lane) {
                    (*q).allowsContinuation = false;
                    (*q).length = nextStopPos;
                }
            }
        }

        myBestLanes.push_back(currentLanes);
        ++seen;
        seenLength += currentLanes[0].lane->getLength();
        ++ce;
        progress &= (seen<=4 || seenLength<3000);
        progress &= seen<=8;
        progress &= ce!=myRoute->end();
        /*
        if(progress) {
        	progress &= (currentLanes.size()!=1||(*ce)->getLanes().size()!=1);
        }
        */
    }

    // we are examining the last lane explicitely
    if (myBestLanes.size()!=0) {
        SUMOReal bestLength = -1;
        int bestThisIndex = 0;
        int index = 0;
        std::vector<LaneQ> &last = myBestLanes.back();
        for (std::vector<LaneQ>::iterator j=last.begin(); j!=last.end(); ++j, ++index) {
            if ((*j).length>bestLength) {
                bestLength = (*j).length;
                bestThisIndex = index;
            }
        }
        index = 0;
        for (std::vector<LaneQ>::iterator j=last.begin(); j!=last.end(); ++j, ++index) {
            if ((*j).length<bestLength) {
                (*j).bestLaneOffset = bestThisIndex - index;
            }
        }
    }

    // go backward through the lanes
    // track back best lane and compute the best prior lane(s)
    for (std::vector<std::vector<LaneQ> >::reverse_iterator i=myBestLanes.rbegin()+1; i!=myBestLanes.rend(); ++i) {
        std::vector<LaneQ> &nextLanes = (*(i-1));
        std::vector<LaneQ> &clanes = (*i);
        MSEdge &cE = clanes[0].lane->getEdge();
        int index = 0;
        SUMOReal bestConnectedLength = -1;
        SUMOReal bestLength = -1;
        for (std::vector<LaneQ>::iterator j=nextLanes.begin(); j!=nextLanes.end(); ++j, ++index) {
            if ((*j).lane->isApproachedFrom(&cE) && bestConnectedLength<(*j).length) {
                bestConnectedLength = (*j).length;
            }
            if (bestLength<(*j).length) {
                bestLength = (*j).length;
            }
        }
        if (bestConnectedLength>0) {
            int bestThisIndex = 0;
            index = 0;
            for (std::vector<LaneQ>::iterator j=clanes.begin(); j!=clanes.end(); ++j, ++index) {
                LaneQ bestConnectedNext;
                bestConnectedNext.length = -1;
                if ((*j).allowsContinuation) {
                    for (std::vector<LaneQ>::const_iterator m=nextLanes.begin(); m!=nextLanes.end(); ++m) {
                        if ((*m).lane->isApproachedFrom(&cE, (*j).lane)) {
                            if (bestConnectedNext.length<(*m).length || (bestConnectedNext.length==(*m).length&&abs(bestConnectedNext.bestLaneOffset)>abs((*m).bestLaneOffset))) {
                                bestConnectedNext = *m;
                            }
                        }
                    }
                    if (bestConnectedNext.length==bestConnectedLength&&abs(bestConnectedNext.bestLaneOffset)<2) {
                        (*j).length += bestLength;
                    } else {
                        (*j).length += bestConnectedNext.length;
                    }
                }
                if (clanes[bestThisIndex].length<(*j).length || (clanes[bestThisIndex].length==(*j).length&&abs(abs(clanes[bestThisIndex].bestLaneOffset>(*j).bestLaneOffset)))) {
                    bestThisIndex = index;
                }
                copy(bestConnectedNext.bestContinuations.begin(), bestConnectedNext.bestContinuations.end(), back_inserter((*j).bestContinuations));
            }

            index = 0;
            for (std::vector<LaneQ>::iterator j=clanes.begin(); j!=clanes.end(); ++j, ++index) {
                if ((*j).length<clanes[bestThisIndex].length || ((*j).length==clanes[bestThisIndex].length&&abs((*j).bestLaneOffset)<abs(clanes[bestThisIndex].bestLaneOffset))) {
                    (*j).bestLaneOffset = bestThisIndex - index;
                } else {
                    (*j).bestLaneOffset = 0;
                }
            }

        } else {

            int bestThisIndex = 0;
            int bestNextIndex = 0;
            int bestDistToNeeded = (int) clanes.size();
            index = 0;
            for (std::vector<LaneQ>::iterator j=clanes.begin(); j!=clanes.end(); ++j,++index) {
                if ((*j).allowsContinuation) {
                    int nextIndex = 0;
                    for (std::vector<LaneQ>::const_iterator m=nextLanes.begin(); m!=nextLanes.end(); ++m, ++nextIndex) {
                        if ((*m).lane->isApproachedFrom(&cE, (*j).lane)) {
                            if (bestDistToNeeded>abs((*m).bestLaneOffset)) {
                                bestDistToNeeded = abs((*m).bestLaneOffset);
                                bestThisIndex = index;
                                bestNextIndex = nextIndex;
                            }
                        }
                    }
                }
            }
            clanes[bestThisIndex].length += nextLanes[bestNextIndex].length;
            copy(nextLanes[bestNextIndex].bestContinuations.begin(), nextLanes[bestNextIndex].bestContinuations.end(), back_inserter(clanes[bestThisIndex].bestContinuations));
            index = 0;
            for (std::vector<LaneQ>::iterator j=clanes.begin(); j!=clanes.end(); ++j, ++index) {
                if ((*j).length<clanes[bestThisIndex].length || ((*j).length==clanes[bestThisIndex].length&&abs((*j).bestLaneOffset)<abs(clanes[bestThisIndex].bestLaneOffset))) {
                    (*j).bestLaneOffset = bestThisIndex - index;
                } else {
                    (*j).bestLaneOffset = 0;
                }
            }

        }

    }

    // update occupancy and current lane index
    std::vector<LaneQ> &currLanes = *myBestLanes.begin();
    std::vector<LaneQ>::iterator i;
    for (i=currLanes.begin(); i!=currLanes.end(); ++i) {
        SUMOReal nextOccupation = 0;
        for (std::vector<MSLane*>::const_iterator j=(*i).bestContinuations.begin()+1; j!=(*i).bestContinuations.end(); ++j) {
            nextOccupation += (*j)->getVehLenSum();
        }
        (*i).nextOccupation = nextOccupation;
        if ((*i).lane==startLane) {
            myCurrentLaneInBestLanes = i;
        }
    }
    return *myBestLanes.begin();
}


const std::vector<MSLane*> &
MSVehicle::getBestLanesContinuation() const throw() {
    if (myBestLanes.empty()||myBestLanes[0].empty()||myLane->getEdge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return myEmptyLaneVector;
    }
    return (*myCurrentLaneInBestLanes).bestContinuations;
}


const std::vector<MSLane*> &
MSVehicle::getBestLanesContinuation(const MSLane * const l) const throw() {
    for (std::vector<std::vector<LaneQ> >::const_iterator i=myBestLanes.begin(); i!=myBestLanes.end(); ++i) {
        if ((*i).size()!=0&&(*i)[0].lane==l) {
            return (*i)[0].bestContinuations;
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
        if (&myLane->getEdge() == *myCurrEdge) {
            // vehicle is on a normal edge
            distance = myRoute->getDistanceBetween(getPositionOnLane(), destPos, *myCurrEdge, destEdge);
        } else {
            // vehicle is on inner junction edge
            distance = myLane->getLength() - getPositionOnLane();
            distance += myRoute->getDistanceBetween(0, destPos, *(myCurrEdge+1), destEdge);
        }
    }
    return distance;
}


SUMOReal
MSVehicle::getHBEFA_CO2Emissions() const throw() {
    return HelpersHBEFA::computeCO2(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHBEFA_COEmissions() const throw() {
    return HelpersHBEFA::computeCO(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHBEFA_HCEmissions() const throw() {
    return HelpersHBEFA::computeHC(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHBEFA_NOxEmissions() const throw() {
    return HelpersHBEFA::computeNOx(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHBEFA_PMxEmissions() const throw() {
    return HelpersHBEFA::computePMx(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHBEFA_FuelConsumption() const throw() {
    return HelpersHBEFA::computeFuel(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


SUMOReal
MSVehicle::getHarmonoise_NoiseEmissions() const throw() {
    return HelpersHarmonoise::computeNoise(myType->getEmissionClass(), myState.speed(), myPreDawdleAcceleration);
}


void
MSVehicle::addPerson(MSPerson* person) throw() {
    if (myPersonDevice == 0) {
        myPersonDevice = MSDevice_Person::buildVehicleDevices(*this, myDevices);
        myMoveReminders.push_back(std::make_pair(myPersonDevice, 0.));
    }
    myPersonDevice->addPerson(person);
    if (myStops.size() > 0 && myStops.front().reached && myStops.front().triggered) {
        myStops.front().duration = 0;
    }
}


void
MSVehicle::setBlinkerInformation() throw() {
    switchOffSignal(VEH_SIGNAL_BLINKER_RIGHT|VEH_SIGNAL_BLINKER_LEFT);
    int state = getLaneChangeModel().getOwnState();
    if ((state&LCA_LEFT)!=0) {
        switchOnSignal(VEH_SIGNAL_BLINKER_LEFT);
    } else if ((state&LCA_RIGHT)!=0) {
        switchOnSignal(VEH_SIGNAL_BLINKER_RIGHT);
    } else {
        const MSLane*lane = getLane();
        MSLinkCont::const_iterator link = lane->succLinkSec(*this, 1, *lane, getBestLanesContinuation());
        if (link!=lane->getLinkCont().end()&&lane->getLength()-getPositionOnLane()<lane->getMaxSpeed()*(SUMOReal) 7.) {
            switch ((*link)->getDirection()) {
            case MSLink::LINKDIR_TURN:
            case MSLink::LINKDIR_LEFT:
            case MSLink::LINKDIR_PARTLEFT:
                switchOnSignal(VEH_SIGNAL_BLINKER_LEFT);
                break;
            case MSLink::LINKDIR_RIGHT:
            case MSLink::LINKDIR_PARTRIGHT:
                switchOnSignal(VEH_SIGNAL_BLINKER_RIGHT);
                break;
            default:
                break;
            }
        }
    }

}


void
MSVehicle::replaceVehicleType(MSVehicleType *type) throw() {
    if (myType->amVehicleSpecific()) {
        delete myType;
    }
    myType = type;
}


#ifndef NO_TRACI

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
    if ((unsigned int)(*myCurrEdge)->getLanes().size() <= myDestinationLane) {
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
    // check for applied lane changing constraints
    checkLaneChangeConstraint(time);
}


bool
MSVehicle::addTraciStop(MSLane* lane, SUMOReal pos, SUMOReal /*radius*/, SUMOTime duration) {
    //if the stop exists update the duration
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end(); iter++) {
        if (iter->lane == lane && fabs(iter->endPos - pos) < POSITION_EPS) {
            if (duration == 0 && !iter->reached) {
                myStops.erase(iter);
            } else {
                iter->duration = duration;
            }
            return true;
        }
    }

    SUMOVehicleParameter::Stop newStop;
    newStop.lane = lane->getID();
    newStop.busstop = MSNet::getInstance()->getBusStopID(lane, pos);
    newStop.startPos = pos - POSITION_EPS;
    newStop.endPos = pos;
    newStop.duration = duration;
    newStop.until = -1;
    newStop.triggered = false;
    newStop.parking = false;
    newStop.index = STOP_INDEX_END;
    return addStop(newStop);
}


MSVehicle::Influencer &
MSVehicle::getInfluencer() {
    if(myInfluencer==0) {
        myInfluencer = new Influencer();
    }
    return *myInfluencer;
}


SUMOReal 
MSVehicle::getSpeedWithoutTraciInfluence() const {
    if(myInfluencer!=0) {
        return myInfluencer->getOriginalSpeed();
    }
    return myState.mySpeed;
}


#endif


/****************************************************************************/
