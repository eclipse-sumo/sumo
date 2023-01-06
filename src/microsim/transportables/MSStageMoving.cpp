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
/// @file    MSStageMoving.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// The common superclass for modelling walking and tranship
/****************************************************************************/
#include <config.h>

#include <utils/options/OptionsCont.h>
#include <utils/router/IntermodalEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/transportables/MSPModel.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSStageMoving.h>


/* -------------------------------------------------------------------------
* MSStageMoving - methods
* ----------------------------------------------------------------------- */
MSStageMoving::~MSStageMoving() {
    delete myState;
}

const MSEdge*
MSStageMoving::getEdge() const {
    if (myCurrentInternalEdge != nullptr) {
        return myCurrentInternalEdge;
    } else {
        return myRouteStep == myRoute.end() ? nullptr : *myRouteStep;
    }
}

const MSEdge*
MSStageMoving::getFromEdge() const {
    return myRoute.front();
}

ConstMSEdgeVector
MSStageMoving::getEdges() const {
    return myRoute;
}


double
MSStageMoving::getEdgePos(SUMOTime now) const {
    return myState == nullptr ? 0. : myState->getEdgePos(*this, now);
}

int
MSStageMoving::getDirection() const {
    return myState == nullptr ? MSPModel::UNDEFINED_DIRECTION : myState->getDirection(*this, MSNet::getInstance()->getCurrentTimeStep());
}


Position
MSStageMoving::getPosition(SUMOTime now) const {
    return myState == nullptr ? Position::INVALID : myState->getPosition(*this, now);
}

double
MSStageMoving::getAngle(SUMOTime now) const {
    return myState == nullptr ? 0. : myState->getAngle(*this, now);
}

SUMOTime
MSStageMoving::getWaitingTime(SUMOTime now) const {
    return myState == nullptr ? 0 : myState->getWaitingTime(*this, now);
}

double
MSStageMoving::getSpeed() const {
    return myState == nullptr ? 0. : myState->getSpeed(*this);
}

const MSLane*
MSStageMoving::getLane() const {
    return myState == nullptr ? nullptr : myState->getLane();
}

void
MSStageMoving::setRouteIndex(MSTransportable* const transportable, int routeOffset) {
    assert(routeOffset >= 0);
    assert(routeOffset < (int)myRoute.size());
    getEdge()->removeTransportable(transportable);
    myRouteStep = myRoute.begin() + routeOffset;
    getEdge()->addTransportable(transportable);
}

void
MSStageMoving::replaceRoute(MSTransportable* const transportable, const ConstMSEdgeVector& edges, int routeOffset) {
    assert(routeOffset >= 0);
    assert(routeOffset < (int)edges.size());
    getEdge()->removeTransportable(transportable);
    myRoute = edges;
    myRouteStep = myRoute.begin() + routeOffset;
    getEdge()->addTransportable(transportable);
}


const MSLane*
MSStageMoving::checkDepartLane(const MSEdge* edge, SUMOVehicleClass svc, int laneIndex, const std::string& id) {
    const MSLane* lane = getSidewalk<MSEdge, MSLane>(edge, svc);
    if (laneIndex > 0) {
        const std::vector<MSLane*>& departLanes = edge->getLanes();
        if ((int)departLanes.size() <= laneIndex || !departLanes[laneIndex]->allowsVehicleClass(svc)) {
            std::string error = "Invalid departLane '" + toString(laneIndex) + "' for person '" + id + "'";
            if (OptionsCont::getOptions().getBool("ignore-route-errors")) {
                WRITE_WARNING(error);
                return nullptr;
            } else {
                throw ProcessError(error);
            }
        } else {
            lane = departLanes[laneIndex];
        }
    }
    return lane;
}


/****************************************************************************/
