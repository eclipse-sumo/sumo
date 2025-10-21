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
/// @file    GNEClosingReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>

#include "GNEClosingReroute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingReroute::GNEClosingReroute(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_CLOSING_REROUTE, ""),
    GNEAdditionalListed(this) {
}


GNEClosingReroute::GNEClosingReroute(GNEAdditional* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions permissions) :
    GNEAdditional(rerouterIntervalParent, SUMO_TAG_CLOSING_REROUTE, ""),
    GNEAdditionalListed(this),
    myClosedEdge(closedEdge),
    myPermissions(permissions) {
    // set parents
    setParent<GNEAdditional*>(rerouterIntervalParent);
    setParent<GNEEdge*>(closedEdge);
    // update boundary of rerouter parent
    rerouterIntervalParent->getParentAdditionals().front()->updateCenteringBoundary(true);
}


GNEClosingReroute::~GNEClosingReroute() {}


GNEMoveElement*
GNEClosingReroute::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEClosingReroute::getParameters() {
    return nullptr;
}


const Parameterised*
GNEClosingReroute::getParameters() const {
    return nullptr;
}


void
GNEClosingReroute::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_CLOSING_REROUTE);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
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


bool
GNEClosingReroute::isAdditionalValid() const {
    return true;
}


std::string
GNEClosingReroute::getAdditionalProblem() const {
    return "";
}


void
GNEClosingReroute::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEClosingReroute::checkDrawMoveContour() const {
    return false;
}


void
GNEClosingReroute::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNEClosingReroute::getPositionInView() const {
    return getListedPositionInView();
}


void
GNEClosingReroute::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
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
    drawListedAdditional(s, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_CLOSINGREROUTE,
                         getAttribute(SUMO_ATTR_EDGE));
}


std::string
GNEClosingReroute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGE:
            return myClosedEdge->getID();
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myPermissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(myPermissions));
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNEClosingReroute::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEClosingReroute::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEClosingReroute::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
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
        default:
            return isCommonAttributeValid(key, value);
    }
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
            setAdditionalID(value);
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
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
