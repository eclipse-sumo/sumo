/****************************************************************************/
/// @file    MSBaseVehicle.cpp
/// @author  Michael Behrisch
/// @date    Mon, 8 Nov 2010
/// @version $Id$
///
// A base class for vehicle implementations
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

#include <iostream>
#include <cassert>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSMoveReminder.h"
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_Person.h>
#include <microsim/devices/MSDevice_HBEFA.h>
#include "MSBaseVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSBaseVehicle::MSBaseVehicle(SUMOVehicleParameter* pars, const MSRoute* route, const MSVehicleType* type) throw(ProcessError) :
        myParameter(pars),
        myRoute(route),
        myCurrEdge(route->begin()),
        myMoveReminders(0),
        myIndividualMaxSpeed(0.0),
        myHasIndividualMaxSpeed(false),
        myReferenceSpeed(-1.0),
        myType(type),
        myDeparture(-1),
        myArrivalPos(-1),
        myNumberReroutes(0) {
    // init devices
    MSDevice_Vehroutes::buildVehicleDevices(*this, myDevices);
    MSDevice_Tripinfo::buildVehicleDevices(*this, myDevices);
    MSDevice_Routing::buildVehicleDevices(*this, myDevices);
    MSDevice_HBEFA::buildVehicleDevices(*this, myDevices);
    for (std::vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        myMoveReminders.push_back(std::make_pair(*dev, 0.));
    }
    myRoute->addReference();
    calculateArrivalPos();
}

MSBaseVehicle::~MSBaseVehicle() throw() {
    myRoute->release();
    delete myParameter;
    for (std::vector< MSDevice* >::iterator dev=myDevices.begin(); dev != myDevices.end(); ++dev) {
        delete(*dev);
    }
}


const std::string &
MSBaseVehicle::getID() const throw() {
    return myParameter->id;
}


const SUMOVehicleParameter &
MSBaseVehicle::getParameter() const throw() {
    return *myParameter;
}


const MSRoute &
MSBaseVehicle::getRoute() const throw() {
    return *myRoute;
}


const MSVehicleType &
MSBaseVehicle::getVehicleType() const throw() {
    return *myType;
}


SUMOReal
MSBaseVehicle::getMaxSpeed() const {
    if (myHasIndividualMaxSpeed) {
        return myIndividualMaxSpeed;
    }
    return myType->getMaxSpeed();
}


SUMOReal
MSBaseVehicle::adaptMaxSpeed(SUMOReal referenceSpeed) {
    if (myType->hasSpeedDeviation() && referenceSpeed != myReferenceSpeed) {
        myHasIndividualMaxSpeed = true;
        myIndividualMaxSpeed = myType->getMaxSpeedWithDeviation(referenceSpeed);
        myReferenceSpeed = referenceSpeed;
    }
    if (myHasIndividualMaxSpeed)
        return myIndividualMaxSpeed;
    return MIN2(myType->getMaxSpeed(), referenceSpeed);
}


const MSEdge*
MSBaseVehicle::succEdge(unsigned int nSuccs) const throw() {
    if (myCurrEdge + nSuccs < myRoute->end()) {
        return *(myCurrEdge + nSuccs);
    } else {
        return 0;
    }
}


const MSEdge*
MSBaseVehicle::getEdge() const {
    return *myCurrEdge;
}


void
MSBaseVehicle::reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle> &router, bool withTaz) throw() {
    // check whether to reroute
    std::vector<const MSEdge*> edges;
    if (withTaz && MSEdge::dictionary(myParameter->fromTaz+"-source") && MSEdge::dictionary(myParameter->toTaz+"-sink")) {
        router.compute(MSEdge::dictionary(myParameter->fromTaz+"-source"), MSEdge::dictionary(myParameter->toTaz+"-sink"), this, t, edges);
        if (edges.size() >= 2) {
            edges.erase(edges.begin());
            edges.pop_back();
        }
    } else {
        router.compute(*myCurrEdge, myRoute->getLastEdge(), this, t, edges);
    }
    if (edges.empty()) {
        WRITE_WARNING("No route for vehicle '" + getID() + "' found.");
        return;
    }
    replaceRouteEdges(edges, withTaz);
}


