/****************************************************************************/
/// @file    GNEClosingReroute.cpp
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

#include "GNEClosingReroute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingReroute::GNEClosingReroute(GNERerouterInterval *rerouterIntervalParent, std::string closedEdgeId, std::vector<SUMOVehicleClass> allowVehicles, std::vector<SUMOVehicleClass> disallowVehicles) :
    myClosedEdgeId(closedEdgeId),
    myAllowVehicles(allowVehicles),
    myDisallowVehicles(disallowVehicles),
    myRerouterIntervalParent(rerouterIntervalParent),
    myTag(SUMO_TAG_CLOSING_REROUTE) {
}


GNEClosingReroute::~GNEClosingReroute() {
}


void
GNEClosingReroute::insertAllowVehicle(SUMOVehicleClass vclass) {
    // Check if was already allowed
    if (std::find(myAllowVehicles.begin(), myAllowVehicles.end(), vclass) != myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " was already allowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdgeId + "'");
    }
    // insert in vector
    myAllowVehicles.push_back(vclass);
}


void
GNEClosingReroute::removeAllowVehicle(SUMOVehicleClass vclass) {
    // Check if was already allowed
    std::vector<SUMOVehicleClass>::iterator i = std::find(myAllowVehicles.begin(), myAllowVehicles.end(), vclass);
    if ( i == myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " wasn't previously allowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdgeId + "'");
    } else {
        myAllowVehicles.erase(i);
    }
}


void
GNEClosingReroute::insertDisallowVehicle(SUMOVehicleClass vclass) {
    // Check if was already disallowed
    if (std::find(myAllowVehicles.begin(), myDisallowVehicles.end(), vclass) != myDisallowVehicles.end()) {
        throw ProcessError(toString(vclass) + " was already disallowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdgeId + "'");
    }
    // insert in vector
    myDisallowVehicles.push_back(vclass);
}


void
GNEClosingReroute::removeDisallowVehicle(SUMOVehicleClass vclass) {
    // Check if was already disallowed
    std::vector<SUMOVehicleClass>::iterator i = std::find(myDisallowVehicles.begin(), myDisallowVehicles.end(), vclass);
    if ( i == myAllowVehicles.end()) {
        throw ProcessError(toString(vclass) + " wasn't previously disallowed in " + toString(myTag) + " with closed edge ID ='" + myClosedEdgeId + "'");
    } else {
        myDisallowVehicles.erase(i);
    }
}


const std::vector<SUMOVehicleClass>&
GNEClosingReroute::getAllowVehicles() const {
    return myAllowVehicles;
}


const std::vector<SUMOVehicleClass>&
GNEClosingReroute::getDisallowVehicles() const {
    return myDisallowVehicles;
}


std::string
GNEClosingReroute::getClosedEdgeId() const {
    return myClosedEdgeId;
}


SumoXMLTag 
GNEClosingReroute::getTag() const {
    return myTag;
}


GNERerouterInterval*
GNEClosingReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}

/****************************************************************************/
