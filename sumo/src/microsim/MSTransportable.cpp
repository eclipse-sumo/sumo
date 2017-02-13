/****************************************************************************/
/// @file    MSTransportable.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
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

#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSEdge.h"
#include "MSLane.h"
#include "MSNet.h"
#include <microsim/pedestrians/MSPerson.h>
#include "MSTransportableControl.h"
#include "MSTransportable.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
const SUMOReal MSTransportable::ROADSIDE_OFFSET(3);

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTransportable::Stage - methods
 * ----------------------------------------------------------------------- */
MSTransportable::Stage::Stage(const MSEdge& destination, MSStoppingPlace* toStop, const SUMOReal arrivalPos, StageType type)
    : myDestination(destination), myDestinationStop(toStop), myArrivalPos(arrivalPos), myDeparted(-1), myArrived(-1), myType(type) {}

MSTransportable::Stage::~Stage() {}

const MSEdge&
MSTransportable::Stage::getDestination() const {
    return myDestination;
}


void
MSTransportable::Stage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}

void
MSTransportable::Stage::setArrived(SUMOTime now) {
    myArrived = now;
}

bool
MSTransportable::Stage::isWaitingFor(const std::string& /*line*/) const {
    return false;
}

Position
MSTransportable::Stage::getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const {
    return getLanePosition(e->getLanes()[0], at, offset);
}

Position
MSTransportable::Stage::getLanePosition(const MSLane* lane, SUMOReal at, SUMOReal offset) const {
    return lane->getShape().positionAtOffset(lane->interpolateLanePosToGeometryPos(at), offset);
}

SUMOReal
MSTransportable::Stage::getEdgeAngle(const MSEdge* e, SUMOReal at) const {
    return e->getLanes()[0]->getShape().rotationAtOffset(at);
}


/* -------------------------------------------------------------------------
* MSTransportable::Stage_Waiting - methods
* ----------------------------------------------------------------------- */
MSTransportable::Stage_Waiting::Stage_Waiting(const MSEdge& destination,
        SUMOTime duration, SUMOTime until, SUMOReal pos, const std::string& actType,
        const bool initial) :
    MSTransportable::Stage(destination, 0, SUMOVehicleParameter::interpretEdgePos(
                               pos, destination.getLength(), SUMO_ATTR_DEPARTPOS, "stopping at " + destination.getID()),
                           initial ? WAITING_FOR_DEPART : WAITING),
    myWaitingDuration(duration),
    myWaitingUntil(until),
    myActType(actType) {
}


MSTransportable::Stage_Waiting::~Stage_Waiting() {}


const MSEdge*
MSTransportable::Stage_Waiting::getEdge() const {
    return &myDestination;
}


const MSEdge*
MSTransportable::Stage_Waiting::getFromEdge() const {
    return &myDestination;
}


SUMOReal
MSTransportable::Stage_Waiting::getEdgePos(SUMOTime /* now */) const {
    return myArrivalPos;
}


SUMOTime
MSTransportable::Stage_Waiting::getUntil() const {
    return myWaitingUntil;
}


Position
MSTransportable::Stage_Waiting::getPosition(SUMOTime /* now */) const {
    return getEdgePosition(&myDestination, myArrivalPos, ROADSIDE_OFFSET);
}


SUMOReal
MSTransportable::Stage_Waiting::getAngle(SUMOTime /* now */) const {
    return getEdgeAngle(&myDestination, myArrivalPos) + M_PI / 2;
}


void
MSTransportable::Stage_Waiting::proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous) {
    myWaitingStart = now;
    const SUMOTime until = MAX3(now, now + myWaitingDuration, myWaitingUntil);
    if (dynamic_cast<MSPerson*>(transportable) != 0) {
        previous->getEdge()->addPerson(transportable);
        net->getPersonControl().setWaitEnd(until, transportable);
    } else {
        previous->getEdge()->addContainer(transportable);
        net->getContainerControl().setWaitEnd(until, transportable);
    }
}


