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
#include "GNELaneType.h"


// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeType::GNEEdgeType(GNENet* net):
    GNENetworkElement(net, "", GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
}


GNEEdgeType::~GNEEdgeType() {
    // Drop LaneTypes
    for (const auto& laneType : myLaneTypes) {
        laneType->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + laneType->getTagStr() + " '" + laneType->getID() + "' in GNEEdgeType destructor");
        delete laneType;
    }
}


const std::vector<GNELaneType*>&
GNEEdgeType::getLaneTypes() const {
    return myLaneTypes;
}


int 
GNEEdgeType::getLaneTypeIndex(const GNELaneType* laneType) const {
    for (int i = 0; i < (int)myLaneTypes.size(); i++) {
        if (myLaneTypes.at(i) == laneType) {
            return i;
        }
    }
    return (int)myLaneTypes.size();
}


void 
GNEEdgeType::addLaneType(GNELaneType* laneType, const int position) {
    if (std::find(myLaneTypes.begin(), myLaneTypes.end(), laneType) != myLaneTypes.end()) {
        throw ProcessError("GNELaneType already inserted");
    } else {
        if (position < 0 || position > myLaneTypes.size()) {
            throw ProcessError("invalid position");
        } else if (position == myLaneTypes.size()) {
            myLaneTypes.push_back(laneType);
        } else {
            myLaneTypes.insert(myLaneTypes.begin() + position, laneType);
        }
    }
}


void 
GNEEdgeType::removeLaneType(GNELaneType* laneType) {
    auto it = std::find(myLaneTypes.begin(), myLaneTypes.end(), laneType);
    if (it == myLaneTypes.end()) {
        throw ProcessError("GNELaneType wasn't inserted");
    } else {
        myLaneTypes.erase(it);
    }
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
            return toString(numLanes);
        case SUMO_ATTR_SPEED:
            return toString(speed);
        case SUMO_ATTR_ALLOW:
            return getVehicleClassNames(permissions);
        case SUMO_ATTR_DISALLOW:
            return getVehicleClassNames(invertPermissions(permissions));
        case SUMO_ATTR_PRIORITY:
            return toString(priority);
        case SUMO_ATTR_ONEWAY:
            return toString(oneWay);
        case SUMO_ATTR_DISCARD:
            return toString(discard);
        case SUMO_ATTR_WIDTH:
            return toString(width);
        case SUMO_ATTR_WIDTHRESOLUTION:
            return toString(widthResolution);
        case SUMO_ATTR_MAXWIDTH:
            return toString(maxWidth);
        case SUMO_ATTR_SIDEWALKWIDTH:
            return toString(sidewalkWidth);
        case SUMO_ATTR_BIKELANEWIDTH:
            return toString(bikeLaneWidth);
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
            numLanes = parse<int>(value);
            break;
        case SUMO_ATTR_SPEED:
            speed = parse<double>(value);
            break;
        case SUMO_ATTR_ALLOW:
            permissions = parseVehicleClasses(value);
            break;
        case SUMO_ATTR_DISALLOW:
            permissions = invertPermissions(parseVehicleClasses(value));
            break;
        case SUMO_ATTR_PRIORITY:
            priority = parse<int>(value);
            break;
        case SUMO_ATTR_ONEWAY:
            oneWay = parse<bool>(value);
            break;
        case SUMO_ATTR_DISCARD:
            discard = parse<bool>(value);
            break;
        case SUMO_ATTR_WIDTH:
            width = parse<double>(value);
            break;
        case SUMO_ATTR_WIDTHRESOLUTION:
            widthResolution = parse<double>(value);
            break;
        case SUMO_ATTR_MAXWIDTH:
            maxWidth = parse<double>(value);
            break;
        case SUMO_ATTR_SIDEWALKWIDTH:
            sidewalkWidth = parse<double>(value);
            break;
        case SUMO_ATTR_BIKELANEWIDTH:
            bikeLaneWidth = parse<double>(value);
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
