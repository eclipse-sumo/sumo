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

#include <iostream>
#include <cassert>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSGlobals.h"
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

SUMOReal
MSBaseVehicle::getPreviousSpeed() const {
    throw ProcessError("getPreviousSpeed() is not available for non-MSVehicles.");
}

MSBaseVehicle::MSBaseVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                             const MSVehicleType* type, const SUMOReal speedFactor) :
    myParameter(pars),
    myRoute(route),
    myType(type),
    myCurrEdge(route->begin()),
    myChosenSpeedFactor(speedFactor),
    myMoveReminders(0),
    myDeparture(NOT_YET_DEPARTED),
    myDepartPos(-1),
    myArrivalPos(-1),
    myArrivalLane(-1),
    myNumberReroutes(0)
#ifdef _DEBUG
    , myTraceMoveReminders(myShallTraceMoveReminders.count(pars->id) > 0)
#endif
{
    if ((*myRoute->begin())->isTaz() || myRoute->getLastEdge()->isTaz()) {
        pars->setParameter |= VEHPARS_FORCE_REROUTE;
    }
    // init devices
    MSDevice::buildVehicleDevices(*this, myDevices);
    //
    for (std::vector< MSDevice* >::iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        myMoveReminders.push_back(std::make_pair(*dev, 0.));
    }
    myRoute->addReference();
    if (!pars->wasSet(VEHPARS_FORCE_REROUTE)) {
        calculateArrivalParams();
    }
}

