/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrictionCoefficientSymbol.cpp
/// @author  Pablo Alvarez Lopez
/// @author  Thomas Weber
/// @date    Jul 2020
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEFrictionCoefficientSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEFrictionCoefficientSymbol::GNEFrictionCoefficientSymbol(GNEAdditional* COFParent, GNELane* lane) :
    GNEAdditional(COFParent->getNet(), GLO_COF, GNE_TAG_COF_SYMBOL, "",
{}, {}, {lane}, {COFParent}, {}, {}, {}, {},
std::map<std::string, std::string>()) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEFrictionCoefficientSymbol::~GNEFrictionCoefficientSymbol() {
}


GNEMoveOperation*
GNEFrictionCoefficientSymbol::getMoveOperation() {
    // GNEFrictionCoefficientSymbols cannot be moved
    return nullptr;
}


void GNEFrictionCoefficientSymbol::writeAdditional(OutputDevice& /*device*/) const {
    // nothing to write
}


void
GNEFrictionCoefficientSymbol::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), 1.5, 0);
}


void
GNEFrictionCoefficientSymbol::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    // add center
    myAdditionalBoundary.add(getPositionInView());
    // grow
    myAdditionalBoundary.grow(10);
}


Position
GNEFrictionCoefficientSymbol::getPositionInView() const {
    return myAdditionalGeometry.getShape().getCentroid();
}


void
GNEFrictionCoefficientSymbol::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/,
        const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to split
}


std::string
GNEFrictionCoefficientSymbol::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEFrictionCoefficientSymbol::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double COFExaggeration = s.addSize.getExaggeration(s, getParentAdditionals().front());
    // first check if additional has to be drawn
    if (s.drawAdditionals(COFExaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            (myAdditionalGeometry.getShape().size() > 0) && (myAdditionalGeometry.getShapeRotations().size() > 0)) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor);
        // Start drawing adding an gl identificator (except in Move mode)
        if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE) {
            GLHelper::pushName(getParentAdditionals().front()->getGlID());
        }
        // start drawing symbol
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(getParentAdditionals().front(), GLO_COF);
        // translate to position
        glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() + 90);
        // scale
        glScaled(COFExaggeration, COFExaggeration, 1);
        // set color
        if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(RGBColor::RED);
        }
        // draw circle
        GLHelper::drawFilledCircle((double) 1.3, s.getCircleResolution());
        // draw details
        if (!s.forceDrawForPositionSelection && (s.scale >= 5)) {
            // move to front
            glTranslated(0, 0, 0.1);
            // set color
            if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                GLHelper::setColor(s.colorSettings.selectedAdditionalColor.changedBrightness(-32));
            } else {
                GLHelper::setColor(RGBColor::BLACK);
            }
            // draw another circle
            GLHelper::drawFilledCircle((double) 1.1, s.getCircleResolution());
            // move to front
            glTranslated(0, 0, 0.1);
            // draw speed
            if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                GLHelper::drawText("F", Position(0, 0), .1, 1.2, s.colorSettings.selectedAdditionalColor, 180);
            } else {
                GLHelper::drawText("F", Position(0, 0), .1, 1.2, RGBColor::YELLOW, 180);
            }
        }
        // Pop symbol matrix
        GLHelper::popMatrix();
        // Pop COF name
        if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE) {
            GLHelper::popName();
        }
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(getParentAdditionals().front())) {
            GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::INSPECT, s, myAdditionalGeometry.getShape().front(), 1.3, COFExaggeration);
        }
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == getParentAdditionals().front())) {
            GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::FRONT, s, myAdditionalGeometry.getShape().front(), 1.3, COFExaggeration);
        }
    }
}


std::string
GNEFrictionCoefficientSymbol::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEFrictionCoefficientSymbol::getAttributeDouble(SumoXMLAttr /*key*/) const {
    throw InvalidArgument("Symbols cannot be edited");
}


void
GNEFrictionCoefficientSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNEFrictionCoefficientSymbol::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNEFrictionCoefficientSymbol::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEFrictionCoefficientSymbol::getPopUpID() const {
    return getParentLanes().front()->getPopUpID();
}


std::string
GNEFrictionCoefficientSymbol::getHierarchyName() const {
    return getParentLanes().front()->getHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEFrictionCoefficientSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


void
GNEFrictionCoefficientSymbol::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}

void
GNEFrictionCoefficientSymbol::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
