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
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include "MSContainerControl.h"
#include "MSPersonControl.h"
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


void
MSTransportable::tripInfoOutput(OutputDevice& os) const {
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os);
    }
}


void
MSTransportable::routeOutput(OutputDevice& os) const {
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->routeOutput(os);
    }
}


/****************************************************************************/
