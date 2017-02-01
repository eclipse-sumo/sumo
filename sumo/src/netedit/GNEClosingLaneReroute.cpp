/****************************************************************************/
/// @file    GNEClosingLaneReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.cpp 22699 2017-01-25 14:56:03Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <utils/common/ToString.h>

#include "GNEClosingLaneReroute.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterInterval *rerouterIntervalParent, GNEEdge *closedEdge, std::vector<SUMOVehicleClass> allowVehicles, std::vector<SUMOVehicleClass> disallowVehicles) :
    myClosedEdge(closedEdge),
    myAllowVehicles(allowVehicles),
    myDisallowVehicles(disallowVehicles),
    myRerouterIntervalParent(rerouterIntervalParent),
    myTag(SUMO_TAG_CLOSING_LANE_REROUTE) {
    // edge cannot be NULL 
    assert(myClosedEdge);
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {
}


void
GNEClosingLaneReroute::insertAllowVehicle(SUMOVehicleClass vclass) {
    // Check if was already allowed
    if (std::find(myAllowVehicles.begin(), myAllowVehicles.end(), vclass) != myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " was already allowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdge->getID() + "'");
    }
    // insert in vector
    myAllowVehicles.push_back(vclass);
}


void
GNEClosingLaneReroute::removeAllowVehicle(SUMOVehicleClass vclass) {
    // Check if was already allowed
    std::vector<SUMOVehicleClass>::iterator i = std::find(myAllowVehicles.begin(), myAllowVehicles.end(), vclass);
    if ( i == myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " wasn't previously allowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdge->getID() + "'");
    } else {
        myAllowVehicles.erase(i);
    }
}


void
GNEClosingLaneReroute::insertDisallowVehicle(SUMOVehicleClass vclass) {
    // Check if was already disallowed
    if (std::find(myAllowVehicles.begin(), myDisallowVehicles.end(), vclass) != myDisallowVehicles.end()) {
        throw ProcessError(toString(vclass) + " was already disallowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdge->getID() + "'");
    }
    // insert in vector
    myDisallowVehicles.push_back(vclass);
}


void
GNEClosingLaneReroute::removeDisallowVehicle(SUMOVehicleClass vclass) {
    // Check if was already disallowed
    std::vector<SUMOVehicleClass>::iterator i = std::find(myDisallowVehicles.begin(), myDisallowVehicles.end(), vclass);
    if ( i == myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " wasn't previously disallowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdge->getID() + "'");
    } else {
        myDisallowVehicles.erase(i);
    }
}


const std::vector<SUMOVehicleClass>&
GNEClosingLaneReroute::getAllowVehicles() const {
    return myAllowVehicles;
}


const std::vector<SUMOVehicleClass>&
GNEClosingLaneReroute::getDisallowVehicles() const {
    return myDisallowVehicles;
}


GNEEdge*
GNEClosingLaneReroute::getClosedEdge() const {
    return myClosedEdge;
}


SumoXMLTag 
GNEClosingLaneReroute::getTag() const {
    return myTag;
}


GNERerouterInterval*
GNEClosingLaneReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}

/****************************************************************************/
