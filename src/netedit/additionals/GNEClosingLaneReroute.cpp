/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <config.h>

#include <utils/common/ToString.h>
#include "GNEClosingLaneReroute.h"
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterIntervalDialog.h>

#include <netedit/GNEUndoList.h>
#include "GNERerouter.h"
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include "GNERerouterInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAdditional(rerouterIntervalDialog->getEditedRerouterInterval(), rerouterIntervalDialog->getEditedRerouterInterval()->getViewNet(), GLO_REROUTER, SUMO_TAG_CLOSING_LANE_REROUTE, false, false),
    myClosedLane(rerouterIntervalDialog->getEditedRerouterInterval()->getAdditionalParent()->getEdgeChilds().at(0)->getLanes().at(0)),
    myAllowedVehicles(getTagProperties(SUMO_TAG_CLOSING_LANE_REROUTE).getDefaultValue(SUMO_ATTR_ALLOW)),
    myDisallowedVehicles(getTagProperties(SUMO_TAG_CLOSING_LANE_REROUTE).getDefaultValue(SUMO_ATTR_DISALLOW)) {
}


GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterInterval* rerouterIntervalParent, GNELane* closedLane, const std::string &allowedVehicles, const std::string &disallowedVehicles) :
    GNEAdditional(rerouterIntervalParent, rerouterIntervalParent->getViewNet(), GLO_REROUTER, SUMO_TAG_CLOSING_LANE_REROUTE, false, false),
    myClosedLane(closedLane),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles) {
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {}


void 
GNEClosingLaneReroute::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNEClosingLaneReroute::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void 
GNEClosingLaneReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNEClosingLaneReroute::getPositionInView() const {
    return Position();
}


std::string 
GNEClosingLaneReroute::getParentName() const {
    return myAdditionalParent->getID();
}


void 
GNEClosingLaneReroute::drawGL(const GUIVisualizationSettings& s) const {
    // Currently This additional isn't drawn
}


std::string
GNEClosingLaneReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myClosedLane->getID();
        case SUMO_ATTR_ALLOW:
            return myAllowedVehicles;
        case SUMO_ATTR_DISALLOW:
            return myDisallowedVehicles;
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
        case SUMO_ATTR_ID:
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
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            return (myViewNet->getNet()->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_ALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
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
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myClosedLane = myViewNet->getNet()->retrieveLane(value);
            break;
        case SUMO_ATTR_ALLOW:
            myAllowedVehicles = value;
            break;
        case SUMO_ATTR_DISALLOW:
            myAllowedVehicles = value;
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
