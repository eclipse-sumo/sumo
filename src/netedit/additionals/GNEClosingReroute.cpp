/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <config.h>

#include <utils/common/ToString.h>
#include "GNEClosingReroute.h"
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

GNEClosingReroute::GNEClosingReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAdditional(rerouterIntervalDialog->getEditedRerouterInterval(), rerouterIntervalDialog->getEditedRerouterInterval()->getViewNet(), GLO_CALIBRATOR, SUMO_TAG_CLOSING_REROUTE, false, false),
    myClosedEdge(rerouterIntervalDialog->getEditedRerouterInterval()->getAdditionalParent()->getEdgeChilds().at(0)),
    myAllowedVehicles(getTagProperties(SUMO_TAG_CLOSING_LANE_REROUTE).getDefaultValue(SUMO_ATTR_ALLOW)),
    myDisallowedVehicles(getTagProperties(SUMO_TAG_CLOSING_LANE_REROUTE).getDefaultValue(SUMO_ATTR_DISALLOW)) {
}


GNEClosingReroute::GNEClosingReroute(GNERerouterInterval* rerouterIntervalParent, GNEEdge* closedEdge, const std::string &allowedVehicles, const std::string &disallowedVehicles) :
    GNEAdditional(rerouterIntervalParent, rerouterIntervalParent->getViewNet(), GLO_CALIBRATOR, SUMO_TAG_CLOSING_REROUTE, false, false),
    myClosedEdge(closedEdge),
    myAllowedVehicles(allowedVehicles),
    myDisallowedVehicles(disallowedVehicles) {
}


GNEClosingReroute::~GNEClosingReroute() {}


void 
GNEClosingReroute::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNEClosingReroute::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void 
GNEClosingReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNEClosingReroute::getPositionInView() const {
    return Position();
}


std::string 
GNEClosingReroute::getParentName() const {
    return myAdditionalParent->getID();
}


void 
GNEClosingReroute::drawGL(const GUIVisualizationSettings& /* s */) const {
    // Currently this additional isn't drawn
}


std::string
GNEClosingReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myClosedEdge->getID();
        case SUMO_ATTR_ALLOW:
            return myAllowedVehicles;
        case SUMO_ATTR_DISALLOW:
            return myDisallowedVehicles;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEClosingReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEClosingReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            return (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
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
GNEClosingReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            myClosedEdge = myViewNet->getNet()->retrieveEdge(value);
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
