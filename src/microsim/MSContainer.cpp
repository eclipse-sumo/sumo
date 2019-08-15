/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSContainer.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The class for modelling container-movements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include "MSNet.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSContainer.h"
#include "microsim/MSStoppingPlace.h"
#include "MSTransportableControl.h"
#include "MSInsertionControl.h"
#include "MSVehicle.h"
#include "MSVehicleControl.h"
#include "MSCModel_NonInteracting.h"

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Driving - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Driving::MSContainerStage_Driving(const MSEdge* destination,
        MSStoppingPlace* toStop, const double arrivalPos, const std::vector<std::string>& lines) :
    MSTransportable::Stage_Driving(destination, toStop,
                                   SUMOVehicleParameter::interpretEdgePos(
                                       arrivalPos, destination->getLength(), SUMO_ATTR_ARRIVALPOS, "container getting transported to " + destination->getID()),
                                   lines) {
}


MSContainer::MSContainerStage_Driving::~MSContainerStage_Driving() {}

MSTransportable::Stage*
MSContainer::MSContainerStage_Driving::clone() const {
    return new MSContainerStage_Driving(myDestination, myDestinationStop, myArrivalPos, std::vector<std::string>(myLines.begin(), myLines.end()));
}

void
MSContainer::MSContainerStage_Driving::proceed(MSNet* net, MSTransportable* container, SUMOTime now, Stage* previous) {
    if (previous->getDestinationStop() != nullptr) {
        // the arrival stop may have an access point
        myWaitingEdge = &previous->getDestinationStop()->getLane().getEdge();
    } else {
        myWaitingEdge = previous->getEdge();
    }
    myWaitingPos = previous->getEdgePos(now);
    myWaitingSince = now;
    SUMOVehicle* availableVehicle = net->getVehicleControl().getWaitingVehicle(container, myWaitingEdge, myWaitingPos);
    if (availableVehicle != nullptr && availableVehicle->getParameter().departProcedure == DEPART_CONTAINER_TRIGGERED && !availableVehicle->hasDeparted()) {
        setVehicle(availableVehicle);
        myVehicle->addContainer(container);
        net->getInsertionControl().add(myVehicle);
        net->getVehicleControl().removeWaiting(myWaitingEdge, myVehicle);
        net->getVehicleControl().unregisterOneWaiting(false);
    } else {
        net->getContainerControl().addWaiting(myWaitingEdge, container);
        myWaitingEdge->addContainer(container);
    }
}


std::string
MSContainer::MSContainerStage_Driving::getStageDescription() const {
    return isWaiting4Vehicle() ? "waiting for " + joinToString(myLines, ",") : "transport";
}


std::string
MSContainer::MSContainerStage_Driving::getStageSummary() const {
    const std::string dest = (getDestinationStop() == nullptr ?
                              " edge '" + getDestination()->getID() + "'" :
                              " stop '" + getDestinationStop()->getID() + "'");
    return isWaiting4Vehicle() ?
           "waiting for " + joinToString(myLines, ",") + " then transported to " + dest :
           "transported to " + dest;
}



void
MSContainer::MSContainerStage_Driving::tripInfoOutput(OutputDevice& os, const MSTransportable* const) const {
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    const SUMOTime departed = myDeparted >= 0 ? myDeparted : now;
    os.openTag("transport");
    os.writeAttr("waitingTime", time2string(departed - myWaitingSince));
    os.writeAttr("vehicle", myVehicleID);
    os.writeAttr("depart", myDeparted >= 0 ? time2string(myDeparted) : "-1");
    os.writeAttr("arrival", myArrived >= 0 ? time2string(myArrived) : "-1");
    os.writeAttr("arrivalPos", toString(myArrivalPos));
    os.writeAttr("duration", myArrived >= 0 ? time2string(myArrived - myDeparted) :
                 (myDeparted >= 0 ? time2string(now - myDeparted) : "-1"));
    os.writeAttr("routeLength", myVehicleDistance);
    os.closeTag();
}


