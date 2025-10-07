/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2021-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>

#include "GNETractionSubstation.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETractionSubstation::GNETractionSubstation(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_TRACTION_SUBSTATION, ""),
    myMoveElementView(new GNEMoveElementView(this)) {
}


GNETractionSubstation::GNETractionSubstation(const std::string& id, GNENet* net, const std::string& filename, const Position& pos,
        const double voltage, const double currentLimit, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_TRACTION_SUBSTATION, ""),
    Parameterised(parameters),
    myMoveElementView(new GNEMoveElementView(this, GNEMoveElementView::AttributesFormat::POSITION, pos)),
    myVoltage(voltage),
    myCurrentLimit(currentLimit) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNETractionSubstation::~GNETractionSubstation() {
    delete myMoveElementView;
}


GNEMoveElement*
GNETractionSubstation::getMoveElement() const {
    return myMoveElementView;
}


void
GNETractionSubstation::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_TRACTION_SUBSTATION);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move atributes
    myMoveElementView->writeMoveAttributes(device);
    // write specific attributes
    if (myVoltage != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_VOLTAGE)) {
        device.writeAttr(SUMO_ATTR_VOLTAGE, myVoltage);
    }
    if (myCurrentLimit != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_CURRENTLIMIT)) {
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


bool
GNETractionSubstation::checkDrawMoveContour() const {
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
GNETractionSubstation::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myMoveElementView->myPosOverView, 0);
}


Position
GNETractionSubstation::getPositionInView() const {
    return myMoveElementView->myPosOverView;
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
    myAdditionalBoundary.grow(5);
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
    drawSquaredAdditional(s, myMoveElementView->myPosOverView, s.additionalSettings.tractionSubstationSize, GUITexture::TRACTIONSUBSTATION, GUITexture::TRACTIONSUBSTATION_SELECTED);
}


std::string
GNETractionSubstation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edges;
            for (const auto& tractionSubstationSymbol : getChildAdditionals()) {
                if (tractionSubstationSymbol->getTagProperty()->isSymbol()) {
                    edges.push_back(tractionSubstationSymbol->getAttribute(SUMO_ATTR_EDGE));
                }
            }
            return toString(edges);
        }
        case SUMO_ATTR_POSITION:
            return toString(myMoveElementView->myPosOverView);
        case SUMO_ATTR_VOLTAGE:
            return toString(myVoltage);
        case SUMO_ATTR_CURRENTLIMIT:
            return toString(myCurrentLimit);
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNETractionSubstation::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


const
Parameterised::Map& GNETractionSubstation::getACParametersMap() const {
    return getParametersMap();
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
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
        default:
            return isCommonValid(key, value);
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
            setAdditionalID(value);
            break;
        case SUMO_ATTR_POSITION:
            myMoveElementView->myPosOverView = parse<Position>(value);
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
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}

/****************************************************************************/
