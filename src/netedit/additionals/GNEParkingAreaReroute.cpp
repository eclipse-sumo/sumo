/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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

#include <utils/common/ToString.h>
#include <netedit/dialogs/GNERerouterIntervalDialog.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEParkingArea.h"
#include "GNEParkingAreaReroute.h"
#include <netedit/GNEUndoList.h>
#include "GNERerouter.h"
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include "GNERerouterInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEParkingAreaReroute::GNEParkingAreaReroute(GNERerouterIntervalDialog* rerouterIntervalDialog) :
    GNEAdditional(rerouterIntervalDialog->getEditedRerouterInterval(), rerouterIntervalDialog->getEditedRerouterInterval()->getViewNet(), GLO_REROUTER, SUMO_TAG_PARKING_ZONE_REROUTE, false, false),
    myParkingArea(nullptr),
    myProbability(parse<double>(getTagProperties(SUMO_TAG_PARKING_ZONE_REROUTE).getDefaultValue(SUMO_ATTR_PROB))) {
}


GNEParkingAreaReroute::GNEParkingAreaReroute(GNERerouterInterval* rerouterIntervalParent, GNEParkingArea* newParkingArea, double probability):
    GNEAdditional(rerouterIntervalParent, rerouterIntervalParent->getViewNet(), GLO_REROUTER, SUMO_TAG_PARKING_ZONE_REROUTE, false, false),
    myParkingArea(newParkingArea),
    myProbability(probability) {
}


GNEParkingAreaReroute::~GNEParkingAreaReroute() {}


void 
GNEParkingAreaReroute::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNEParkingAreaReroute::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void 
GNEParkingAreaReroute::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNEParkingAreaReroute::getPositionInView() const {
    return Position();
}


std::string 
GNEParkingAreaReroute::getParentName() const {
    return myAdditionalParent->getID();
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
            return myParkingArea == nullptr ? "" : myParkingArea->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingAreaReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
         return isValidAdditionalID(value);
        case SUMO_ATTR_PARKING: {
            GNEAdditional* a = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false);
            return (a != nullptr) && (a->getTag() == SUMO_TAG_PARKING_AREA);
        }
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
            myParkingArea = dynamic_cast<GNEParkingArea*>(myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value));
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
