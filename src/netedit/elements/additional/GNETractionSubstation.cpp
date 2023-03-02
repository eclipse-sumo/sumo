/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2021-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNETractionSubstation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNETractionSubstation.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETractionSubstation::GNETractionSubstation(GNENet* net) :
    GNEAdditional("", net, GLO_TRACTIONSUBSTATION, SUMO_TAG_TRACTION_SUBSTATION,
                  GUIIconSubSys::getIcon(GUIIcon::TRACTION_SUBSTATION), "", {}, {}, {}, {}, {}, {}),
                            myVoltage(0),
myCurrentLimit(0) {
    // reset default values
    resetDefaultValues();
}


GNETractionSubstation::GNETractionSubstation(const std::string& id, GNENet* net, const Position& pos, const double voltage,
        const double currentLimit, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_TRACTIONSUBSTATION, SUMO_TAG_TRACTION_SUBSTATION,
                  GUIIconSubSys::getIcon(GUIIcon::TRACTION_SUBSTATION), "", {}, {}, {}, {}, {}, {}),
Parameterised(parameters),
myPosition(pos),
myVoltage(voltage),
myCurrentLimit(currentLimit) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNETractionSubstation::~GNETractionSubstation() {
}


void
GNETractionSubstation::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_TRACTION_SUBSTATION);
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition);
    if (getAttribute(SUMO_ATTR_VOLTAGE) != myTagProperty.getDefaultValue(SUMO_ATTR_VOLTAGE)) {
        device.writeAttr(SUMO_ATTR_VOLTAGE, myVoltage);
    }
    if (getAttribute(SUMO_ATTR_CURRENTLIMIT) != myTagProperty.getDefaultValue(SUMO_ATTR_CURRENTLIMIT)) {
        device.writeAttr(SUMO_ATTR_CURRENTLIMIT, myCurrentLimit);
    }
    // write parameters
    writeParams(device);
    device.closeTag();
}


bool
GNETractionSubstation::isAdditionalValid() const {
    return true;
}


std::string
GNETractionSubstation::getAdditionalProblem() const {
    return "";
}


void
GNETractionSubstation::fixAdditionalProblem() {
    // nothing to fix
}


GNEMoveOperation*
GNETractionSubstation::getMoveOperation() {
    // return move operation for additional placed in view
    return new GNEMoveOperation(this, myPosition);
}


void
GNETractionSubstation::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
}


Position
GNETractionSubstation::getPositionInView() const {
    return myPosition;
}


void
GNETractionSubstation::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // now update geometry
    updateGeometry();
    // add shape boundary
    myAdditionalBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    // grow
    myAdditionalBoundary.grow(10);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNETractionSubstation::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNETractionSubstation::getParentName() const {
    return myNet->getMicrosimID();
}


void
GNETractionSubstation::drawGL(const GUIVisualizationSettings& s) const {
    // draw parent and child lines
    drawParentChildLines(s, s.additionalSettings.connectionColor, true);
    // draw TractionSubstation
    drawSquaredAdditional(s, myPosition, s.additionalSettings.tractionSubstationSize, GUITexture::TRACTIONSUBSTATION, GUITexture::TRACTIONSUBSTATION_SELECTED);
}


std::string
GNETractionSubstation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edges;
            for (const auto& tractionSubstationSymbol : getChildAdditionals()) {
                if (tractionSubstationSymbol->getTagProperty().isSymbol()) {
                    edges.push_back(tractionSubstationSymbol->getAttribute(SUMO_ATTR_EDGE));
                }
            }
            return toString(edges);
        }
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_VOLTAGE:
            return toString(myVoltage);
        case SUMO_ATTR_CURRENTLIMIT:
            return toString(myCurrentLimit);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNETractionSubstation::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


const
Parameterised::Map& GNETractionSubstation::getACParametersMap() const {
    return PARAMETERS_EMPTY;
}


void
GNETractionSubstation::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        // special case  for lanes due tractionSubstation Symbols
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_VOLTAGE:
        case SUMO_ATTR_CURRENTLIMIT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETractionSubstation::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGES:
            return canParse<std::vector<GNEEdge*> >(myNet, value, false);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_VOLTAGE:
        case SUMO_ATTR_CURRENTLIMIT:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNETractionSubstation::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNETractionSubstation::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNETractionSubstation::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_EDGES:
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
        case SUMO_ATTR_VOLTAGE:
            myVoltage = parse<double>(value);
            break;
        case SUMO_ATTR_CURRENTLIMIT:
            myCurrentLimit = parse<double>(value);
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
GNETractionSubstation::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    updateGeometry();
}


void
GNETractionSubstation::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(GUIIcon::TRACTION_SUBSTATION, "position of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->end();
}

/****************************************************************************/
