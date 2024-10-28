/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEClosingLaneReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/
#include <config.h>

#include "GNEClosingLaneReroute.h"
#include <netedit/changes/GNEChange_Attribute.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNENet* net) :
    GNEAdditional("", net, GLO_REROUTER_CLOSINGLANEREROUTE, SUMO_TAG_CLOSING_LANE_REROUTE,
                  GUIIconSubSys::getIcon(GUIIcon::CLOSINGLANEREROUTE), "", {}, {}, {}, {}, {}, {}),
                            myClosedLane(nullptr),
myPermissions(0) {
    // reset default values
    resetDefaultValues();
}


GNEClosingLaneReroute::GNEClosingLaneReroute(GNEAdditional* rerouterIntervalParent, GNELane* closedLane, SVCPermissions permissions) :
    GNEAdditional(rerouterIntervalParent->getNet(), GLO_REROUTER_CLOSINGLANEREROUTE, SUMO_TAG_CLOSING_LANE_REROUTE,
                  GUIIconSubSys::getIcon(GUIIcon::CLOSINGLANEREROUTE), "", {}, {}, {}, {rerouterIntervalParent}, {}, {}),
myClosedLane(closedLane),
myPermissions(permissions) {
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {}


void
GNEClosingLaneReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_CLOSING_LANE_REROUTE);
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_LANE));
    if (getAttribute(SUMO_ATTR_ALLOW) != getVehicleClassNames(SVCAll)) {
        if (!getAttribute(SUMO_ATTR_ALLOW).empty()) {
            device.writeAttr(SUMO_ATTR_ALLOW, getAttribute(SUMO_ATTR_ALLOW));
        } else {
            device.writeAttr(SUMO_ATTR_DISALLOW, getAttribute(SUMO_ATTR_DISALLOW));
        }
    }
    device.closeTag();
}


bool
GNEClosingLaneReroute::isAdditionalValid() const {
    return true;
}


std::string
GNEClosingLaneReroute::getAdditionalProblem() const {
    return "";
}


void
GNEClosingLaneReroute::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEClosingLaneReroute::checkDrawMoveContour() const {
    return false;
}


GNEMoveOperation*
GNEClosingLaneReroute::getMoveOperation() {
    // GNEClosingLaneReroute cannot be moved
    return nullptr;
}


void
GNEClosingLaneReroute::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


Position
GNEClosingLaneReroute::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5 + 6.25, (getDrawPositionIndex() * -1) - getParentAdditionals().front()->getDrawPositionIndex() + 1, 0);
    // return signPosition
    return signPosition;
}


void
GNEClosingLaneReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNEClosingLaneReroute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEClosingLaneReroute::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEClosingLaneReroute::drawGL(const GUIVisualizationSettings& s) const {
    // draw closing lane reroute as listed attribute
    drawListedAdditional(s, getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                         1, getParentAdditionals().front()->getDrawPositionIndex(),
                         RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_CLOSINGLANEREROUTE,
                         getAttribute(SUMO_ATTR_LANE));
}


std::string
GNEClosingLaneReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return myClosedLane->getID();
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myPermissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(myPermissions));
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEClosingLaneReroute::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


const Parameterised::Map&
GNEClosingLaneReroute::getACParametersMap() const {
    return PARAMETERS_EMPTY;
}


void
GNEClosingLaneReroute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
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
            return (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case GNE_ATTR_SELECTED:
            return canParse<double>(value);
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
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myClosedLane = myNet->getAttributeCarriers()->retrieveLane(value);
            break;
        case SUMO_ATTR_ALLOW:
            myPermissions = parseVehicleClasses(value);
            break;
        case SUMO_ATTR_DISALLOW:
            myPermissions = invertPermissions(parseVehicleClasses(value));
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEClosingLaneReroute::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEClosingLaneReroute::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
