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
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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

#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSEdge.h"
#include "MSLane.h"
#include "MSTransportable.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTransportable::Stage - methods
 * ----------------------------------------------------------------------- */
MSTransportable::Stage::Stage(const MSEdge& destination, StageType type)
    : myDestination(destination), myDeparted(-1), myArrived(-1), myType(type) {}

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
    PositionVector shp = e->getLanes()[0]->getShape();
    return -shp.rotationDegreeAtOffset(at);
}


/* -------------------------------------------------------------------------
 * MSTransportable - methods
 * ----------------------------------------------------------------------- */
MSTransportable::MSTransportable(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportablePlan* plan)
        : myParameter(pars), myVType(vtype), myPlan(plan) {
    myStep = myPlan->begin();
}

MSTransportable::~MSTransportable() {
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        delete *i;
    }
    delete myPlan;
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


/****************************************************************************/
