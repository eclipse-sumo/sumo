/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEParkingAreaReroute.cpp
/// @author  Jakob Erdmann
/// @date    May 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEParkingAreaReroute.h"
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>

// ===========================================================================
// member method definitions
// ===========================================================================

GNEParkingAreaReroute::GNEParkingAreaReroute(GNENet* net):
    GNEAdditional("", net, GLO_REROUTER, SUMO_TAG_PARKING_AREA_REROUTE, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myProbability(0),
    myVisible(0) {
    // reset default values
    resetDefaultValues();
}


GNEParkingAreaReroute::GNEParkingAreaReroute(GNEAdditional* rerouterIntervalParent, GNEAdditional* newParkingArea, double probability, bool visible):
    GNEAdditional(rerouterIntervalParent->getNet(), GLO_REROUTER, SUMO_TAG_PARKING_AREA_REROUTE, "",
        {}, {}, {}, {rerouterIntervalParent, newParkingArea}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myProbability(probability),
    myVisible(visible) {
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEParkingAreaReroute::~GNEParkingAreaReroute() {}


void 
GNEParkingAreaReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_PARKING_AREA_REROUTE);
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_PARKING));
    if (myProbability != 1.0) {
        device.writeAttr(SUMO_ATTR_PROB, myProbability);
    }
    if (myVisible) {
        device.writeAttr(SUMO_ATTR_VISIBLE, true);
    }
    device.closeTag();
}


GNEMoveOperation*
GNEParkingAreaReroute::getMoveOperation() {
    // GNEParkingAreaReroutes cannot be moved
    return nullptr;
}


void
GNEParkingAreaReroute::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


Position
GNEParkingAreaReroute::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5 + 6.25, (getDrawPositionIndex() * -1) - getParentAdditionals().front()->getDrawPositionIndex() + 1, 0);
    // return signPosition
    return signPosition;
}


void
GNEParkingAreaReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    myAdditionalBoundary.add(getPositionInView());
    myAdditionalBoundary.grow(5);
}


void
GNEParkingAreaReroute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEParkingAreaReroute::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEParkingAreaReroute::drawGL(const GUIVisualizationSettings& s) const {
    // draw route prob reroute as listed attribute
    drawListedAddtional(s, getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                        1, getParentAdditionals().front()->getDrawPositionIndex(),
                        RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_PARKINGAREAREROUTE,
                        getAttribute(SUMO_ATTR_PARKING) + ": " + getAttribute(SUMO_ATTR_PROB));
}


std::string
GNEParkingAreaReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_PARKING:
            return getParentAdditionals().at(1)->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_VISIBLE:
            return toString(myVisible);
        case GNE_ATTR_PARENT:
            return toString(getParentAdditionals().at(0)->getID());
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEParkingAreaReroute::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
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
            return isValidAdditionalID(value) && (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        case SUMO_ATTR_VISIBLE:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingAreaReroute::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEParkingAreaReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNEParkingAreaReroute::getHierarchyName() const {
    return getTagStr() + ": " + getParentAdditionals().at(1)->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingAreaReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_PARKING:
            replaceAdditionalParent(SUMO_TAG_PARKING_AREA, value, 1);
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case SUMO_ATTR_VISIBLE:
            myVisible = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingAreaReroute::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEParkingAreaReroute::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
