/****************************************************************************/
/// @file    MSBaseVehicle.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Mon, 8 Nov 2010
/// @version $Id$
///
// A base class for vehicle implementations
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

#include <iostream>
#include <cassert>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSMoveReminder.h"
#include "MSBaseVehicle.h"
#include "MSNet.h"
#include "devices/MSDevice.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
const SUMOTime MSBaseVehicle::NOT_YET_DEPARTED = SUMOTime_MAX;
#ifdef _DEBUG
std::set<std::string> MSBaseVehicle::myShallTraceMoveReminders;
#endif

// ===========================================================================
// method definitions
// ===========================================================================
MSBaseVehicle::MSBaseVehicle(SUMOVehicleParameter* pars, const MSRoute* route, const MSVehicleType* type, const SUMOReal speedFactor) :
    myParameter(pars),
    myRoute(route),
    myType(type),
    myCurrEdge(route->begin()),
    myChosenSpeedFactor(speedFactor),
    myMoveReminders(0),
    myDeparture(NOT_YET_DEPARTED),
    myArrivalPos(-1),
    myNumberReroutes(0)
#ifdef _DEBUG
    , myTraceMoveReminders(myShallTraceMoveReminders.count(pars->id) > 0)
#endif
{
    // init devices
    MSDevice::buildVehicleDevices(*this, myDevices);
    //
    for (std::vector< MSDevice* >::iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        myMoveReminders.push_back(std::make_pair(*dev, 0.));
    }
    myRoute->addReference();
    calculateArrivalPos();
}

MSBaseVehicle::~MSBaseVehicle() {
    myRoute->release();
    if (myParameter->repetitionNumber == 0) {
        MSRoute::checkDist(myParameter->routeid);
    }
    delete myParameter;
    for (std::vector< MSDevice* >::iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        delete *dev;
    }
}


const std::string&
MSBaseVehicle::getID() const {
    return myParameter->id;
}


const SUMOVehicleParameter&
MSBaseVehicle::getParameter() const {
    return *myParameter;
}


SUMOReal
MSBaseVehicle::getMaxSpeed() const {
    return myType->getMaxSpeed();
}


const MSEdge*
MSBaseVehicle::succEdge(unsigned int nSuccs) const {
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
MSBaseVehicle::reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, bool withTaz) {
    // check whether to reroute
    std::vector<const MSEdge*> edges;
    if (withTaz && MSEdge::dictionary(myParameter->fromTaz + "-source") && MSEdge::dictionary(myParameter->toTaz + "-sink")) {
        router.compute(MSEdge::dictionary(myParameter->fromTaz + "-source"), MSEdge::dictionary(myParameter->toTaz + "-sink"), this, t, edges);
        if (edges.size() >= 2) {
            edges.erase(edges.begin());
            edges.pop_back();
        }
    } else {
        router.compute(getRerouteOrigin(), myRoute->getLastEdge(), this, t, edges);
    }
    if (edges.empty()) {
        WRITE_WARNING("No route for vehicle '" + getID() + "' found.");
        return;
    }
    replaceRouteEdges(edges, withTaz);
}


bool
MSBaseVehicle::replaceRouteEdges(MSEdgeVector& edges, bool onInit) {
    // build a new id, first
    std::string id = getID();
    if (id[0] != '!') {
        id = "!" + id;
    }
    if (myRoute->getID().find("!var#") != std::string::npos) {
        id = myRoute->getID().substr(0, myRoute->getID().rfind("!var#") + 5) + toString(getNumberReroutes() + 1);
    } else {
        id = id + "!var#1";
    }
    const MSEdge* const origin = getRerouteOrigin();
    if (origin != *myCurrEdge && edges.front() == origin) {
        edges.insert(edges.begin(), *myCurrEdge);
    }
    const int oldSize = (int)edges.size();
    edges.insert(edges.begin(), myRoute->begin(), myCurrEdge);
    const RGBColor& c = myRoute->getColor();
    MSRoute* newRoute = new MSRoute(id, edges, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), myRoute->getStops());
    if (!MSRoute::dictionary(id, newRoute)) {
        delete newRoute;
        return false;
    }
    if (!replaceRoute(newRoute, onInit, (int)edges.size() - oldSize)) {
        newRoute->addReference();
        newRoute->release();
        return false;
    }
    return true;
}


SUMOReal
MSBaseVehicle::getAcceleration() const {
    return 0;
}


void
MSBaseVehicle::onDepart() {
    myDeparture = MSNet::getInstance()->getCurrentTimeStep();
    MSNet::getInstance()->getVehicleControl().vehicleDeparted(*this);
}


bool
MSBaseVehicle::hasDeparted() const {
    return myDeparture != NOT_YET_DEPARTED;
}


bool
MSBaseVehicle::hasArrived() const {
    return succEdge(1) == 0;
}

void
MSBaseVehicle::addPerson(MSPerson* /*person*/) {
}


