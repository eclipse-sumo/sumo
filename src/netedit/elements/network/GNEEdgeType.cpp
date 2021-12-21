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
#include <utils/options/OptionsCont.h>

#include "GNEEdgeType.h"
#include "GNELaneType.h"
#include "GNEEdgeTemplate.h"
#include "GNELaneTemplate.h"


// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeType::GNEEdgeType(GNECreateEdgeFrame* createEdgeFrame) :
    GNENetworkElement(createEdgeFrame->getViewNet()->getNet(), "", GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create laneType
    myLaneTypes.push_back(new GNELaneType(this));
}


GNEEdgeType::GNEEdgeType(const GNEEdgeType* edgeType) :
    GNENetworkElement(edgeType->getNet(), edgeType->getID(), GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}),
    Parameterised(edgeType->getParametersMap()),
    NBTypeCont::EdgeTypeDefinition(edgeType) {
}


GNEEdgeType::GNEEdgeType(GNENet* net) :
    GNENetworkElement(net, net->getAttributeCarriers()->generateEdgeTypeID(), GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create laneType
    GNELaneType* laneType = new GNELaneType(this);
    myLaneTypes.push_back(laneType);
}


GNEEdgeType::GNEEdgeType(GNENet* net, const std::string& ID, const NBTypeCont::EdgeTypeDefinition* edgeType) :
    GNENetworkElement(net, ID, GLO_EDGE, SUMO_TAG_TYPE, {}, {}, {}, {}, {}, {}, {}, {}) {
    // create  laneTypes
    for (const auto& laneTypeDef : edgeType->laneTypeDefinitions) {
        GNELaneType* laneType = new GNELaneType(this, laneTypeDef);
        myLaneTypes.push_back(laneType);
    }
    // copy parameters
    speed = edgeType->speed;
    priority = edgeType->priority;
    permissions = edgeType->permissions;
    spreadType = edgeType->spreadType;
    oneWay = edgeType->oneWay;
    discard = edgeType->discard;
    width = edgeType->width;
    widthResolution = edgeType->widthResolution;
    maxWidth = edgeType->maxWidth;
    minWidth = edgeType->minWidth;
    sidewalkWidth = edgeType->sidewalkWidth;
    bikeLaneWidth = edgeType->bikeLaneWidth;
    restrictions = edgeType->restrictions;
    attrs = edgeType->attrs;
    laneTypeDefinitions = edgeType->laneTypeDefinitions;
}


void
GNEEdgeType::copyTemplate(const GNEEdgeTemplate *edgeTemplate) {
    // copy all edge attributes
    setAttribute(SUMO_ATTR_NUMLANES, edgeTemplate->getAttribute(SUMO_ATTR_NUMLANES));
    setAttribute(SUMO_ATTR_SPEED, edgeTemplate->getAttribute(SUMO_ATTR_SPEED));
    setAttribute(SUMO_ATTR_ALLOW, edgeTemplate->getAttribute(SUMO_ATTR_ALLOW));
    setAttribute(SUMO_ATTR_DISALLOW, edgeTemplate->getAttribute(SUMO_ATTR_DISALLOW));
    setAttribute(SUMO_ATTR_SPREADTYPE, edgeTemplate->getAttribute(SUMO_ATTR_SPREADTYPE));
    if (canParse<double>(edgeTemplate->getAttribute(SUMO_ATTR_WIDTH))) {
        setAttribute(SUMO_ATTR_WIDTH, edgeTemplate->getAttribute(SUMO_ATTR_WIDTH));
    }
    setAttribute(SUMO_ATTR_PRIORITY, edgeTemplate->getAttribute(SUMO_ATTR_PRIORITY));
    setAttribute(GNE_ATTR_PARAMETERS, edgeTemplate->getAttribute(GNE_ATTR_PARAMETERS));
    // copy lane attributes
    for (int i = 0; i < (int)edgeTemplate->getLaneTemplates().size(); i++) {
        if (canParse<double>(edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_SPEED))) {
            myLaneTypes.at(i)->setAttribute(SUMO_ATTR_SPEED, edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_SPEED));
        }
        myLaneTypes.at(i)->setAttribute(SUMO_ATTR_ALLOW, edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_ALLOW));
        myLaneTypes.at(i)->setAttribute(SUMO_ATTR_DISALLOW, edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_DISALLOW));
        if (canParse<double>(edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_WIDTH))) {
            myLaneTypes.at(i)->setAttribute(SUMO_ATTR_WIDTH, edgeTemplate->getLaneTemplates().at(i)->getAttribute(SUMO_ATTR_WIDTH));
        }
        myLaneTypes.at(i)->setAttribute(GNE_ATTR_PARAMETERS, edgeTemplate->getLaneTemplates().at(i)->getAttribute(GNE_ATTR_PARAMETERS));
    }
}


GNEEdgeType::~GNEEdgeType() {
    // delete laneTypes
    for (const auto& laneType : myLaneTypes) {
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
GNEEdgeType::addLaneType(GNELaneType* laneType) {
    myLaneTypes.push_back(laneType);
}


void
GNEEdgeType::removeLaneType(const int index) {
    if (index < (int)myLaneTypes.size()) {
        myLaneTypes.erase(myLaneTypes.begin() + index);
    } else {
        throw ProcessError("Invalid index");
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
GNEEdgeType::getMoveOperation() {
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


double
GNEEdgeType::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
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
            if ((permissions == SVCAll) || (permissions == -1)) {
                return "all";
            } else if  (permissions == 0) {
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
        // non editable attributes
        case SUMO_ATTR_ONEWAY:
            return toString(oneWay);
        case SUMO_ATTR_DISCARD:
            return toString(discard);
        case SUMO_ATTR_WIDTHRESOLUTION:
            return toString(widthResolution);
        case SUMO_ATTR_MAXWIDTH:
            return toString(maxWidth);
        case SUMO_ATTR_MINWIDTH:
            return toString(minWidth);
        case SUMO_ATTR_SIDEWALKWIDTH:
            return toString(sidewalkWidth);
        case SUMO_ATTR_BIKELANEWIDTH:
            return toString(bikeLaneWidth);
        // parameters
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdgeType::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("EdgeType attributes cannot be edited here");
}


bool
GNEEdgeType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveEdgeType(value, false) == nullptr);
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


bool
GNEEdgeType::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
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
        case SUMO_ATTR_ID: {
            // update comboBox
            myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->updateIDinComboBox(getID(), value);
            // update ID
            myNet->getAttributeCarriers()->updateEdgeTypeID(this, value);
            break;
        }
        case SUMO_ATTR_NUMLANES: {
            const int numLanes = parse<int>(value);
            // add new lanes
            while (numLanes > (int)myLaneTypes.size()) {
                myLaneTypes.push_back(new GNELaneType(this));
            }
            // remove extra lanes
            while (numLanes < (int)myLaneTypes.size()) {
                delete myLaneTypes.back();
                myLaneTypes.pop_back();
            }
            break;
        }
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
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
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeAttributes()->refreshAttributesCreator();
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getLaneTypeSelector()->refreshLaneTypeSelector();
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
