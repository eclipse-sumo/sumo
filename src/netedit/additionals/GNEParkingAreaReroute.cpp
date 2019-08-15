/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEParkingAreaReroute.cpp
/// @author  Jakob Erdmann
/// @date    May 2018
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/dialogs/GNERerouterIntervalDialog.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEParkingAreaReroute.h"
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

// ===========================================================================
// member method definitions
// ===========================================================================

GNEParkingAreaReroute::GNEParkingAreaReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAdditional(rerouterIntervalDialog->getEditedAdditional(), rerouterIntervalDialog->getEditedAdditional()->getViewNet(), GLO_REROUTER, SUMO_TAG_PARKING_ZONE_REROUTE, "", false,
{}, {}, {}, {rerouterIntervalDialog->getEditedAdditional(), rerouterIntervalDialog->getEditedAdditional()->getViewNet()->getNet()->getAttributeCarriers().additionals.at(SUMO_TAG_PARKING_AREA).begin()->second}, {}, {}, {}, {}, {}, {}) {
    // fill route type with default values
    setDefaultValues();
}


GNEParkingAreaReroute::GNEParkingAreaReroute(GNEAdditional* rerouterIntervalParent, GNEAdditional* newParkingArea, double probability, bool visible):
    GNEAdditional(rerouterIntervalParent, rerouterIntervalParent->getViewNet(), GLO_REROUTER, SUMO_TAG_PARKING_ZONE_REROUTE, "", false,
{}, {}, {}, {rerouterIntervalParent, newParkingArea}, {}, {}, {}, {}, {}, {}),
myProbability(probability),
myVisible(visible) {
}


GNEParkingAreaReroute::~GNEParkingAreaReroute() {}


void
GNEParkingAreaReroute::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNEParkingAreaReroute::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNEParkingAreaReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position
GNEParkingAreaReroute::getPositionInView() const {
    return getAdditionalParents().at(0)->getPositionInView();
}


Boundary
GNEParkingAreaReroute::getCenteringBoundary() const {
    return getAdditionalParents().at(0)->getCenteringBoundary();
}


std::string
GNEParkingAreaReroute::getParentName() const {
    return getAdditionalParents().at(0)->getID();
}


void
GNEParkingAreaReroute::drawGL(const GUIVisualizationSettings& /* s */) const {
    // Currently this additional isn't drawn
}


std::string
GNEParkingAreaReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_PARKING:
            return getAdditionalParents().at(1)->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_VISIBLE:
            return toString(myVisible);
        case GNE_ATTR_PARENT:
            return toString(getAdditionalParents().at(0)->getID());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingAreaReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PARKING:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_VISIBLE:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingAreaReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_PARKING:
            return isValidAdditionalID(value) && (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        case SUMO_ATTR_VISIBLE:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEParkingAreaReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNEParkingAreaReroute::getHierarchyName() const {
    return getTagStr() + ": " + getAdditionalParents().at(1)->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingAreaReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_PARKING:
            changeAdditionalParent(this, value, 1);
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case SUMO_ATTR_VISIBLE:
            myVisible = parse<bool>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