bool
MSBaseVehicle::replaceRouteEdges(const MSEdgeVector &edges, bool onInit) throw() {
    // build a new id, first
    std::string id = getID();
    if (id[0]!='!') {
        id = "!" + id;
    }
    if (myRoute->getID().find("!var#")!=std::string::npos) {
        id = myRoute->getID().substr(0, myRoute->getID().rfind("!var#")+4) + toString(getNumberReroutes() + 1);
    } else {
        id = id + "!var#1";
    }
    MSRoute *newRoute = new MSRoute(id, edges, 0, myRoute->getColor(), myRoute->getStops());
    if (!MSRoute::dictionary(id, newRoute)) {
        delete newRoute;
        return false;
    }
    if (!replaceRoute(newRoute, onInit)) {
        newRoute->addReference();
        newRoute->release();
        return false;
    }
    return true;
}


SUMOReal
MSBaseVehicle::getPreDawdleAcceleration() const throw() {
    return 0;
}


void
MSBaseVehicle::onDepart() throw() {
    myDeparture = MSNet::getInstance()->getCurrentTimeStep();
    MSNet::getInstance()->getVehicleControl().vehicleEmitted(*this);
}


SUMOTime
MSBaseVehicle::getDeparture() const throw() {
    return myDeparture;
}


unsigned int
MSBaseVehicle::getNumberReroutes() const throw() {
    return myNumberReroutes;
}


void
MSBaseVehicle::addPerson(MSPerson* /*person*/) throw() {
}

bool
MSBaseVehicle::isStopped() const {
    return false;
}


bool
MSBaseVehicle::hasValidRoute(std::string &msg) const throw() {
    MSRouteIterator last = myRoute->end() - 1;
    // check connectivity, first
    for (MSRouteIterator e=myCurrEdge; e!=last; ++e) {
        if ((*e)->allowedLanes(**(e+1), myType->getVehicleClass())==0) {
            msg = "No connection between '" + (*e)->getID() + "' and '" + (*(e+1))->getID() + "'.";
            return false;
        }
    }
    last = myRoute->end();
    // check usable lanes, then
    for (MSRouteIterator e=myCurrEdge; e!=last; ++e) {
        if ((*e)->prohibits(this)) {
            msg = "Edge '" + (*e)->getID() + "' prohibits.";
            return false;
        }
    }
    return true;
}


void
MSBaseVehicle::addReminder(MSMoveReminder* rem) throw() {
    myMoveReminders.push_back(std::make_pair(rem, 0.));
}


void
MSBaseVehicle::calculateArrivalPos() throw() {
    const SUMOReal lastLaneLength = (myRoute->getLastEdge()->getLanes())[0]->getLength();
    if (myArrivalPos < 0 || myArrivalPos > lastLaneLength || myParameter->arrivalPosProcedure == ARRIVAL_POS_MAX) {
        switch (myParameter->arrivalPosProcedure) {
        case ARRIVAL_POS_DEFAULT:
        case ARRIVAL_POS_GIVEN:
            // Maybe we should warn the user about invalid inputs!
            myArrivalPos = MIN2(myParameter->arrivalPos, lastLaneLength);
            if (myArrivalPos < 0) {
                myArrivalPos = MAX2(myArrivalPos + lastLaneLength, static_cast<SUMOReal>(0));
            }
            break;
        case ARRIVAL_POS_RANDOM:
            myArrivalPos = RandHelper::rand(static_cast<SUMOReal>(0), lastLaneLength);
            break;
        case ARRIVAL_POS_MAX:
            myArrivalPos = lastLaneLength;
            break;
        }
    }
}


/****************************************************************************/

