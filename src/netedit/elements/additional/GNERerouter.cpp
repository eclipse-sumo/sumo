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
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>

#include "GNERerouter.h"
#include "GNERerouterSymbol.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouter::GNERerouter(GNENet* net) :
    GNEAdditional("", net, GLO_REROUTER, SUMO_TAG_REROUTER, GUIIconSubSys::getIcon(GUIIcon::REROUTER), "",
{}, {}, {}, {}, {}, {}),
myProbability(0),
myOff(false),
myOptional(false),
myTimeThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNERerouter::GNERerouter(const std::string& id, GNENet* net, const Position& pos, const std::string& name,
                         double probability, bool off, bool optional, SUMOTime timeThreshold, const std::vector<std::string>& vTypes,
                         const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_REROUTER, SUMO_TAG_REROUTER, GUIIconSubSys::getIcon(GUIIcon::REROUTER), name,
{}, {}, {}, {}, {}, {}),
Parameterised(parameters),
myPosition(pos),
myProbability(probability),
myOff(off),
myOptional(optional),
myTimeThreshold(timeThreshold),
myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNERerouter::~GNERerouter() {
}


void
GNERerouter::writeAdditional(OutputDevice& device) const {
    // avoid write rerouters without edges
    if (getAttribute(SUMO_ATTR_EDGES).size() > 0) {
        device.openTag(SUMO_TAG_REROUTER);
        device.writeAttr(SUMO_ATTR_ID, getID());
        device.writeAttr(SUMO_ATTR_EDGES, getAttribute(SUMO_ATTR_EDGES));
        device.writeAttr(SUMO_ATTR_POSITION, myPosition);
        if (!myAdditionalName.empty()) {
            device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
        }
        if (myProbability != 1.0) {
            device.writeAttr(SUMO_ATTR_PROB, myProbability);
        }
        if (time2string(myTimeThreshold) != "0.00") {
            device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, time2string(myTimeThreshold));
        }
        if (!myVTypes.empty()) {
            device.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
        }
        if (myOff) {
            device.writeAttr(SUMO_ATTR_OFF, myOff);
        }
        if (myOptional) {
            device.writeAttr(SUMO_ATTR_OPTIONAL, myOptional);
        }
        // write all rerouter interval
        for (const auto& rerouterInterval : getChildAdditionals()) {
            if (!rerouterInterval->getTagProperty().isSymbol()) {
                rerouterInterval->writeAdditional(device);
            }
        }
        // write parameters (Always after children to avoid problems with additionals.xsd)
        writeParams(device);
        device.closeTag();
    } else {
        WRITE_WARNING("Rerouter '" + getID() + TL("' needs at least one edge"));
    }
}


bool
GNERerouter::isAdditionalValid() const {
    return true;
}


std::string GNERerouter::getAdditionalProblem() const {
    return "";
}


void
GNERerouter::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNERerouter::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            !myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // only move the first element
        return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
    } else {
        return false;
    }
}


GNEMoveOperation*
GNERerouter::getMoveOperation() {
    // return move operation for additional placed in view
    return new GNEMoveOperation(this, myPosition);
}


void
GNERerouter::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
    // update geometries (boundaries of all children)
    for (const auto& additionalChildren : getChildAdditionals()) {
        additionalChildren->updateGeometry();
        for (const auto& rerouterElement : additionalChildren->getChildAdditionals()) {
            rerouterElement->updateGeometry();
        }
    }
}


Position
GNERerouter::getPositionInView() const {
    return myPosition;
}


