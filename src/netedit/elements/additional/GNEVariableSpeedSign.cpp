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
/// @file    GNEVariableSpeedSign.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNEVariableSpeedSignDialog.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>

#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignSymbol.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSign::GNEVariableSpeedSign(const std::string& id, GNENet* net, const Position& pos, const std::string& name,
        const std::vector<std::string>& vTypes, const std::map<std::string, std::string>& parameters, bool blockMovement) :
    GNEAdditional(id, net, GLO_VSS, SUMO_TAG_VSS, name,
{}, {}, {}, {}, {}, {}, {}, {},
parameters, blockMovement),
            myPosition(pos),
myVehicleTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEVariableSpeedSign::~GNEVariableSpeedSign() {
}


GNEMoveOperation*
GNEVariableSpeedSign::getMoveOperation(const double /*shapeOffset*/) {
    if (myBlockMovement) {
        // element blocked, then nothing to move
        return nullptr;
    } else {
        // return move operation for additional placed in view
        return new GNEMoveOperation(this, myPosition);
    }
}


void
GNEVariableSpeedSign::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
    // Update Hierarchical connections geometry
    myHierarchicalConnections.update();
}


Position
GNEVariableSpeedSign::getPositionInView() const {
    return myPosition;
}


void
GNEVariableSpeedSign::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // update geometry
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
GNEVariableSpeedSign::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEVariableSpeedSign::openAdditionalDialog() {
    // Open VSS dialog
    GNEVariableSpeedSignDialog(this);
}


std::string
GNEVariableSpeedSign::getParentName() const {
    return myNet->getMicrosimID();
}


void
GNEVariableSpeedSign::drawGL(const GUIVisualizationSettings& s) const {
    // draw VSS
    drawSquaredAdditional(s, myPosition, s.additionalSettings.VSSSize, GUITexture::VARIABLESPEEDSIGN, GUITexture::VARIABLESPEEDSIGN_SELECTED);
}


std::string
GNEVariableSpeedSign::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANES: {
            std::vector<std::string> lanes;
            for (const auto& VSSSymbol : getChildAdditionals()) {
                if (VSSSymbol->getTagProperty().isSymbol()) {
                    lanes.push_back(VSSSymbol->getAttribute(SUMO_ATTR_LANE));
                }
            }
            return toString(lanes);
        }
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
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
GNEVariableSpeedSign::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEVariableSpeedSign::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        // special case  for lanes due VSS Symbols
        case SUMO_ATTR_LANES:
            // rebuild VSS Symbols
            rebuildVSSSymbols(value, undoList);
            break;
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_VTYPES:
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
GNEVariableSpeedSign::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_LANES:
            return canParse<std::vector<GNELane*> >(myNet, value, false);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
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
GNEVariableSpeedSign::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEVariableSpeedSign::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEVariableSpeedSign::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVariableSpeedSign::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANES:
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
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
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
GNEVariableSpeedSign::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    updateGeometry();
}


void
GNEVariableSpeedSign::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->p_end();
}


void
GNEVariableSpeedSign::rebuildVSSSymbols(const std::string& value, GNEUndoList* undoList) {
    undoList->p_begin(("change " + getTagStr() + " attribute").c_str());
    // drop all additional children
    while (getChildAdditionals().size() > 0) {
        undoList->add(new GNEChange_Additional(getChildAdditionals().front(), false), true);
    }
    // get lane vector
    const std::vector<GNELane*> lanes = parse<std::vector<GNELane*> >(myNet, value);
    // create new VSS Symbols
    for (const auto& lane : lanes) {
        // create VSS Symbol
        GNEAdditional* VSSSymbol = new GNEVariableSpeedSignSymbol(this, lane);
        // add it using GNEChange_Additional
        myNet->getViewNet()->getUndoList()->add(new GNEChange_Additional(VSSSymbol, true), true);
    }
    undoList->p_end();
}

/****************************************************************************/
