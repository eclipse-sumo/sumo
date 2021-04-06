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
/// @file    GNELaneType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
/// A SUMO lane type file assigns default values for certain attributes to types of roads.
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/options/OptionsCont.h>

#include "GNELaneType.h"
#include "GNEEdgeType.h"


// ===========================================================================
// members methods
// ===========================================================================

GNELaneType::GNELaneType(GNEEdgeType* edgeTypeParent):
    GNENetworkElement(edgeTypeParent->getNet(), "", GLO_LANE, SUMO_TAG_LANETYPE, {}, {}, {}, {}, {}, {}, {}, {}),
myEdgeTypeParent(edgeTypeParent) {
}


GNELaneType::GNELaneType(GNEEdgeType* edgeTypeParent, const NBTypeCont::LaneTypeDefinition& laneType):
    GNENetworkElement(edgeTypeParent->getNet(), "", GLO_LANE, SUMO_TAG_LANETYPE, {}, {}, {}, {}, {}, {}, {}, {}),
myEdgeTypeParent(edgeTypeParent) {
    // copy parameters
    speed = laneType.speed;
    permissions = laneType.permissions;
    width = laneType.width;
    attrs = laneType.attrs;
}


GNELaneType::~GNELaneType() {
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


GNEMoveOperation*
GNELaneType::getMoveOperation(const double /*shapeOffset*/) {
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


void
GNELaneType::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNELaneType::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


std::string
GNELaneType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myEdgeTypeParent->getID() + toString(myEdgeTypeParent->getLaneTypeIndex(this));
        case SUMO_ATTR_SPEED:
            if (attrs.count(key) == 0) {
                return "";
            } else {
                return toString(speed);
            }
        case SUMO_ATTR_ALLOW:
            if (attrs.count(SUMO_ATTR_DISALLOW) == 0) {
                return "";
            } else {
                return getVehicleClassNames(permissions);
            }
        case SUMO_ATTR_DISALLOW:
            if (attrs.count(SUMO_ATTR_DISALLOW) == 0) {
                return "";
            } else {
                return getVehicleClassNames(invertPermissions(permissions));
            }
        case SUMO_ATTR_WIDTH:
            if (attrs.count(key) == 0) {
                return "";
            } else {
                return toString(width);
            }
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNELaneType::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_WIDTH:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELaneType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && ((parse<double>(value) >= -1) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELaneType::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


const std::map<std::string, std::string>&
GNELaneType::getACParametersMap() const {
    return getParametersMap();
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
            if (value.empty()) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                speed = parse<double>(value);
            }
            break;
        case SUMO_ATTR_ALLOW:
            if (value.empty()) {
                attrs.erase(SUMO_ATTR_DISALLOW);
            } else {
                attrs.insert(SUMO_ATTR_DISALLOW);
                permissions = parseVehicleClasses(value);
            }
            break;
        case SUMO_ATTR_DISALLOW:
            if (value.empty()) {
                attrs.erase(SUMO_ATTR_DISALLOW);
            } else {
                attrs.insert(SUMO_ATTR_DISALLOW);
                permissions = invertPermissions(parseVehicleClasses(value));
            }
            break;
        case SUMO_ATTR_WIDTH:
            if (value.empty()) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                width = parse<double>(value);
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNELaneType::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNELaneType::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
