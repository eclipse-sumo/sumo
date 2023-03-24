/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSStage.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#include <config.h>

#include <utils/vehicle/SUMOVehicleParameter.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/devices/MSTransportableDevice.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPModel.h>


/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
const double MSStage::ROADSIDE_OFFSET(3);


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSStage - methods
 * ----------------------------------------------------------------------- */
MSStage::MSStage(const MSEdge* destination, MSStoppingPlace* toStop, const double arrivalPos, MSStageType type, const std::string& group) :
    myDestination(destination),
    myDestinationStop(toStop),
    myArrivalPos(arrivalPos),
    myDeparted(-1),
    myArrived(-1),
    myType(type),
    myGroup(group),
    myParametersSet(0)
{}

MSStage::~MSStage() {}

const MSEdge*
MSStage::getDestination() const {
    return myDestination;
}


const MSEdge*
MSStage::getEdge() const {
    return myDestination;
}


const MSEdge*
MSStage::getFromEdge() const {
    return myDestination;
}


double
MSStage::getEdgePos(SUMOTime /* now */) const {
    return myArrivalPos;
}

int
MSStage::getDirection() const {
    return MSPModel::UNDEFINED_DIRECTION;
}


SUMOTime
MSStage::getWaitingTime(SUMOTime /* now */) const {
    return 0;
}


double
MSStage::getSpeed() const {
    return 0.;
}


ConstMSEdgeVector
MSStage::getEdges() const {
    ConstMSEdgeVector result;
    result.push_back(getDestination());
    return result;
}


void
MSStage::setDeparted(SUMOTime now) {
    if (myDeparted < 0) {
        myDeparted = now;
    }
}

SUMOTime
MSStage::getDeparted() const {
    return myDeparted;
}

SUMOTime
MSStage::getArrived() const {
    return myArrived;
}

const std::string
MSStage::setArrived(MSNet* /* net */, MSTransportable* /* transportable */, SUMOTime now, const bool /* vehicleArrived */) {
    myArrived = now;
    return "";
}

bool
MSStage::isWaitingFor(const SUMOVehicle* /*vehicle*/) const {
    return false;
}

Position
MSStage::getEdgePosition(const MSEdge* e, double at, double offset) const {
    return getLanePosition(e->getLanes()[0], at, offset);
}

Position
MSStage::getLanePosition(const MSLane* lane, double at, double offset) const {
    return lane->getShape().positionAtOffset(lane->interpolateLanePosToGeometryPos(at), offset);
}


double
MSStage::getEdgeAngle(const MSEdge* e, double at) const {
    return e->getLanes()[0]->getShape().rotationAtOffset(at);
}


void
MSStage::setDestination(const MSEdge* newDestination, MSStoppingPlace* newDestStop) {
    myDestination = newDestination;
    myDestinationStop = newDestStop;
    if (newDestStop != nullptr) {
        myArrivalPos = (newDestStop->getBeginLanePosition() + newDestStop->getEndLanePosition()) / 2;
    }
}


/****************************************************************************/
