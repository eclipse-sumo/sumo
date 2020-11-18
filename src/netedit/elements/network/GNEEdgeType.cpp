/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEEdgeType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
/// A SUMO edge type file assigns default values for certain attributes to types of roads.
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/options/OptionsCont.h>

#include "GNEEdgeType.h"


// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeType::GNEEdgeType(GNENet* net, NBTypeCont::TypeDefinition *type):
    GNENetworkElement(net, "", GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}),
    myType(type) {
}


GNEEdgeType::~GNEEdgeType() {

}


void
GNEEdgeType::updateGeometry() {
}


Position
GNEEdgeType::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


GNEMoveOperation*
GNEEdgeType::getMoveOperation(const double /*shapeOffset*/) {
    return nullptr;
}


void 
GNEEdgeType::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


GUIGLObjectPopupMenu*
GNEEdgeType::getPopUpMenu(GUIMainWindow& /*app*/, GUISUMOAbstractView& /*parent*/) {
    return nullptr;
}


void 
GNEEdgeType::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNEEdgeType::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


std::string
GNEEdgeType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_NUMLANES:
            return toString(myType->numLanes);
        case SUMO_ATTR_SPEED:
            return toString(myType->speed);
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(myType->permissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(myType->permissions));
        case SUMO_ATTR_PRIORITY:
            return toString(myType->priority);
        case SUMO_ATTR_ONEWAY:
            return toString(myType->oneWay);
        case SUMO_ATTR_DISCARD:
            return toString(myType->discard);
        case SUMO_ATTR_WIDTH:
            return toString(myType->width);
        case SUMO_ATTR_WIDTHRESOLUTION:
            return toString(myType->widthResolution);
        case SUMO_ATTR_MAXWIDTH:
            return toString(myType->maxWidth);
        case SUMO_ATTR_SIDEWALKWIDTH:
            return toString(myType->sidewalkWidth);
        case SUMO_ATTR_BIKELANEWIDTH:
            return toString(myType->bikeLaneWidth);
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdgeType::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_NUMLANES:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_PRIORITY:
        case SUMO_ATTR_ONEWAY:
        case SUMO_ATTR_DISCARD:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_WIDTHRESOLUTION:
        case SUMO_ATTR_MAXWIDTH:
        case SUMO_ATTR_SIDEWALKWIDTH:
        case SUMO_ATTR_BIKELANEWIDTH:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdgeType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveEdgeType(value, false) == nullptr);
        case SUMO_ATTR_NUMLANES:
            return canParse<int>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_PRIORITY:
            return canParse<int>(value);
        case SUMO_ATTR_ONEWAY:
            return canParse<bool>(value);
        case SUMO_ATTR_DISCARD:
            return canParse<bool>(value);
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_WIDTHRESOLUTION:
        case SUMO_ATTR_MAXWIDTH:
        case SUMO_ATTR_SIDEWALKWIDTH:
        case SUMO_ATTR_BIKELANEWIDTH:
            return canParse<double>(value) && ((parse<double>(value) >= -1) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdgeType::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


const std::map<std::string, std::string>& 
GNEEdgeType::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEEdgeType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
    case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_NUMLANES:
            myType->numLanes = parse<int>(value);
            break;
        case SUMO_ATTR_SPEED:
            myType->speed = parse<double>(value);
            break;
        case SUMO_ATTR_ALLOW:
            myType->permissions = parseVehicleClasses(value);
            break;
        case SUMO_ATTR_DISALLOW:
            myType->permissions = invertPermissions(parseVehicleClasses(value));
            break;
        case SUMO_ATTR_PRIORITY:
            myType->priority = parse<int>(value);
            break;
        case SUMO_ATTR_ONEWAY:
            myType->oneWay = parse<bool>(value);
            break;
        case SUMO_ATTR_DISCARD:
            myType->discard = parse<bool>(value);
            break;
        case SUMO_ATTR_WIDTH:
            myType->width = parse<double>(value);
            break;
        case SUMO_ATTR_WIDTHRESOLUTION:
            myType->widthResolution = parse<double>(value);
            break;
        case SUMO_ATTR_MAXWIDTH:
            myType->maxWidth = parse<double>(value);
            break;
        case SUMO_ATTR_SIDEWALKWIDTH:
            myType->sidewalkWidth = parse<double>(value);
            break;
        case SUMO_ATTR_BIKELANEWIDTH:
            myType->bikeLaneWidth = parse<double>(value);
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdgeType::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEEdgeType::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
