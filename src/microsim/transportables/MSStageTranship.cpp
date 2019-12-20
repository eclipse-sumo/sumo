/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSStageTranship.cpp
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
#include <microsim/transportables/MSPModel.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include "MSCModel_NonInteracting.h"
#include "MSStageTranship.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSStageTranship::MSStageTranship(const std::vector<const MSEdge*>& route,
                                 MSStoppingPlace* toStop,
                                 double speed,
                                 double departPos, double arrivalPos) :
    MSStageMoving(route, toStop, speed, departPos, arrivalPos, 0., MSStageType::TRANSHIP) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(
                  departPos, myRoute.front()->getLength(), SUMO_ATTR_DEPARTPOS,
                  "container getting transhipped from " + myRoute.front()->getID());
    myArrivalPos = SUMOVehicleParameter::interpretEdgePos(
                   arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS,
                   "container getting transhipped to " + route.back()->getID());
}


MSStageTranship::~MSStageTranship() {
}


MSStage*
MSStageTranship::clone() const {
    return new MSStageTranship(myRoute, myDestinationStop, mySpeed, myDepartPos, myArrivalPos);
}


void
MSStageTranship::proceed(MSNet* /* net */, MSTransportable* container, SUMOTime now, MSStage* previous) {
    myDeparted = now;
    //MSCModel_NonInteracting moves the container straight from start to end in
    //a single step and assumes that moveToNextEdge is only called once)
    //therefor we define that the container is already on its destination edge
    myRouteStep = myRoute.end() - 1;
    myDepartPos = previous->getEdgePos(now);
    myState = MSCModel_NonInteracting::getModel()->add(container, this, now);
    (*myRouteStep)->addContainer(container);
}

const MSEdge*
MSStageTranship::getEdge() const {
    if (myCurrentInternalEdge != nullptr) {
        return myCurrentInternalEdge;
    } else {
        return *myRouteStep;
    }
}

const MSEdge*
MSStageTranship::getFromEdge() const {
    return myRoute.front();
}

double
MSStageTranship::getEdgePos(SUMOTime now) const {
    return myState->getEdgePos(*this, now);
}

Position
MSStageTranship::getPosition(SUMOTime now) const {
    return myState->getPosition(*this, now);
}

double
MSStageTranship::getAngle(SUMOTime now) const {
    return myState->getAngle(*this, now);
}

SUMOTime
MSStageTranship::getWaitingTime(SUMOTime /* now */) const {
    return 0;
}

double
MSStageTranship::getSpeed() const {
    return myState->getSpeed(*this);
}


ConstMSEdgeVector
MSStageTranship::getEdges() const {
    return myRoute;
}

double
MSStageTranship::getDistance() const {
    if (myArrived >= 0) {
        const SUMOTime duration = myArrived - myDeparted;
        return mySpeed * STEPS2TIME(duration);
    } else {
        return -1;
    }
}

void
MSStageTranship::tripInfoOutput(OutputDevice& os, const MSTransportable* const) const {
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
MSStageTranship::routeOutput(const bool /*isPerson*/, OutputDevice& os, const bool withRouteLength) const {
    os.openTag("tranship").writeAttr(SUMO_ATTR_EDGES, myRoute);
    os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    if (withRouteLength) {
        os.writeAttr("routeLength", mySpeed * (myArrived - myDeparted));
    }
    os.closeTag();
}


bool
MSStageTranship::moveToNextEdge(MSTransportable* container, SUMOTime currentTime, MSEdge* nextInternal) {
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
MSStageTranship::getStageSummary(const bool /*isPerson*/) const {
    const std::string dest = (getDestinationStop() == nullptr ?
                              " edge '" + getDestination()->getID() + "'" :
                              " stop '" + getDestinationStop()->getID() + "'");
    return "transhipped to " + dest;
}


/****************************************************************************/