bool
MSBaseVehicle::hasValidRoute(std::string& msg) const {
    MSRouteIterator last = myRoute->end() - 1;
    // check connectivity, first
    for (MSRouteIterator e = myCurrEdge; e != last; ++e) {
        if ((*e)->allowedLanes(**(e + 1), myType->getVehicleClass()) == 0) {
            msg = "No connection between '" + (*e)->getID() + "' and '" + (*(e + 1))->getID() + "'.";
            return false;
        }
    }
    last = myRoute->end();
    // check usable lanes, then
    for (MSRouteIterator e = myCurrEdge; e != last; ++e) {
        if ((*e)->prohibits(this)) {
            msg = "Edge '" + (*e)->getID() + "' prohibits.";
            return false;
        }
    }
    return true;
}


void
MSBaseVehicle::addReminder(MSMoveReminder* rem) {
#ifdef _DEBUG
    if (myTraceMoveReminders) {
        traceMoveReminder("add", rem, 0, true);
    }
#endif
    myMoveReminders.push_back(std::make_pair(rem, 0.));
}


void
MSBaseVehicle::removeReminder(MSMoveReminder* rem) {
    for (MoveReminderCont::iterator r = myMoveReminders.begin(); r != myMoveReminders.end(); ++r) {
        if (r->first == rem) {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("remove", rem, 0, false);
            }
#endif
            myMoveReminders.erase(r);
            return;
        }
    }
}


void
MSBaseVehicle::activateReminders(const MSMoveReminder::Notification reason) {
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end();) {
        if (rem->first->notifyEnter(*this, reason)) {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyEnter", rem->first, rem->second, true);
            }
#endif
            ++rem;
        } else {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyEnter", rem->first, rem->second, false);
            }
#endif
            rem = myMoveReminders.erase(rem);
        }
    }
}


void
MSBaseVehicle::calculateArrivalPos() {
    const SUMOReal lastLaneLength = (myRoute->getLastEdge()->getLanes())[0]->getLength();
    switch (myParameter->arrivalPosProcedure) {
        case ARRIVAL_POS_GIVEN:
            if (fabs(myParameter->arrivalPos) > lastLaneLength) {
                WRITE_WARNING("Vehicle '" + getID() + "' will not be able to arrive at the given position!");
            }
            // Maybe we should warn the user about invalid inputs!
            myArrivalPos = MIN2(myParameter->arrivalPos, lastLaneLength);
            if (myArrivalPos < 0) {
                myArrivalPos = MAX2(myArrivalPos + lastLaneLength, static_cast<SUMOReal>(0));
            }
            break;
        case ARRIVAL_POS_RANDOM:
            myArrivalPos = RandHelper::rand(static_cast<SUMOReal>(0), lastLaneLength);
            break;
        default:
            myArrivalPos = lastLaneLength;
            break;
    }
}


SUMOReal
MSBaseVehicle::getImpatience() const {
    return MAX2((SUMOReal)0, MIN2((SUMOReal)1, getVehicleType().getImpatience() +
                                  (MSGlobals::gTimeToGridlock > 0 ? (SUMOReal)getWaitingTime() / MSGlobals::gTimeToGridlock : 0)));
}


MSDevice*
MSBaseVehicle::getDevice(const std::type_info& type) const {
    for (std::vector<MSDevice*>::const_iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        if (typeid(**dev) == type) {
            return *dev;
        }
    }
    return 0;
}


void
MSBaseVehicle::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_VEHICLE).writeAttr(SUMO_ATTR_ID, myParameter->id);
    out.writeAttr(SUMO_ATTR_DEPART, myParameter->depart);
    out.writeAttr(SUMO_ATTR_ROUTE, myRoute->getID());
    out.writeAttr(SUMO_ATTR_TYPE, myType->getID());
    // here starts the vehicle internal part (see loading)
    // @note: remember to close the vehicle tag when calling this in a subclass!
}


#ifdef _DEBUG
void
MSBaseVehicle::initMoveReminderOutput(const OptionsCont& oc) {
    if (oc.isSet("movereminder-output.vehicles")) {
        const std::vector<std::string> vehicles = oc.getStringVector("movereminder-output.vehicles");
        myShallTraceMoveReminders.insert(vehicles.begin(), vehicles.end());
    }
}


void
MSBaseVehicle::traceMoveReminder(const std::string& type, MSMoveReminder* rem, SUMOReal pos, bool keep) const {
    OutputDevice& od = OutputDevice::getDeviceByOption("movereminder-output");
    od.openTag("movereminder");
    od.writeAttr(SUMO_ATTR_TIME, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()));
    od.writeAttr("veh", getID());
    od.writeAttr(SUMO_ATTR_ID, rem->getDescription());
    od.writeAttr("type", type);
    od.writeAttr("pos", toString(pos));
    od.writeAttr("keep", toString(keep));
    od.closeTag();
}
#endif

/****************************************************************************/

