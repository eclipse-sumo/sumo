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
/// @file    GNEEdgeType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
/// A SUMO edge type file assigns default values for certain attributes to types of roads.
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEUndoList.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_LaneType.h>
#include <utils/options/OptionsCont.h>

#include "GNEEdgeType.h"
#include "GNELaneType.h"


// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeType::GNEEdgeType(GNECreateEdgeFrame* createEdgeFrame) :
    GNENetworkElement(createEdgeFrame->getViewNet()->getNet(), "", GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create laneType
    GNELaneType* laneType = new GNELaneType(this);
    laneType->incRef("GNEEdgeType::GNEEdgeType(Default)");
    myLaneTypes.push_back(laneType);
}


GNEEdgeType::GNEEdgeType(GNENet* net) :
    GNENetworkElement(net, net->generateEdgeTypeID(), GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create laneType
    GNELaneType* laneType = new GNELaneType(this);
    laneType->incRef("GNEEdgeType::GNEEdgeType");
    myLaneTypes.push_back(laneType);
}


GNEEdgeType::GNEEdgeType(GNENet* net, const std::string& ID, const NBTypeCont::EdgeTypeDefinition* edgeType) :
    GNENetworkElement(net, ID, GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create  laneTypes
    for (const auto& laneTypeDef : edgeType->laneTypeDefinitions) {
        GNELaneType* laneType = new GNELaneType(this, laneTypeDef);
        laneType->incRef("GNEEdgeType::GNEEdgeType(parameters)");
        myLaneTypes.push_back(laneType);
    }
    // copy parameters
    speed = edgeType->speed;
    priority = edgeType->priority;
    permissions = edgeType->permissions;
    spreadType = edgeType->spreadType;
    width = edgeType->width;
    attrs = edgeType->attrs;
    laneTypeDefinitions = edgeType->laneTypeDefinitions;
}


GNEEdgeType::~GNEEdgeType() {
    // delete laneTypes
    for (const auto& laneType : myLaneTypes) {
        laneType->decRef("GNEEdgeType::~GNEEdgeType");
        if (laneType->unreferenced()) {
            delete laneType;
        }
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
        if (position < 0 || position > (int)myLaneTypes.size()) {
            throw ProcessError("invalid position");
        } else if (position == (int)myLaneTypes.size()) {
            myLaneTypes.push_back(laneType);
        } else {
            myLaneTypes[position] = laneType;
        }
    }
}


void
GNEEdgeType::addLaneType(GNEUndoList* undoList) {
    // get options
    const OptionsCont& oc = OptionsCont::getOptions();
    // create new laneType
    GNELaneType* laneType = new GNELaneType(this);
    // begin undoList
    undoList->p_begin("add laneType");
    // add lane
    undoList->add(new GNEChange_LaneType(laneType, (int)myLaneTypes.size(), true), true);
    // set default parameters
    laneType->setAttribute(SUMO_ATTR_SPEED, toString(oc.getFloat("default.speed")), undoList);
    laneType->setAttribute(SUMO_ATTR_DISALLOW, oc.getString("default.disallow"), undoList);
    laneType->setAttribute(SUMO_ATTR_WIDTH, toString(NBEdge::UNSPECIFIED_WIDTH), undoList);
    laneType->setAttribute(GNE_ATTR_PARAMETERS, "", undoList);
    // end undoList
    undoList->p_end();
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
GNEEdgeType::removeLaneType(const int index, GNEUndoList* undoList) {
    // first check if index is correct
    if ((myLaneTypes.size() > 1) && (index < (int)myLaneTypes.size())) {
        // begin undoList
        undoList->p_begin("remove laneType");
        // copy laneType values
        for (int i = index; i < ((int)myLaneTypes.size() - 1); i++) {
            myLaneTypes.at(i)->copyLaneType(myLaneTypes.at(i + 1), undoList);
        }
        // remove last lane
        undoList->add(new GNEChange_LaneType(myLaneTypes.back(), ((int)myLaneTypes.size() - 1), false), true);
        // end undoList
        undoList->p_end();
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
    // get options
    const OptionsCont& oc = OptionsCont::getOptions();
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_NUMLANES:
            return toString(myLaneTypes.size());
        case SUMO_ATTR_SPEED:
            if (attrs.count(key) == 0) {
                return toString(oc.getFloat("default.speed"));
            } else {
                return toString(speed);
            }
        case SUMO_ATTR_ALLOW:
            if (attrs.count(SUMO_ATTR_DISALLOW) == 0) {
                return "all";
            } else {
                return getVehicleClassNames(permissions);
            }
        case SUMO_ATTR_DISALLOW:
            if (attrs.count(SUMO_ATTR_DISALLOW) == 0) {
                return "";
            } else {
                return getVehicleClassNames(invertPermissions(permissions));
            }
        case SUMO_ATTR_SPREADTYPE:
            return SUMOXMLDefinitions::LaneSpreadFunctions.getString(spreadType);
        case SUMO_ATTR_WIDTH:
            if (attrs.count(key) == 0) {
                return toString(NBEdge::UNSPECIFIED_WIDTH);
            } else {
                return toString(width);
            }
        case SUMO_ATTR_PRIORITY:
            if (attrs.count(key) == 0) {
                return toString(-1);
            } else {
                return toString(priority);
            }
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
        case SUMO_ATTR_SPREADTYPE:
        case SUMO_ATTR_DISCARD:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_PRIORITY:
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
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value) && (parse<double>(value) > 0);
            }
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            if (value.empty()) {
                return true;
            } else {
                return canParseVehicleClasses(value);
            }
        case SUMO_ATTR_SPREADTYPE:
            return SUMOXMLDefinitions::LaneSpreadFunctions.hasString(value);
        case SUMO_ATTR_WIDTH:
            if (value.empty() || (value == "-1")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_PRIORITY:
            return canParse<int>(value);
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
                attrs.erase(SUMO_ATTR_ALLOW);
            } else {
                attrs.insert(SUMO_ATTR_ALLOW);
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
        case SUMO_ATTR_SPREADTYPE:
            spreadType = SUMOXMLDefinitions::LaneSpreadFunctions.get(value);
            break;
        case SUMO_ATTR_DISCARD:
            if (value.empty()) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                discard = parse<bool>(value);
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
        case SUMO_ATTR_PRIORITY:
            if (value.empty()) {
                attrs.erase(key);
            } else {
                attrs.insert(key);
                priority = parse<int>(value);
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // update edge selector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
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
