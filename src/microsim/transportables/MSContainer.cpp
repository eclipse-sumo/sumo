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
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSCModel_NonInteracting.h>
#include "MSContainer.h"


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
    SUMOVehicle* availableVehicle = myWaitingEdge->getWaitingVehicle(container, myWaitingPos);
    if (availableVehicle != nullptr && availableVehicle->getParameter().departProcedure == DEPART_CONTAINER_TRIGGERED && !availableVehicle->hasDeparted()) {
        setVehicle(availableVehicle);
        myVehicle->addContainer(container);
        net->getInsertionControl().add(myVehicle);
        myWaitingEdge->removeWaiting(myVehicle);
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
                           StageType::TRANSHIP), myRoute(route),
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
    : MSTransportable(pars, vtype, plan, false) {
}


MSContainer::~MSContainer() {
}


/****************************************************************************/
