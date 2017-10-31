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
#include "GNEUndoList.h"
#include "GNEChange_Attribute.h"
#include "GNERerouter.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNERerouterInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterInterval* rerouterIntervalParent, GNELane* closedLane, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles) :
    GNEAttributeCarrier(SUMO_TAG_CLOSING_LANE_REROUTE, ICON_EMPTY),
    myRerouterIntervalParent(rerouterIntervalParent),
    myClosedLane(closedLane),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles) {
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {}


void 
GNEClosingLaneReroute::writeClosingLaneReroute(OutputDevice& device) const {
    // open closing reroute tag
    device.openTag(getTag());
    // write Lane ID
    device.writeAttr(SUMO_ATTR_LANE, myClosedLane->getID());
    // write Allowed vehicles
    device.writeAttr(SUMO_ATTR_ALLOW, myAllowedVehicles);
    // write disallowed vehicles
    device.writeAttr(SUMO_ATTR_DISALLOW, myDisallowedVehicles);
    // close closing reroute tag
    device.closeTag();
}


GNERerouterInterval*
GNEClosingLaneReroute::getRerouterIntervalParent() const {
    return myRerouterIntervalParent;
}


std::string 
GNEClosingLaneReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
    case SUMO_ATTR_LANE:
        return myClosedLane->getID();
    case SUMO_ATTR_ALLOW:
        return toString(myAllowedVehicles);
    case SUMO_ATTR_DISALLOW:
        return toString(myDisallowedVehicles);
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void 
GNEClosingLaneReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
    case SUMO_ATTR_LANE:
    case SUMO_ATTR_ALLOW:
    case SUMO_ATTR_DISALLOW:
        undoList->p_add(new GNEChange_Attribute(this, key, value));
        break;
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEClosingLaneReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
        return (myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveLane(value, false) != NULL);
    case SUMO_ATTR_ALLOW:
        return true;
        /** falta **/
    case SUMO_ATTR_DISALLOW:
        return true;
        /** falta **/
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void 
GNEClosingLaneReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID: {
        myClosedLane = myRerouterIntervalParent->getRerouterParent()->getViewNet()->getNet()->retrieveLane(value);
        break;
    }
    case SUMO_ATTR_ALLOW: {
        myAllowedVehicles;
        break;
    }
    case SUMO_ATTR_DISALLOW: {
        myAllowedVehicles;
        break;
    }
    default:
        throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
