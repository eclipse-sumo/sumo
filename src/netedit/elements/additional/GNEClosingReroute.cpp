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
/// @file    GNEClosingReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/
#include <config.h>

#include "GNEClosingReroute.h"
#include <netedit/changes/GNEChange_Attribute.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingReroute::GNEClosingReroute(GNENet* net) :
    GNEAdditional("", net, GLO_REROUTER_CLOSINGREROUTE, SUMO_TAG_CLOSING_REROUTE, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myClosedEdge(nullptr),
    myPermissions(0) {
    // reset default values
    resetDefaultValues();
}


GNEClosingReroute::GNEClosingReroute(GNEAdditional* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions permissions) :
    GNEAdditional(rerouterIntervalParent->getNet(), GLO_REROUTER_CLOSINGREROUTE, SUMO_TAG_CLOSING_REROUTE, "",
        {}, {}, {}, {rerouterIntervalParent}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myClosedEdge(closedEdge),
    myPermissions(permissions) {
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEClosingReroute::~GNEClosingReroute() {}


void
GNEClosingReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_CLOSING_REROUTE);
    device.writeAttr(SUMO_ATTR_ID, getAttribute(SUMO_ATTR_EDGE));
    if (getAttribute(SUMO_ATTR_ALLOW) != "authority") {
        if (!getAttribute(SUMO_ATTR_ALLOW).empty()) {
            device.writeAttr(SUMO_ATTR_ALLOW, getAttribute(SUMO_ATTR_ALLOW));
        } else {
            device.writeAttr(SUMO_ATTR_DISALLOW, getAttribute(SUMO_ATTR_DISALLOW));
        }
    }
    device.closeTag();
}


GNEMoveOperation*
GNEClosingReroute::getMoveOperation() {
    // GNEClosingReroutes cannot be moved
    return nullptr;
}


void
GNEClosingReroute::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


Position
GNEClosingReroute::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5 + 6.25, (getDrawPositionIndex() * -1) - getParentAdditionals().front()->getDrawPositionIndex() + 1, 0);
    // return signPosition
    return signPosition;
}


void
GNEClosingReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    myAdditionalBoundary.add(getPositionInView());
    myAdditionalBoundary.grow(5);
}


void
GNEClosingReroute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEClosingReroute::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEClosingReroute::drawGL(const GUIVisualizationSettings& s) const {
    // draw closing reroute as listed attribute
    drawListedAddtional(s, getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                        1, getParentAdditionals().front()->getDrawPositionIndex(),
                        RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_CLOSINGREROUTE,
                        getAttribute(SUMO_ATTR_EDGE));
}


std::string
GNEClosingReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_EDGE:
            return myClosedEdge->getID();
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myPermissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(myPermissions));
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEClosingReroute::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEClosingReroute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            return (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_ALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case GNE_ATTR_SELECTED:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEClosingReroute::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEClosingReroute::getPopUpID() const {
    return getTagStr();
}


std::string
GNEClosingReroute::getHierarchyName() const {
    return getTagStr() + ": " + myClosedEdge->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEClosingReroute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_EDGE:
            myClosedEdge = myNet->getAttributeCarriers()->retrieveEdge(value);
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
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEClosingReroute::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEClosingReroute::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