void
MSContainer::MSContainerStage_Driving::routeOutput(OutputDevice& os, const bool withRouteLength) const {
    os.openTag("transport").writeAttr(SUMO_ATTR_FROM, getFromEdge()->getID()).writeAttr(SUMO_ATTR_TO, getDestination()->getID());
    os.writeAttr(SUMO_ATTR_LINES, myLines).closeTag();
    if (withRouteLength) {
        os.writeAttr("routeLength", myVehicleDistance);
    }
}



/* -------------------------------------------------------------------------
 * MSContainer::MSContainerStage_Tranship - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainerStage_Tranship::MSContainerStage_Tranship(const std::vector<const MSEdge*>& route,
        MSStoppingPlace* toStop,
        double speed,
        double departPos, double arrivalPos) :
    MSTransportable::Stage(route.back(), toStop, SUMOVehicleParameter::interpretEdgePos(
                               arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS,
                               "container getting transhipped to " + route.back()->getID()),
                           MOVING_WITHOUT_VEHICLE), myRoute(route),
    mySpeed(speed), myContainerState(nullptr), myCurrentInternalEdge(nullptr) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                      departPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS,
                      "container getting transhipped from " + myRoute.front()->getID());
}

MSContainer::MSContainerStage_Tranship::~MSContainerStage_Tranship() {
}

MSTransportable::Stage*
MSContainer::MSContainerStage_Tranship::clone() const {
    return new MSContainerStage_Tranship(myRoute, myDestinationStop, mySpeed, myDepartPos, myArrivalPos);
}

void
MSContainer::MSContainerStage_Tranship::proceed(MSNet* /* net */, MSTransportable* container, SUMOTime now, Stage* previous) {
    myDeparted = now;
    //MSCModel_NonInteracting moves the container straight from start to end in
    //a single step and assumes that moveToNextEdge is only called once)
    //therefor we define that the container is already on its destination edge
    myRouteStep = myRoute.end() - 1;
    myDepartPos = previous->getEdgePos(now);
    myContainerState = MSCModel_NonInteracting::getModel()->add(container, this, now);
    (*myRouteStep)->addContainer(container);
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getEdge() const {
    if (myCurrentInternalEdge != nullptr) {
        return myCurrentInternalEdge;
    } else {
        return *myRouteStep;
    }
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getFromEdge() const {
    return myRoute.front();
}

const MSEdge*
MSContainer::MSContainerStage_Tranship::getToEdge() const {
    return myRoute.back();
}

double
MSContainer::MSContainerStage_Tranship::getEdgePos(SUMOTime now) const {
    return myContainerState->getEdgePos(*this, now);
}

Position
MSContainer::MSContainerStage_Tranship::getPosition(SUMOTime now) const {
    return myContainerState->getPosition(*this, now);
}

double
MSContainer::MSContainerStage_Tranship::getAngle(SUMOTime now) const {
    return myContainerState->getAngle(*this, now);
}

SUMOTime
MSContainer::MSContainerStage_Tranship::getWaitingTime(SUMOTime /* now */) const {
    return 0;
}

double
MSContainer::MSContainerStage_Tranship::getSpeed() const {
    return myContainerState->getSpeed(*this);
}


ConstMSEdgeVector
MSContainer::MSContainerStage_Tranship::getEdges() const {
    return myRoute;
}

double
MSContainer::MSContainerStage_Tranship::getDistance() const {
    if (myArrived >= 0) {
        const SUMOTime duration = myArrived - myDeparted;
        return mySpeed * STEPS2TIME(duration);
    } else {
        return -1;
    }
}

void
MSContainer::MSContainerStage_Tranship::tripInfoOutput(OutputDevice& os, const MSTransportable* const) const {
    os.openTag("tranship");
    os.writeAttr("depart", time2string(myDeparted));
    os.writeAttr("departPos", myDepartPos);
    os.writeAttr("arrival", time2string(myArrived));
    os.writeAttr("arrivalPos", myArrivalPos);
    os.writeAttr("duration", myArrived >= 0 ? time2string(myArrived - myDeparted) : "-1");
    os.writeAttr("routeLength", getDistance());
    os.writeAttr("maxSpeed", mySpeed);
    os.closeTag();
}


void
MSContainer::MSContainerStage_Tranship::routeOutput(OutputDevice& os, const bool withRouteLength) const {
    os.openTag("tranship").writeAttr(SUMO_ATTR_EDGES, myRoute);
    os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    if (withRouteLength) {
        os.writeAttr("routeLength", mySpeed * (myArrived - myDeparted));
    }
    os.closeTag();
}


void
MSContainer::MSContainerStage_Tranship::beginEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "departure")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.front()->getID()).closeTag();
}


