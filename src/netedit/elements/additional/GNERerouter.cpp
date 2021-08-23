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

GNERerouter::GNERerouter(const std::string& id, GNENet* net, const Position& pos, const std::string& name,
                         const std::string& filename, double probability, bool off, SUMOTime timeThreshold, const std::vector<std::string>& vTypes,
                         const std::map<std::string, std::string>& parameters, bool blockMovement) :
    GNEAdditional(id, net, GLO_REROUTER, SUMO_TAG_REROUTER, name,
{}, {}, {}, {}, {}, {}, {}, {},
parameters, blockMovement),
            myPosition(pos),
            myFilename(filename),
            myProbability(probability),
            myOff(off),
            myTimeThreshold(timeThreshold),
myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNERerouter::~GNERerouter() {
}


GNEMoveOperation*
GNERerouter::getMoveOperation(const double /*shapeOffset*/) {
    if (myBlockMovement) {
        // element blocked, then nothing to move
        return nullptr;
    } else {
        // return move operation for additional placed in view
        return new GNEMoveOperation(this, myPosition);
    }
}


void
GNERerouter::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
    // Update Hierarchical connections geometry
    myHierarchicalConnections.update();
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
    myBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    // add positions of all childrens
    for (const auto& additionalChildren : getChildAdditionals()) {
        if (additionalChildren->getTagProperty().isSymbol()) {
            myBoundary.add(additionalChildren->getPositionInView());
        }
    }
    // grow
    myBoundary.grow(10);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
    // Update Hierarchical connections geometry
    myHierarchicalConnections.update();
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
    // draw Rerouter
    drawSquaredAdditional(s, myPosition, s.additionalSettings.rerouterSize, GUITexture::REROUTER, GUITexture::REROUTER_SELECTED);
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
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
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_VTYPES:
            return toString(myVTypes);
        case SUMO_ATTR_OFF:
            return toString(myOff);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
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
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_OFF:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
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
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouter::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            // update boundary
            updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
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
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->p_end();
}


void
GNERerouter::rebuildRerouterSymbols(const std::string& value, GNEUndoList* undoList) {
    undoList->p_begin(("change " + getTagStr() + " attribute").c_str());
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
    undoList->p_end();
}


/****************************************************************************/
