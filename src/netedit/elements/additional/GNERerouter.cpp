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
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/elements/GNERerouterDialog.h>
#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNERerouter.h"
#include "GNERerouterSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouter::GNERerouter(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_REROUTER, ""),
    GNEAdditionalSquared(this) {
}


GNERerouter::GNERerouter(const std::string& id, GNENet* net, const std::string& filename, const Position& pos, const std::string& name,
                         double probability, bool off, bool optional, SUMOTime timeThreshold, const std::vector<std::string>& vTypes,
                         const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_REROUTER, name),
    GNEAdditionalSquared(this, pos),
    Parameterised(parameters),
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


GNEMoveElement*
GNERerouter::getMoveElement() const {
    return myMoveElementView;
}


Parameterised*
GNERerouter::getParameters() {
    return this;
}


const Parameterised*
GNERerouter::getParameters() const {
    return this;
}


void
GNERerouter::writeAdditional(OutputDevice& device) const {
    // avoid write rerouters without edges
    if (getAttribute(SUMO_ATTR_EDGES).size() > 0) {
        device.openTag(SUMO_TAG_REROUTER);
        // write common additional attributes
        writeAdditionalAttributes(device);
        // write move atributes
        myMoveElementView->writeMoveAttributes(device);
        // write specific attributes
        device.writeAttr(SUMO_ATTR_EDGES, getAttribute(SUMO_ATTR_EDGES));
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
            if (!rerouterInterval->getTagProperty()->isSymbol()) {
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


void
GNERerouter::updateGeometry() {
    updatedSquaredGeometry();
}


Position
GNERerouter::getPositionInView() const {
    return myPosOverView;
}


void
GNERerouter::updateCenteringBoundary(const bool updateGrid) {
    updatedSquaredCenteringBoundary(updateGrid);
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
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor, true);
        // draw Rerouter
        drawSquaredAdditional(s, s.additionalSettings.rerouterSize, GUITexture::REROUTER, GUITexture::REROUTER_SELECTED);
        // iterate over additionals and check if drawn
        for (const auto& interval : getChildAdditionals()) {
            // if rerouter or their intevals are selected, then draw
            if (myNet->getViewNet()->getNetworkViewOptions().showSubAdditionals() ||
                    isAttributeCarrierSelected() || inspectedElements.isACInspected(this) ||
                    interval->isAttributeCarrierSelected() || inspectedElements.isACInspected(interval) ||
                    interval->isMarkedForDrawingFront()) {
                interval->drawGL(s);
            } else {
                // if rerouterElements are inspected or selected, then draw
                for (const auto& rerouterElement : interval->getChildAdditionals()) {
                    if (rerouterElement->isAttributeCarrierSelected() || inspectedElements.isACInspected(rerouterElement) ||
                            rerouterElement->isMarkedForDrawingFront()) {
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
                if (rerouterSymbol->getTagProperty()->isSymbol()) {
                    edges.push_back(rerouterSymbol->getAttribute(SUMO_ATTR_EDGE));
                }
            }
            return toString(edges);
        }
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
        default:
            return myMoveElementView->getMovingAttribute(key);
    }
}


double
GNERerouter::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_PROB:
            return myProbability;
        default:
            return myMoveElementView->getMovingAttributeDouble(key);
    }
}


Position
GNERerouter::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePosition(key);
}


PositionVector
GNERerouter::getAttributePositionVector(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePositionVector(key);
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
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_OFF:
        case SUMO_ATTR_OPTIONAL:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementView->setMovingAttribute(key, value, undoList);
            break;
    }
}


bool
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGES:
            return canParse<std::vector<GNEEdge*> >(myNet, value, false);
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
        default:
            return myMoveElementView->isMovingAttributeValid(key, value);
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
        default:
            myMoveElementView->setMovingAttribute(key, value);
            break;
    }
    // update boundary (except for template)
    if (getID().size() > 0) {
        updateCenteringBoundary(true);
    }
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