MSBaseVehicle::~MSBaseVehicle() {
    myRoute->release();
    if (myParameter->repetitionNumber == 0) {
        MSRoute::checkDist(myParameter->routeid);
    }
    for (std::vector< MSDevice* >::iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        delete *dev;
    }
    delete myParameter;
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
MSBaseVehicle::succEdge(int nSuccs) const {
    if (myCurrEdge + nSuccs < myRoute->end() && std::distance(myCurrEdge, myRoute->begin()) <= nSuccs) {
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
MSBaseVehicle::reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, const bool onInit, const bool withTaz) {
    // check whether to reroute
    const MSEdge* source = withTaz && onInit ? MSEdge::dictionary(myParameter->fromTaz + "-source") : getRerouteOrigin();
    if (source == 0) {
        source = getRerouteOrigin();
    }
    const MSEdge* sink = withTaz ? MSEdge::dictionary(myParameter->toTaz + "-sink") : myRoute->getLastEdge();
    if (sink == 0) {
        sink = myRoute->getLastEdge();
    }
    ConstMSEdgeVector edges;
    ConstMSEdgeVector stops;
    if (myParameter->via.size() == 0) {
        stops = getStopEdges();
    } else {
        // via takes precedence over stop edges
        // XXX check for inconsistencies #2275
        for (std::vector<std::string>::const_iterator it = myParameter->via.begin(); it != myParameter->via.end(); ++it) {
            MSEdge* viaEdge = MSEdge::dictionary(*it);
            assert(viaEdge != 0);
            if (viaEdge->allowedLanes(getVClass()) == 0) {
                throw ProcessError("Vehicle '" + getID() + "' is not allowed on any lane of via edge '" + viaEdge->getID() + "'.");
            }
            stops.push_back(viaEdge);
        }
    }

    for (MSRouteIterator s = stops.begin(); s != stops.end(); ++s) {
        if (*s != source) {
            // !!! need to adapt t here
            router.compute(source, *s, this, t, edges);
            source = *s;
            edges.pop_back();
        }
    }
    router.compute(source, sink, this, t, edges);
    if (!edges.empty() && edges.front()->getPurpose() == MSEdge::EDGEFUNCTION_DISTRICT) {
        edges.erase(edges.begin());
    }
    if (!edges.empty() && edges.back()->getPurpose() == MSEdge::EDGEFUNCTION_DISTRICT) {
        edges.pop_back();
    }
    replaceRouteEdges(edges, onInit);
    // this must be called even if the route could not be replaced
    if (onInit) {
        calculateArrivalParams();
    }
}


bool
MSBaseVehicle::replaceRouteEdges(ConstMSEdgeVector& edges, bool onInit, bool check, bool addStops) {
    if (edges.empty()) {
        WRITE_WARNING("No route for vehicle '" + getID() + "' found.");
        return false;
    }
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
    int oldSize = (int)edges.size();
    if (!onInit) {
        const MSEdge* const origin = getRerouteOrigin();
        if (origin != *myCurrEdge && edges.front() == origin) {
            edges.insert(edges.begin(), *myCurrEdge);
            oldSize = (int)edges.size();
        }
        edges.insert(edges.begin(), myRoute->begin(), myCurrEdge);
    }
    if (edges == myRoute->getEdges()) {
        return true;
    }
    const RGBColor& c = myRoute->getColor();
    MSRoute* newRoute = new MSRoute(id, edges, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), myRoute->getStops());
    if (!MSRoute::dictionary(id, newRoute)) {
        delete newRoute;
        return false;
    }

    std::string msg;
    if (check && !hasValidRoute(msg, newRoute)) {
        WRITE_WARNING("Invalid route replacement for vehicle '" + getID() + "'. " + msg);
        if (MSGlobals::gCheckRoutes) {
            newRoute->addReference();
            newRoute->release();
            return false;
        }
    }
    if (!replaceRoute(newRoute, onInit, (int)edges.size() - oldSize, addStops)) {
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


SUMOReal
MSBaseVehicle::getSlope() const {
    return 0;
}


void
MSBaseVehicle::onDepart() {
    myDeparture = MSNet::getInstance()->getCurrentTimeStep();
    myDepartPos = getPositionOnLane();
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
MSBaseVehicle::addPerson(MSTransportable* /*person*/) {
}

void
MSBaseVehicle::addContainer(MSTransportable* /*container*/) {
}

bool
MSBaseVehicle::hasValidRoute(std::string& msg, const MSRoute* route) const {
    MSRouteIterator start = myCurrEdge;
    if (route == 0) {
        route = myRoute;
    } else {
        start = route->begin();
    }
    MSRouteIterator last = route->end() - 1;
    // check connectivity, first
    for (MSRouteIterator e = start; e != last; ++e) {
        if ((*e)->allowedLanes(**(e + 1), myType->getVehicleClass()) == 0) {
            msg = "No connection between edge '" + (*e)->getID() + "' and edge '" + (*(e + 1))->getID() + "'.";
            return false;
        }
    }
    last = route->end();
    // check usable lanes, then
    for (MSRouteIterator e = start; e != last; ++e) {
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
MSBaseVehicle::calculateArrivalParams() {
    if (myRoute->getLastEdge()->isTaz()) {
        return;
    }
    const std::vector<MSLane*>& lanes = myRoute->getLastEdge()->getLanes();
    const SUMOReal lastLaneLength = lanes[0]->getLength();
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
    if (myParameter->arrivalLaneProcedure == ARRIVAL_LANE_GIVEN) {
        if (myParameter->arrivalLane >= (int)lanes.size() || !lanes[myParameter->arrivalLane]->allowsVehicleClass(myType->getVehicleClass())) {
            WRITE_WARNING("Vehicle '" + getID() + "' will not be able to arrive at the given lane '" + myRoute->getLastEdge()->getID() + "_" + toString(myParameter->arrivalLane) + "'!");
        }
        myArrivalLane = MIN2(myParameter->arrivalLane, (int)(lanes.size() - 1));
    }
    if (myParameter->arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN) {
        for (std::vector<MSLane*>::const_iterator l = lanes.begin(); l != lanes.end(); ++l) {
            if (myParameter->arrivalSpeed <= (*l)->getVehicleMaxSpeed(this)) {
                return;
            }
        }
        WRITE_WARNING("Vehicle '" + getID() + "' will not be able to arrive with the given speed!");
    }
}


SUMOReal
MSBaseVehicle::getImpatience() const {
    return MAX2((SUMOReal)0, MIN2((SUMOReal)1, getVehicleType().getImpatience() +
                                  (MSGlobals::gTimeToGridlock > 0 ? (SUMOReal)getWaitingTime() / MSGlobals::gTimeToGridlock : 0)));
//    Alternavite to avoid time to teleport effect on the simulation. No effect if time to teleport is -1
//    return MAX2((SUMOReal)0, MIN2((SUMOReal)1, getVehicleType().getImpatience()));
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
    // this saves lots of departParameters which are only needed for vehicles that did not yet depart
    // the parameters may hold the name of a vTypeDistribution but we are interested in the actual type
    myParameter->write(out, OptionsCont::getOptions(), SUMO_TAG_VEHICLE, getVehicleType().getID());
    // params and stops must be written in child classes since they may wish to add additional attributes first
    out.writeAttr(SUMO_ATTR_ROUTE, myRoute->getID());
    // here starts the vehicle internal part (see loading)
    // @note: remember to close the vehicle tag when calling this in a subclass!
}


void
MSBaseVehicle::addStops(const bool ignoreStopErrors) {
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i = myParameter->stops.begin(); i != myParameter->stops.end(); ++i) {
        std::string errorMsg;
        if (!addStop(*i, errorMsg) && !ignoreStopErrors) {
            throw ProcessError(errorMsg);
        }
        if (errorMsg != "") {
            WRITE_WARNING(errorMsg);
        }
    }
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i = myRoute->getStops().begin(); i != myRoute->getStops().end(); ++i) {
        std::string errorMsg;
        if (!addStop(*i, errorMsg) && !ignoreStopErrors) {
            throw ProcessError(errorMsg);
        }
        if (errorMsg != "") {
            WRITE_WARNING(errorMsg);
        }
    }
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

