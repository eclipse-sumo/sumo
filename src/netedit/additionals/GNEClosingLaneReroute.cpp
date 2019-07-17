/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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

#include "GNEClosingLaneReroute.h"
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterIntervalDialog.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAdditional(rerouterIntervalDialog->getEditedAdditional(), rerouterIntervalDialog->getEditedAdditional()->getViewNet(), GLO_REROUTER, SUMO_TAG_CLOSING_LANE_REROUTE, "", false,
{}, {}, {}, {rerouterIntervalDialog->getEditedAdditional()}, {}, {}, {}, {}, {}, {}),
myClosedLane(rerouterIntervalDialog->getEditedAdditional()->getAdditionalParents().at(0)->getEdgeChildren().at(0)->getLanes().at(0)) {
    // fill closing lane reroute interval with default values
    setDefaultValues();
}


GNEClosingLaneReroute::GNEClosingLaneReroute(GNEAdditional* rerouterIntervalParent, GNELane* closedLane, SVCPermissions permissions) :
    GNEAdditional(rerouterIntervalParent, rerouterIntervalParent->getViewNet(), GLO_REROUTER, SUMO_TAG_CLOSING_LANE_REROUTE, "", false,
{}, {}, {}, {rerouterIntervalParent}, {}, {}, {}, {}, {}, {}),
myClosedLane(closedLane),
myPermissions(permissions) {
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {}


void
GNEClosingLaneReroute::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNEClosingLaneReroute::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNEClosingLaneReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position
GNEClosingLaneReroute::getPositionInView() const {
    return getAdditionalParents().at(0)->getPositionInView();
}


Boundary
GNEClosingLaneReroute::getCenteringBoundary() const {
    return myClosedLane->getCenteringBoundary();
}


std::string
GNEClosingLaneReroute::getParentName() const {
    return getAdditionalParents().at(0)->getID();
}


void
GNEClosingLaneReroute::drawGL(const GUIVisualizationSettings& /* s */) const {
    // Currently this additional isn't drawn
}


std::string
GNEClosingLaneReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myClosedLane->getID();
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myPermissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(myPermissions));
        case GNE_ATTR_PARENT:
            return getAdditionalParents().at(0)->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEClosingLaneReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNEClosingLaneReroute::getHierarchyName() const {
    return getTagStr() + ": " + myClosedLane->getID();
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
            myPermissions = parseVehicleClasses(value);
            break;
        case SUMO_ATTR_DISALLOW:
            myPermissions = invertPermissions(parseVehicleClasses(value));
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
