/****************************************************************************/
/// @file    GNEClosingReroute.cpp
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

#include "GNEClosingReroute.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingReroute::GNEClosingReroute(GNERerouterInterval& rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) :
    myRerouterIntervalParent(&rerouterIntervalParent),
    myClosedEdge(closedEdge),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles),
    myTag(SUMO_TAG_CLOSING_REROUTE) {
}


GNEClosingReroute::~GNEClosingReroute() {
}


SVCPermissions
GNEClosingReroute::getAllowedVehicles() const {
    return myAllowedVehicles;
}


SVCPermissions
GNEClosingReroute::getDisallowedVehicles() const {
    return myDisallowedVehicles;
}


void
GNEClosingReroute::setAllowedVehicles(SVCPermissions allowed) {
    myAllowedVehicles = allowed;
}


void
GNEClosingReroute::setDisallowedVehicles(SVCPermissions disallowed) {
    myDisallowedVehicles = disallowed;
}


GNEEdge*
GNEClosingReroute::getClosedEdge() const {
    return myClosedEdge;
}


void
GNEClosingReroute::setClosedEdge(GNEEdge* edge) {
    myClosedEdge = edge;
}


SumoXMLTag
GNEClosingReroute::getTag() const {
    return myTag;
}


const GNERerouterInterval&
GNEClosingReroute::getRerouterIntervalParent() const {
    return *myRerouterIntervalParent;
}


bool
GNEClosingReroute::operator==(const GNEClosingReroute& closingReroute) {
    if ((myRerouterIntervalParent == closingReroute.myRerouterIntervalParent) &&
            (myClosedEdge == closingReroute.myClosedEdge) &&
            (myAllowedVehicles == closingReroute.myAllowedVehicles) &&
            (myDisallowedVehicles == closingReroute.myDisallowedVehicles)) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
