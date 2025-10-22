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
/// @file    GNEVariableSpeedSign.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/elements/GNEVariableSpeedSignDialog.h>
#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSign::GNEVariableSpeedSign(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_VSS, ""),
    GNEAdditionalSquared(this) {
}


GNEVariableSpeedSign::GNEVariableSpeedSign(const std::string& id, GNENet* net, const std::string& filename,
        const Position& pos, const std::string& name, const std::vector<std::string>& vTypes,
        const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_VSS, name),
    GNEAdditionalSquared(this, pos),
    Parameterised(parameters),
    myVehicleTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEVariableSpeedSign::~GNEVariableSpeedSign() {
}


GNEMoveElement*
GNEVariableSpeedSign::getMoveElement() const {
    return myMoveElementView;
}


Parameterised*
GNEVariableSpeedSign::getParameters() {
    return this;
}


const Parameterised*
GNEVariableSpeedSign::getParameters() const {
    return this;
}


void
GNEVariableSpeedSign::writeAdditional(OutputDevice& device) const {
    // avoid write rerouters without edges
    if (getAttribute(SUMO_ATTR_LANES).size() > 0) {
        device.openTag(SUMO_TAG_VSS);
        // write common additional attributes
        writeAdditionalAttributes(device);
        // write move atributes
        myMoveElementView->writeMoveAttributes(device);
        // write specific attributes
        device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
        if (!myVehicleTypes.empty()) {
            device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
        }
        // write all rerouter interval
        for (const auto& step : getChildAdditionals()) {
            if (!step->getTagProperty()->isSymbol()) {
                step->writeAdditional(device);
            }
        }
        // write parameters (Always after children to avoid problems with additionals.xsd)
        writeParams(device);
        device.closeTag();
    } else {
        WRITE_WARNING("Variable Speed Sign '" + getID() + TL("' needs at least one lane"));
    }
}


bool
GNEVariableSpeedSign::GNEVariableSpeedSign::isAdditionalValid() const {
    return true;
}


std::string
GNEVariableSpeedSign::GNEVariableSpeedSign::getAdditionalProblem() const {
    return "";
}


void
GNEVariableSpeedSign::GNEVariableSpeedSign::fixAdditionalProblem() {
    // nothing to fix
}


void
GNEVariableSpeedSign::updateGeometry() {
    updatedSquaredGeometry();
}


Position
GNEVariableSpeedSign::getPositionInView() const {
    return myPosOverView;
}


void
GNEVariableSpeedSign::updateCenteringBoundary(const bool updateGrid) {
    updatedSquaredCenteringBoundary(updateGrid);
}


void
GNEVariableSpeedSign::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


bool
GNEVariableSpeedSign::checkDrawMoveContour() const {
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
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor, true);
        // draw VSS
        drawSquaredAdditional(s, s.additionalSettings.VSSSize, GUITexture::VARIABLESPEEDSIGN, GUITexture::VARIABLESPEEDSIGN_SELECTED);
        // iterate over additionals and check if drawn
        for (const auto& step : getChildAdditionals()) {
            // if rerouter or their intevals are selected, then draw
            if (myNet->getViewNet()->getNetworkViewOptions().showSubAdditionals() ||
                    isAttributeCarrierSelected() || inspectedElements.isACInspected(this) ||
                    step->isAttributeCarrierSelected() || inspectedElements.isACInspected(step) ||
                    step->isMarkedForDrawingFront()) {
                step->drawGL(s);
            }
        }
    }
}


std::string
GNEVariableSpeedSign::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANES: {
            std::vector<std::string> lanes;
            for (const auto& VSSSymbol : getChildAdditionals()) {
                if (VSSSymbol->getTagProperty()->isSymbol()) {
                    lanes.push_back(VSSSymbol->getAttribute(SUMO_ATTR_LANE));
                }
            }
            return toString(lanes);
        }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
        default:
            return myMoveElementView->getMovingAttribute(key);
    }
}


double
GNEVariableSpeedSign::getAttributeDouble(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributeDouble(key);
}


Position
GNEVariableSpeedSign::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePosition(key);
}


PositionVector
GNEVariableSpeedSign::getAttributePositionVector(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePositionVector(key);
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
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_VTYPES:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementView->setMovingAttribute(key, value, undoList);
            break;
    }
}


bool
GNEVariableSpeedSign::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
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
        default:
            return myMoveElementView->isMovingAttributeValid(key, value);
    }
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
            setAdditionalID(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
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
GNEVariableSpeedSign::rebuildVSSSymbols(const std::string& value, GNEUndoList* undoList) {
    undoList->begin(this, ("change " + getTagStr() + " attribute").c_str());
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
