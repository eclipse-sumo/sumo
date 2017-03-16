/****************************************************************************/
/// @file    TraCI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
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
#include "../../config.h"
#endif

#include <utils/geom/PositionVector.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <microsim/MSEdge.h>
#include "TraCI.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member definitions
// ===========================================================================
void
TraCI::connect(const std::string& host, int port) {
}

void
TraCI::close() {
}

void
TraCI::subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const {
}

void
TraCI::subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<
        int>& vars) const {
}

const TraCI::SubscribedValues&
TraCI::getSubscriptionResults() const {
    return mySubscribedValues;
}

const TraCI::TraCIValues&
TraCI::getSubscriptionResults(const std::string& objID) const {
    if (mySubscribedValues.find(objID)!=mySubscribedValues.end()) {
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
    if (mySubscribedContextValues.find(objID)!=mySubscribedContextValues.end()) {
        return mySubscribedContextValues.find(objID)->second;
    } else {
        throw; // Something?
    }
}


TraCIPositionVector 
TraCI::makeTraCIPositionVector(const PositionVector& positionVector) {
    TraCIPositionVector tp;
    for (int i = 0; i<positionVector.size(); ++i) {
        TraCIPosition pos = makeTraCIPosition(positionVector[i]);
        tp.push_back(pos);
    }
    return tp;
}


PositionVector 
TraCI::makePositionVector(const TraCIPositionVector& vector) {
    PositionVector pv;
    for (int i = 0; i<vector.size(); i++) {
        Position p;
        p.setx(vector[i].x);
        p.sety(vector[i].y);
        pv.push_back(p);
    }
    return pv;
}


TraCIColor 
TraCI::makeTraCIColor(RGBColor color) {
    TraCIColor tc;
    tc.a = color.alpha();
    tc.b = color.blue();
    tc.g = color.green();
    tc.r = color.red();
    return tc;
}

RGBColor 
TraCI::makeRGBColor(const TraCIColor& c) {
    RGBColor rgbColor;
    rgbColor.set((unsigned char)c.r,(unsigned char)c.g,(unsigned char)c.b,(unsigned char)c.a);
    return rgbColor;
}


TraCIPosition 
TraCI::makeTraCIPosition(const Position& position) {
    TraCIPosition p;
    p.x = position.x();
    p.y = position.y();
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

/****************************************************************************/