void
GNERerouter::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // now update geometry
    updateGeometry();
    // add shape boundary
    myAdditionalBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    /*
        // add positions of all childrens (intervals and symbols)
        for (const auto& additionalChildren : getChildAdditionals()) {
            myAdditionalBoundary.add(additionalChildren->getPositionInView());
            for (const auto& rerouterElement : additionalChildren->getChildAdditionals()) {
                myAdditionalBoundary.add(rerouterElement->getPositionInView());
                // special case for parking area rerouter
                if (rerouterElement->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
                    myAdditionalBoundary.add(rerouterElement->getParentAdditionals().at(1)->getCenteringBoundary());
                }
            }
        }
    */
    // grow
    myAdditionalBoundary.grow(5);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNERerouter::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNERerouter::openAdditionalDialog() {
    // Open rerouter dialog
    GNERerouterDialog(this);
}


std::string
GNERerouter::getParentName() const {
    return myNet->getMicrosimID();
}


void
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor, true);
        // draw Rerouter
        drawSquaredAdditional(s, myPosition, s.additionalSettings.rerouterSize, GUITexture::REROUTER, GUITexture::REROUTER_SELECTED);
        // iterate over additionals and check if drawn
        for (const auto& interval : getChildAdditionals()) {
            // if rerouter or their intevals are selected, then draw
            if (myNet->getViewNet()->getNetworkViewOptions().showSubAdditionals() ||
                    isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                    interval->isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(interval) ||
                    (myNet->getViewNet()->getFrontAttributeCarrier() == interval)) {
                interval->drawGL(s);
            } else {
                // if rerouterElements are inspected or selected, then draw
                for (const auto& rerouterElement : interval->getChildAdditionals()) {
                    if (rerouterElement->isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(rerouterElement) ||
                            (myNet->getViewNet()->getFrontAttributeCarrier() == rerouterElement)) {
                        interval->drawGL(s);
                    }
                }
            }
        }
    }
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edges;
            for (const auto& rerouterSymbol : getChildAdditionals()) {
                if (rerouterSymbol->getTagProperty().isSymbol()) {
                    edges.push_back(rerouterSymbol->getAttribute(SUMO_ATTR_EDGE));
                }
            }
            return toString(edges);
        }
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_VTYPES:
            return toString(myVTypes);
        case SUMO_ATTR_OFF:
            return toString(myOff);
        case SUMO_ATTR_OPTIONAL:
            return toString(myOptional);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERerouter::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_PROB:
            return myProbability;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNERerouter::getACParametersMap() const {
    return getParametersMap();
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        // special case  for lanes due rerouter Symbols
        case SUMO_ATTR_EDGES:
            // rebuild rerouter Symbols
            rebuildRerouterSymbols(value, undoList);
            break;
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_OFF:
        case SUMO_ATTR_OPTIONAL:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGES:
            return canParse<std::vector<GNEEdge*> >(myNet, value, false);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) <= 1);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_OFF:
            return canParse<bool>(value);
        case SUMO_ATTR_OPTIONAL:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERerouter::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNERerouter::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_EDGES:
            throw InvalidArgument(getTagStr() + " cannot be edited");
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(true);
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_OFF:
            myOff = parse<bool>(value);
            break;
        case SUMO_ATTR_OPTIONAL:
            myOptional = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
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
GNERerouter::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    updateGeometry();
}


void
GNERerouter::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(this, "position of " + getTagStr());
    GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
    undoList->end();
}


void
GNERerouter::rebuildRerouterSymbols(const std::string& value, GNEUndoList* undoList) {
    undoList->begin(this, ("change " + getTagStr() + " attribute").c_str());
    // drop all additional children
    while (getChildAdditionals().size() > 0) {
        undoList->add(new GNEChange_Additional(getChildAdditionals().front(), false), true);
    }
    // get edge vector
    const std::vector<GNEEdge*> edges = parse<std::vector<GNEEdge*> >(myNet, value);
    // create new VSS Symbols
    for (const auto& edge : edges) {
        // create VSS Symbol
        GNEAdditional* VSSSymbol = new GNERerouterSymbol(this, edge);
        // add it using GNEChange_Additional
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(VSSSymbol, true), true);
    }
    undoList->end();
}


/****************************************************************************/
