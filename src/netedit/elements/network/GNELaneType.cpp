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
/// @file    GNELaneType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
/// A SUMO lane type file assigns default values for certain attributes to types of roads.
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <utils/options/OptionsCont.h>

#include "GNELaneType.h"
#include "GNEEdgeType.h"

// ===========================================================================
// members methods
// ===========================================================================

GNELaneType::GNELaneType(GNEEdgeType* edgeTypeParent) :
    GNENetworkElement(edgeTypeParent->getNet(), "", SUMO_TAG_LANETYPE),
    myEdgeTypeParent(edgeTypeParent) {
}


GNELaneType::GNELaneType(GNEEdgeType* edgeTypeParent, const NBTypeCont::LaneTypeDefinition& laneType) :
    GNENetworkElement(edgeTypeParent->getNet(), "", SUMO_TAG_LANETYPE),
    NBTypeCont::LaneTypeDefinition(laneType),
    myEdgeTypeParent(edgeTypeParent) {
}


GNELaneType::~GNELaneType() {
}


GNEMoveElement*
GNELaneType::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNELaneType::getParameters() {
    return this;
}


const Parameterised*
GNELaneType::getParameters() const {
    return this;
}


GNEEdgeType*
GNELaneType::getEdgeTypeParent() const {
    return myEdgeTypeParent;
}


void
GNELaneType::copyLaneType(GNELaneType* originalLaneType, GNEUndoList* undoList) {
    // copy speed
    setAttribute(SUMO_ATTR_SPEED, originalLaneType->getAttribute(SUMO_ATTR_SPEED), undoList);
    // copy allow (and disallow)
    setAttribute(SUMO_ATTR_ALLOW, originalLaneType->getAttribute(SUMO_ATTR_ALLOW), undoList);
    // copy width
    setAttribute(SUMO_ATTR_WIDTH, originalLaneType->getAttribute(SUMO_ATTR_WIDTH), undoList);
    // copy parameters
    setAttribute(GNE_ATTR_PARAMETERS, originalLaneType->getAttribute(GNE_ATTR_PARAMETERS), undoList);
}


void
GNELaneType::updateGeometry() {
    // nothing to do
}


Position
GNELaneType::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


bool
GNELaneType::checkDrawFromContour() const {
    return false;
}


bool
GNELaneType::checkDrawToContour() const {
    return false;
}


bool
GNELaneType::checkDrawRelatedContour() const {
    return false;
}


bool
GNELaneType::checkDrawOverContour() const {
    return false;
}

bool
GNELaneType::checkDrawDeleteContour() const {
    return false;
}


bool
GNELaneType::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNELaneType::checkDrawSelectContour() const {
    return false;
}


bool
GNELaneType::checkDrawMoveContour() const {
    return false;
}


GNEMoveOperation*
GNELaneType::getMoveOperation() {
    return nullptr;
}


void
GNELaneType::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


GUIGLObjectPopupMenu*
GNELaneType::getPopUpMenu(GUIMainWindow& /*app*/, GUISUMOAbstractView& /*parent*/) {
    return nullptr;
}


Boundary
GNELaneType::getCenteringBoundary() const {
    return myNetworkElementContour.getContourBoundary();
}


void
GNELaneType::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNELaneType::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


void
GNELaneType::deleteGLObject() {
    myNet->deleteNetworkElement(this, myNet->getViewNet()->getUndoList());
}


void
GNELaneType::updateGLObject() {
    updateGeometry();
}


std::string
GNELaneType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return "lane: " + toString(myEdgeTypeParent->getLaneTypeIndex(this));
        case SUMO_ATTR_SPEED:
            if (attrs.count(key) == 0) {
                return toString(OptionsCont::getOptions().getFloat("default.speed"));
            } else {
                return toString(speed);
            }
        case SUMO_ATTR_ALLOW:
            if ((permissions == SVCAll) || (permissions == -1)) {
                return "all";
            } else if (permissions == 0) {
                return "";
            } else {
                return getVehicleClassNames(permissions);
            }
        case SUMO_ATTR_DISALLOW:
            if (permissions == 0) {
                return "all";
            } else if ((permissions == SVCAll) || (permissions == -1)) {
                return "";
            } else {
                return getVehicleClassNames(invertPermissions(permissions));
            }
        case SUMO_ATTR_WIDTH:
            if (attrs.count(key) == 0) {
                return "default";
            } else {
                return toString(width);
            }
        case SUMO_ATTR_FRICTION:
            if (attrs.count(key) == 0) {
                return "";
            } else {
                return toString(width);
            }
        default:
            return getCommonAttribute(key);
    }
}


double
GNELaneType::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNELaneType::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNELaneType::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNELaneType::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("laneType attributes cannot be edited here");
}


bool
GNELaneType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
            if (value.empty() || (value == "default")) {
                return true;
            } else {
                return canParse<double>(value) && (parse<double>(value) > 0);
            }
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            if (value.empty() || (value == "-1") || (value == "default")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        default:
            return isCommonAttributeValid(key, value);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNELaneType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
            if (value.empty() || (value == "default")) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                speed = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ALLOW:
            // parse permissions
            permissions = parseVehicleClasses(value);
            // check attrs
            if ((permissions == SVCAll) || (permissions == -1)) {
                attrs.insert(SUMO_ATTR_ALLOW);
                attrs.erase(SUMO_ATTR_DISALLOW);
            } else if (permissions == 0) {
                attrs.erase(SUMO_ATTR_ALLOW);
                attrs.insert(SUMO_ATTR_DISALLOW);
            } else {
                attrs.insert(SUMO_ATTR_ALLOW);
                attrs.insert(SUMO_ATTR_DISALLOW);
            }
            break;
        case SUMO_ATTR_DISALLOW:
            // parse invert permissions
            permissions = invertPermissions(parseVehicleClasses(value));
            // check attrs
            if ((permissions == SVCAll) || (permissions == -1)) {
                attrs.insert(SUMO_ATTR_ALLOW);
                attrs.erase(SUMO_ATTR_DISALLOW);
            } else if (permissions == 0) {
                attrs.erase(SUMO_ATTR_ALLOW);
                attrs.insert(SUMO_ATTR_DISALLOW);
            } else {
                attrs.insert(SUMO_ATTR_ALLOW);
                attrs.insert(SUMO_ATTR_DISALLOW);
            }
            break;
        case SUMO_ATTR_WIDTH:
            if (value.empty() || (value == "-1") || (value == "default")) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                width = parse<double>(value);
            }
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
    // update edge selector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getLaneTypeAttributes()->refreshAttributesEditor();
    }
}

/****************************************************************************/