void
MSTransportable::Stage_Waiting::tripInfoOutput(OutputDevice& os) const {
    if (myType != WAITING_FOR_DEPART) {
        os.openTag("stop").writeAttr("arrival", time2string(myArrived)).closeTag();
    }
}


void
MSTransportable::Stage_Waiting::routeOutput(OutputDevice& os) const {
    if (myType != WAITING_FOR_DEPART) {
        os.openTag("stop").writeAttr(SUMO_ATTR_LANE, getDestination().getID());
        if (myWaitingDuration >= 0) {
            os.writeAttr(SUMO_ATTR_DURATION, time2string(myWaitingDuration));
        }
        if (myWaitingUntil >= 0) {
            os.writeAttr(SUMO_ATTR_UNTIL, time2string(myWaitingUntil));
        }
        os.closeTag();
    }
}


void
MSTransportable::Stage_Waiting::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actstart " + myActType)
    .writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSTransportable::Stage_Waiting::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "actend " + myActType).writeAttr("agent", p.getID())
    .writeAttr("link", getEdge()->getID()).closeTag();
}


SUMOTime
MSTransportable::Stage_Waiting::getWaitingTime(SUMOTime now) const {
    return now - myWaitingStart;
}


SUMOReal
MSTransportable::Stage_Waiting::getSpeed() const {
    return 0;
}


ConstMSEdgeVector
MSTransportable::Stage_Waiting::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(&getDestination());
    return result;
}



/* -------------------------------------------------------------------------
* MSTransportable::Stage_Driving - methods
* ----------------------------------------------------------------------- */
MSTransportable::Stage_Driving::Stage_Driving(const MSEdge& destination,
        MSStoppingPlace* toStop, const SUMOReal arrivalPos, const std::vector<std::string>& lines)
    : MSTransportable::Stage(destination, toStop, arrivalPos, DRIVING), myLines(lines.begin(), lines.end()),
      myVehicle(0), myStopWaitPos(Position::INVALID) {}


MSTransportable::Stage_Driving::~Stage_Driving() {}


const MSEdge*
MSTransportable::Stage_Driving::getEdge() const {
    if (myVehicle != 0) {
        return &myVehicle->getLane()->getEdge();
    }
    return myWaitingEdge;
}


const MSEdge*
MSTransportable::Stage_Driving::getFromEdge() const {
    return myWaitingEdge;
}


SUMOReal
MSTransportable::Stage_Driving::getEdgePos(SUMOTime /* now */) const {
    if (isWaiting4Vehicle()) {
        return myWaitingPos;
    }
    // vehicle may already have passed the lane (check whether this is correct)
    return MIN2(myVehicle->getPositionOnLane(), getEdge()->getLength());
}


Position
MSTransportable::Stage_Driving::getPosition(SUMOTime /* now */) const {
    if (isWaiting4Vehicle()) {
        if (myStopWaitPos != Position::INVALID) {
            return myStopWaitPos;
        }
        return getEdgePosition(myWaitingEdge, myWaitingPos, ROADSIDE_OFFSET);
    }
    return myVehicle->getPosition();
}


SUMOReal
MSTransportable::Stage_Driving::getAngle(SUMOTime /* now */) const {
    if (!isWaiting4Vehicle()) {
        MSVehicle* veh = dynamic_cast<MSVehicle*>(myVehicle);
        if (veh != 0) {
            return veh->getAngle();
        } else {
            return 0;
        }
    }
    return getEdgeAngle(myWaitingEdge, myWaitingPos) + M_PI / 2.;
}


bool
MSTransportable::Stage_Driving::isWaitingFor(const std::string& line) const {
    return myLines.count(line) > 0;
}


bool
MSTransportable::Stage_Driving::isWaiting4Vehicle() const {
    return myVehicle == 0;
}


