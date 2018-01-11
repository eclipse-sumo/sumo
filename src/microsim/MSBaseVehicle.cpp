/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include "MSTransportable.h"
#include "MSVehicleControl.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSMoveReminder.h"
#include "MSBaseVehicle.h"
#include "MSNet.h"
#include "devices/MSDevice.h"
#include "devices/MSDevice_Routing.h"
#include "MSInsertionControl.h"

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

double
MSBaseVehicle::getPreviousSpeed() const {
    throw ProcessError("getPreviousSpeed() is not available for non-MSVehicles.");
}


MSBaseVehicle::MSBaseVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                             MSVehicleType* type, const double speedFactor) :
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
    if ((*myRoute->begin())->isTazConnector() || myRoute->getLastEdge()->isTazConnector()) {
        pars->parametersSet |= VEHPARS_FORCE_REROUTE;
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
        if (MSGlobals::gCheckRoutes) {
            std::string msg;
            if (!hasValidRoute(msg)) {
                throw ProcessError("Vehicle '" + pars->id + "' has no valid route. " + msg);
            }
        }
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


double
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
            ConstMSEdgeVector into;
            router.compute(source, *s, this, t, into);
            if (into.size() > 0) {
                into.pop_back();
                edges.insert(edges.end(), into.begin(), into.end());
            } else {
                std::string error = "Vehicle '" + getID() + "' has no valid route from edge '" + source->getID() + "' to stop edge '" + (*s)->getID() + "'.";
                if (MSGlobals::gCheckRoutes) {
                    throw ProcessError(error);
                } else {
                    WRITE_WARNING(error);
                    edges.push_back(source);
                }
            }
            source = *s;
        }
    }
    router.compute(source, sink, this, t, edges);
    if (!edges.empty() && edges.front()->isTazConnector()) {
        edges.erase(edges.begin());
    }
    if (!edges.empty() && edges.back()->isTazConnector()) {
        edges.pop_back();
    }
    replaceRouteEdges(edges, onInit);
    // this must be called even if the route could not be replaced
    if (onInit) {
        if (edges.empty()) {
            if (MSGlobals::gCheckRoutes) {
                throw ProcessError("Vehicle '" + getID() + "' has no valid route.");
            } else if (source->isTazConnector()) {
                WRITE_WARNING("Removing vehicle '" + getID() + "' which has no valid route.");
                MSNet::getInstance()->getInsertionControl().descheduleDeparture(this);
                return;
            }
        }
        calculateArrivalParams();
    }
}


bool
MSBaseVehicle::replaceRouteEdges(ConstMSEdgeVector& edges, bool onInit, bool check, bool removeStops) {
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
    MSRoute* newRoute = new MSRoute(id, edges, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), std::vector<SUMOVehicleParameter::Stop>());
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
    if (!replaceRoute(newRoute, onInit, (int)edges.size() - oldSize, false, removeStops)) {
        newRoute->addReference();
        newRoute->release();
        return false;
    }
    return true;
}


double
MSBaseVehicle::getAcceleration() const {
    return 0;
}


double
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
MSBaseVehicle::addPerson(MSTransportable* person) {
    throw ProcessError("Person '" + person->getID() + "' cannot ride in vehicle '" + getID() + "' in the mesoscopic simulation.");
}

