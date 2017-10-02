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
/// @file    GNEClosingReroute.cpp
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

#include "GNEClosingReroute.h"
#include "GNEEdge.h"


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
GNEClosingReroute::operator==(const GNEClosingReroute& closingReroute) const {
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
