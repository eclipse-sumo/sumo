/****************************************************************************/
/// @file    TraCI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/PositionVector.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSFrame.h>
#include <microsim/MSLane.h>
#include <microsim/MSRouteHandler.h>
#include "TraCI.h"

// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string> TraCI::myLoadArgs;

// ===========================================================================
// member definitions
// ===========================================================================
/* void
TraCI::connect(const std::string& host, int port) {
}*/

void
TraCI::load(const std::vector<std::string>& args) {
    myLoadArgs = args;
}

void
TraCI::close() {
}

/* void
TraCI::subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const {
}

void
TraCI::subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<
        int>& vars) const {
} */

const TraCI::SubscribedValues&
TraCI::getSubscriptionResults() const {
    return mySubscribedValues;
}

const TraCI::TraCIValues&
TraCI::getSubscriptionResults(const std::string& objID) const {
    if (mySubscribedValues.find(objID) != mySubscribedValues.end()) {
        return mySubscribedValues.find(objID)->second;
    } else {
        throw; // Something?
    }
}

const TraCI::SubscribedContextValues&
TraCI::getContextSubscriptionResults() const {
    return mySubscribedContextValues;
}

const TraCI::SubscribedValues&
TraCI::getContextSubscriptionResults(const std::string& objID) const {
    if (mySubscribedContextValues.find(objID) != mySubscribedContextValues.end()) {
        return mySubscribedContextValues.find(objID)->second;
    } else {
        throw; // Something?
    }
}


TraCIPositionVector
TraCI::makeTraCIPositionVector(const PositionVector& positionVector) {
    TraCIPositionVector tp;
    for (int i = 0; i < (int)positionVector.size(); ++i) {
        tp.push_back(makeTraCIPosition(positionVector[i]));
    }
    return tp;
}


PositionVector
TraCI::makePositionVector(const TraCIPositionVector& vector) {
    PositionVector pv;
    for (int i = 0; i < (int)vector.size(); i++) {
        pv.push_back(Position(vector[i].x, vector[i].y));
    }
    return pv;
}


TraCIColor
TraCI::makeTraCIColor(const RGBColor& color) {
    TraCIColor tc;
    tc.a = color.alpha();
    tc.b = color.blue();
    tc.g = color.green();
    tc.r = color.red();
    return tc;
}

RGBColor
TraCI::makeRGBColor(const TraCIColor& c) {
    return RGBColor((unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a);
}


TraCIPosition
TraCI::makeTraCIPosition(const Position& position) {
    TraCIPosition p;
    p.x = position.x();
    p.y = position.y();
    p.z = position.z();
    return p;
}

Position
TraCI::makePosition(const TraCIPosition& tpos) {
    Position p;
    p.set(tpos.x, tpos.y, tpos.z);
    return p;
}

MSEdge*
TraCI::getEdge(const std::string& edgeID) {
    MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == 0) {
        throw TraCIException("Referenced edge '" + edgeID + "' is not known.");
    }
    return edge;
}

const MSLane*
TraCI::getLaneChecking(const std::string& edgeID, int laneIndex, double pos) {
    const MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == 0) {
        throw TraCIException("Unknown edge " + edgeID);
    }
    if (laneIndex < 0 || laneIndex >= (int)edge->getLanes().size()) {
        throw TraCIException("Invalid lane index for " + edgeID);
    }
    const MSLane* lane = edge->getLanes()[laneIndex];
    if (pos < 0 || pos > lane->getLength()) {
        throw TraCIException("Position on lane invalid");
    }
    return lane;
}

std::pair<MSLane*, double>
TraCI::convertCartesianToRoadMap(Position pos) {
    /// XXX use rtree instead
    std::pair<MSLane*, double> result;
    std::vector<std::string> allEdgeIds;
    double minDistance = std::numeric_limits<double>::max();

    allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();
    for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
        const std::vector<MSLane*>& allLanes = MSEdge::dictionary((*itId))->getLanes();
        for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
            const double newDistance = (*itLane)->getShape().distance2D(pos);
            if (newDistance < minDistance) {
                minDistance = newDistance;
                result.first = (*itLane);
            }
        }
    }
    // @todo this may be a place where 3D is required but 2D is delivered
    result.second = result.first->getShape().nearest_offset_to_point2D(pos, false);
    return result;
}



/****************************************************************************/