void
MSBaseVehicle::addContainer(MSTransportable* container) {
    throw ProcessError("Container '" + container->getID() + "' cannot ride in vehicle '" + getID() + "' in the mesoscopic simulation.");
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
MSBaseVehicle::activateReminders(const MSMoveReminder::Notification reason, const MSLane* enteredLane) {
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end();) {
        if (rem->first->notifyEnter(*this, reason, enteredLane)) {
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
    if (myRoute->getLastEdge()->isTazConnector()) {
        return;
    }
    const std::vector<MSLane*>& lanes = myRoute->getLastEdge()->getLanes();
    const double lastLaneLength = lanes[0]->getLength();
    switch (myParameter->arrivalPosProcedure) {
        case ARRIVAL_POS_GIVEN:
            if (fabs(myParameter->arrivalPos) > lastLaneLength) {
                WRITE_WARNING("Vehicle '" + getID() + "' will not be able to arrive at the given position!");
            }
            // Maybe we should warn the user about invalid inputs!
            myArrivalPos = MIN2(myParameter->arrivalPos, lastLaneLength);
            if (myArrivalPos < 0) {
                myArrivalPos = MAX2(myArrivalPos + lastLaneLength, 0.);
            }
            break;
        case ARRIVAL_POS_RANDOM:
            myArrivalPos = RandHelper::rand(0., lastLaneLength);
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


double
MSBaseVehicle::getImpatience() const {
    return MAX2(0., MIN2(1., getVehicleType().getImpatience() +
                         (MSGlobals::gTimeToImpatience > 0 ? (double)getWaitingTime() / MSGlobals::gTimeToImpatience : 0)));
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
    if (myParameter->wasSet(VEHPARS_FORCE_REROUTE) && !hasDeparted()) {
        out.writeAttr(SUMO_ATTR_REROUTE, true);
    }
    // here starts the vehicle internal part (see loading)
    // @note: remember to close the vehicle tag when calling this in a subclass!
}


void
MSBaseVehicle::addStops(const bool ignoreStopErrors) {
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i = myRoute->getStops().begin(); i != myRoute->getStops().end(); ++i) {
        std::string errorMsg;
        if (!addStop(*i, errorMsg, myParameter->depart) && !ignoreStopErrors) {
            throw ProcessError(errorMsg);
        }
        if (errorMsg != "") {
            WRITE_WARNING(errorMsg);
        }
    }
    const SUMOTime untilOffset = myParameter->repetitionOffset > 0 ? myParameter->repetitionsDone * myParameter->repetitionOffset : 0;
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i = myParameter->stops.begin(); i != myParameter->stops.end(); ++i) {
        std::string errorMsg;
        if (!addStop(*i, errorMsg, untilOffset) && !ignoreStopErrors) {
            throw ProcessError(errorMsg);
        }
        if (errorMsg != "") {
            WRITE_WARNING(errorMsg);
        }
    }
}


bool
MSBaseVehicle::hasDevice(const std::string& deviceName) const {
    for (std::vector<MSDevice* >::const_iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        if ((*dev)->deviceName() == deviceName) {
            return true;
        }
    }
    return false;
}


void
MSBaseVehicle::createDevice(const std::string& deviceName) {
    if (!hasDevice(deviceName)) {
        if (deviceName == "rerouting") {
            ((SUMOVehicleParameter*)myParameter)->setParameter("has." + deviceName + ".device", "true");
            MSDevice_Routing::buildVehicleDevices(*this, myDevices);
            if (hasDeparted()) {
                // vehicle already departed: disable pre-insertion rerouting and enable regular routing behavior
                MSDevice_Routing* routingDevice = static_cast<MSDevice_Routing*>(getDevice(typeid(MSDevice_Routing)));
                assert(routingDevice != 0);
                routingDevice->notifyEnter(*this, MSMoveReminder::NOTIFICATION_DEPARTED);
            }
        } else {
            throw InvalidArgument("Creating device of type '" + deviceName + "' is not supported");
        }
    }
}


std::string
MSBaseVehicle::getDeviceParameter(const std::string& deviceName, const std::string& key) const {
    for (std::vector<MSDevice* >::const_iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        if ((*dev)->deviceName() == deviceName) {
            return (*dev)->getParameter(key);
        }
    }
    throw InvalidArgument("No device of type '" + deviceName + "' exists");
}


void
MSBaseVehicle::setDeviceParameter(const std::string& deviceName, const std::string& key, const std::string& value) {
    for (std::vector<MSDevice* >::iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        if ((*dev)->deviceName() == deviceName) {
            (*dev)->setParameter(key, value);
            return;
        }
    }
    throw InvalidArgument("No device of type '" + deviceName + "' exists");
}


void
MSBaseVehicle::replaceVehicleType(MSVehicleType* type) {
    if (myType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myType);
    }
    myType = type;
}


MSVehicleType&
MSBaseVehicle::getSingularType() {
    if (myType->isVehicleSpecific()) {
        return *myType;
    }
    MSVehicleType* type = myType->buildSingularType(myType->getID() + "@" + getID());
    replaceVehicleType(type);
    return *type;
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
MSBaseVehicle::traceMoveReminder(const std::string& type, MSMoveReminder* rem, double pos, bool keep) const {
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