SUMOTime
MSTransportable::Stage_Driving::getWaitingTime(SUMOTime now) const {
    return isWaiting4Vehicle() ? now - myWaitingSince : 0;
}


SUMOReal
MSTransportable::Stage_Driving::getSpeed() const {
    return isWaiting4Vehicle() ? 0 : myVehicle->getSpeed();
}


ConstMSEdgeVector
MSTransportable::Stage_Driving::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(getFromEdge());
    result.push_back(&getDestination());
    return result;
}

void
MSTransportable::Stage_Driving::abort(MSTransportable* t) {
    if (myVehicle != 0) {
        // jumping out of a moving vehicle!
        dynamic_cast<MSVehicle*>(myVehicle)->removeTransportable(t);
    }
}



void
MSTransportable::Stage_Driving::beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}


void
MSTransportable::Stage_Driving::endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival").writeAttr("agent", p.getID()).writeAttr("link", getEdge()->getID()).closeTag();
}



/* -------------------------------------------------------------------------
 * MSTransportable - methods
 * ----------------------------------------------------------------------- */
MSTransportable::MSTransportable(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportablePlan* plan)
    : myParameter(pars), myVType(vtype), myPlan(plan) {
    myStep = myPlan->begin();
}

MSTransportable::~MSTransportable() {
    if (myPlan != 0) {
        for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
            delete *i;
        }
        delete myPlan;
        myPlan = 0;
    }
    delete myParameter;
}

const std::string&
MSTransportable::getID() const {
    return myParameter->id;
}

SUMOTime
MSTransportable::getDesiredDepart() const {
    return myParameter->depart;
}

void
MSTransportable::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}

SUMOReal
MSTransportable::getEdgePos() const {
    return (*myStep)->getEdgePos(MSNet::getInstance()->getCurrentTimeStep());
}

Position
MSTransportable::getPosition() const {
    return (*myStep)->getPosition(MSNet::getInstance()->getCurrentTimeStep());
}

SUMOReal
MSTransportable::getAngle() const {
    return (*myStep)->getAngle(MSNet::getInstance()->getCurrentTimeStep());
}

SUMOReal
MSTransportable::getWaitingSeconds() const {
    return STEPS2TIME((*myStep)->getWaitingTime(MSNet::getInstance()->getCurrentTimeStep()));
}

SUMOReal
MSTransportable::getSpeed() const {
    return (*myStep)->getSpeed();
}


int
MSTransportable::getNumRemainingStages() const {
    return (int)(myPlan->end() - myStep);
}

int
MSTransportable::getNumStages() const {
    return (int)myPlan->size();
}

void
MSTransportable::appendStage(Stage* stage) {
    // myStep is invalidated upon modifying myPlan
    const int stepIndex = (int)(myStep - myPlan->begin());
    myPlan->push_back(stage);
    myStep = myPlan->begin() + stepIndex;
}


void
MSTransportable::removeStage(int next) {
    assert(myStep + next < myPlan->end());
    assert(next >= 0);
    if (next > 0) {
        // myStep is invalidated upon modifying myPlan
        int stepIndex = (int)(myStep - myPlan->begin());
        delete *(myStep + next);
        myPlan->erase(myStep + next);
        myStep = myPlan->begin() + stepIndex;
    } else {
        if (myStep + 1 == myPlan->end()) {
            // stay in the simulation until the start of simStep to allow appending new stages (at the correct position)
            appendStage(new Stage_Waiting(*getEdge(), 0, 0, getEdgePos(), "last stage removed", false));
        }
        (*myStep)->abort(this);
        proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep());
    }
}


void
MSTransportable::setSpeed(SUMOReal speed) {
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->setSpeed(speed);
    }
}


void
MSTransportable::replaceVehicleType(MSVehicleType* type) {
    if (myVType->amVehicleSpecific()) {
        delete myVType;
    }
    myVType = type;
}

/****************************************************************************/
