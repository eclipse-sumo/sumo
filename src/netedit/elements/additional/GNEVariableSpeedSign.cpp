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

GNEVariableSpeedSign::GNEVariableSpeedSign(GNENet* net) :
    GNEAdditional("", net, GLO_VSS, SUMO_TAG_VSS, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()) {
    // reset default values
    resetDefaultValues();
}


GNEVariableSpeedSign::GNEVariableSpeedSign(const std::string& id, GNENet* net, const Position& pos, const std::string& name,
        const std::vector<std::string>& vTypes, const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_VSS, SUMO_TAG_VSS, name,
        {}, {}, {}, {}, {}, {}, {}, {},
    parameters),
    myPosition(pos),
    myVehicleTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEVariableSpeedSign::~GNEVariableSpeedSign() {
}


void
GNEVariableSpeedSign::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_VSS);
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
    device.writeAttr(SUMO_ATTR_POSITION, myPosition);
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    if (!myVehicleTypes.empty()) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
    }
    // write all rerouter interval
    for (const auto& step : getChildAdditionals()) {
        if (!step->getTagProperty().isSymbol()) {
            step->writeAdditional(device);
        }
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


GNEMoveOperation*
GNEVariableSpeedSign::getMoveOperation() {
    // return move operation for additional placed in view
    return new GNEMoveOperation(this, myPosition);
}


void
GNEVariableSpeedSign::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
    // update geometries (boundaries of all children)
    for (const auto& additionalChildren : getChildAdditionals()) {
        additionalChildren->updateGeometry();
    }
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
    myAdditionalBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    // add positions of all childrens (symbols and steps)
    for (const auto& additionalChildren : getChildAdditionals()) {
        myAdditionalBoundary.add(additionalChildren->getPositionInView());
        // also update centering boundary
        additionalChildren->updateCenteringBoundary(false);
    }
    // grow
    myAdditionalBoundary.grow(10);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
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
    // draw parent and child lines
    drawParentChildLines(s, s.additionalSettings.connectionColor, true);
    // draw VSS
    drawSquaredAdditional(s, myPosition, s.additionalSettings.VSSSize, GUITexture::VARIABLESPEEDSIGN, GUITexture::VARIABLESPEEDSIGN_SELECTED);
    // iterate over additionals and check if drawn
    for (const auto& step : getChildAdditionals()) {
        // if rerouter or their intevals are selected, then draw
        if (myNet->getViewNet()->getNetworkViewOptions().showSubAdditionals() ||
                isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                step->isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(step) ||
                (myNet->getViewNet()->getFrontAttributeCarrier() == step)) {
            step->drawGL(s);
        }
    }
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
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
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
            // update microsimID
            setMicrosimID(value);
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
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
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
    undoList->begin(GUIIcon::VARIABLESPEEDSIGN, "position of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->end();
}


void
GNEVariableSpeedSign::rebuildVSSSymbols(const std::string& value, GNEUndoList* undoList) {
    undoList->begin(GUIIcon::VARIABLESPEEDSIGN, ("change " + getTagStr() + " attribute").c_str());
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
    undoList->end();
}

/****************************************************************************/
