/****************************************************************************/
/// @file    GNEClosingLaneReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
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
#include "GNELane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterInterval& rerouterIntervalParent, GNELane* closedLane, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) :
    myRerouterIntervalParent(&rerouterIntervalParent),
    myClosedLane(closedLane),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles),
    myTag(SUMO_TAG_CLOSING_LANE_REROUTE) {
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {
}

SVCPermissions
GNEClosingLaneReroute::getAllowedVehicles() const {
    return myAllowedVehicles;
}


SVCPermissions
GNEClosingLaneReroute::getDisallowedVehicles() const {
    return myDisallowedVehicles;
}


void
GNEClosingLaneReroute::setAllowedVehicles(SVCPermissions allowed) {
    myAllowedVehicles = allowed;
}


void
GNEClosingLaneReroute::setDisallowedVehicles(SVCPermissions disallowed) {
    myDisallowedVehicles = disallowed;
}


GNELane*
GNEClosingLaneReroute::getClosedLane() const {
    return myClosedLane;
}


void
GNEClosingLaneReroute::setClosedLane(GNELane* lane) {
    myClosedLane = lane;
}


SumoXMLTag
GNEClosingLaneReroute::getTag() const {
    return myTag;
}


const GNERerouterInterval&
GNEClosingLaneReroute::getRerouterIntervalParent() const {
    return *myRerouterIntervalParent;
}


bool
GNEClosingLaneReroute::operator==(const GNEClosingLaneReroute& closingReroute) {
    if ((myRerouterIntervalParent == closingReroute.myRerouterIntervalParent) &&
            (myClosedLane == closingReroute.myClosedLane) &&
            (myAllowedVehicles == closingReroute.myAllowedVehicles) &&
            (myDisallowedVehicles == closingReroute.myDisallowedVehicles)) {
        return true;
    } else {
        return false;
    }
}
/****************************************************************************/
