/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEParkingAreaReroute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEParkingAreaReroute::GNEParkingAreaReroute(GNENet* net):
    GNEAdditional("", net, "", SUMO_TAG_PARKING_AREA_REROUTE, ""),
    GNEAdditionalListed(this) {
}


GNEParkingAreaReroute::GNEParkingAreaReroute(GNEAdditional* rerouterIntervalParent, GNEAdditional* newParkingArea,
        const double probability, const bool visible):
    GNEAdditional(rerouterIntervalParent, SUMO_TAG_PARKING_AREA_REROUTE, ""),
    GNEAdditionalListed(this),
    myProbability(probability),
    myVisible(visible) {
    // set parents
    setParents<GNEAdditional*>({rerouterIntervalParent, newParkingArea});
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEParkingAreaReroute::~GNEParkingAreaReroute() {}


GNEMoveElement*
GNEParkingAreaReroute::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEParkingAreaReroute::getParameters() {
    return nullptr;
}


const Parameterised*
GNEParkingAreaReroute::getParameters() const {
    return nullptr;
}


void
GNEParkingAreaReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_PARKING_AREA_REROUTE);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_PARKING));
    if (myProbability != 1.0) {
        device.writeAttr(SUMO_ATTR_PROB, myProbability);
    }
    if (myVisible) {
        device.writeAttr(SUMO_ATTR_VISIBLE, true);
    }
    device.closeTag();
}


bool
GNEParkingAreaReroute::isAdditionalValid() const {
    return true;
}


std::string
GNEParkingAreaReroute::getAdditionalProblem() const {
    return "";
}


void
GNEParkingAreaReroute::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEParkingAreaReroute::checkDrawMoveContour() const {
    return false;
}


void
GNEParkingAreaReroute::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNEParkingAreaReroute::getPositionInView() const {
    return getListedPositionInView();
}


void
GNEParkingAreaReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
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
    // draw dest prob reroute as listed attribute
    drawListedAdditional(s, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_PARKINGAREAREROUTE,
                         getAttribute(SUMO_ATTR_PARKING) + ": " + getAttribute(SUMO_ATTR_PROB));
}


std::string
GNEParkingAreaReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_PARKING:
            return getParentAdditionals().at(1)->getID();
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_VISIBLE:
            return toString(myVisible);
        case GNE_ATTR_PARENT:
            return toString(getParentAdditionals().at(0)->getID());
        default:
            return getCommonAttribute(key);
    }
}


double
GNEParkingAreaReroute::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEParkingAreaReroute::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEParkingAreaReroute::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEParkingAreaReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_PARKING:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_PARKING_AREA, value, false) != nullptr);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && parse<double>(value) >= 0 && parse<double>(value) <= 1;
        case SUMO_ATTR_VISIBLE:
            return canParse<bool>(value);
        default:
            return isCommonAttributeValid(key, value);
    }
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
            setAdditionalID(value);
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
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
