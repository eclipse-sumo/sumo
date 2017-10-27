/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEClosingLaneReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
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
GNEClosingLaneReroute::operator==(const GNEClosingLaneReroute& closingReroute) const {
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