void
MSContainer::MSContainerStage_Tranship::endEventOutput(const MSTransportable& c, SUMOTime t, OutputDevice& os) const {
    os.openTag("event").writeAttr("time", time2string(t)).writeAttr("type", "arrival")
    .writeAttr("agent", c.getID()).writeAttr("link", myRoute.back()->getID()).closeTag();
}

bool
MSContainer::MSContainerStage_Tranship::moveToNextEdge(MSTransportable* container, SUMOTime currentTime, MSEdge* nextInternal) {
    ((MSEdge*)getEdge())->removeContainer(container);
    if (myRouteStep == myRoute.end() - 1) {
        if (myDestinationStop != nullptr) {
            myDestinationStop->addTransportable(container);    //jakob
        }
        if (!container->proceed(MSNet::getInstance(), currentTime)) {
            MSNet::getInstance()->getContainerControl().erase(container);
        }
        return true;
    } else {
        if (nextInternal == nullptr) {
            ++myRouteStep;
            myCurrentInternalEdge = nullptr;
        } else {
            myCurrentInternalEdge = nextInternal;
        }
        ((MSEdge*) getEdge())->addContainer(container);
        return false;
    }
}

std::string
MSContainer::MSContainerStage_Tranship::getStageSummary() const {
    const std::string dest = (getDestinationStop() == nullptr ?
                              " edge '" + getDestination()->getID() + "'" :
                              " stop '" + getDestinationStop()->getID() + "'");
    return "transhipped to " + dest;
}

/* -------------------------------------------------------------------------
 * MSContainer - methods
 * ----------------------------------------------------------------------- */
MSContainer::MSContainer(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportablePlan* plan)
    : MSTransportable(pars, vtype, plan) {
}


MSContainer::~MSContainer() {
}


bool
MSContainer::proceed(MSNet* net, SUMOTime time) {
    Stage* prior = *myStep;
    prior->setArrived(net, this, time);
    // must be done before increasing myStep to avoid invalid state for rendering
    prior->getEdge()->removeContainer(this);
    myStep++;
    if (myStep != myPlan->end()) {
        (*myStep)->proceed(net, this, time, prior);
        return true;
    } else {
        // cleanup
        if (prior->getDestinationStop() != nullptr) {
            prior->getDestinationStop()->removeTransportable(this);
        }
        return false;
    }
}


void
MSContainer::tripInfoOutput(OutputDevice& os) const {
    os.openTag("containerinfo").writeAttr("id", getID()).writeAttr("depart", time2string(getDesiredDepart()));
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->tripInfoOutput(os, this);
    }
    os.closeTag();
}


void
MSContainer::routeOutput(OutputDevice& os, const bool withRouteLength) const {
    os.openTag(SUMO_TAG_CONTAINER).writeAttr(SUMO_ATTR_ID, getID()).writeAttr(SUMO_ATTR_DEPART, time2string(getDesiredDepart()));
    if (myStep == myPlan->end()) {
        os.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
        (*i)->routeOutput(os, withRouteLength);
    }
    os.closeTag();
    os.lf();
}


/****************************************************************************/
