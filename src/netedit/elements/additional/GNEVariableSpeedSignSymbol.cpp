/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEVariableSpeedSignSymbol.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEVariableSpeedSignSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignSymbol::GNEVariableSpeedSignSymbol(GNENet* net) :
    GNEAdditional("", net, GLO_VSS, GNE_TAG_VSS_SYMBOL, GUIIconSubSys::getIcon(GUIIcon::VARIABLESPEEDSIGN), "",
{}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEVariableSpeedSignSymbol::GNEVariableSpeedSignSymbol(GNEAdditional* VSSParent, GNELane* lane) :
    GNEAdditional(VSSParent->getNet(), GLO_VSS, GNE_TAG_VSS_SYMBOL, GUIIconSubSys::getIcon(GUIIcon::VARIABLESPEEDSIGN), "",
{}, {}, {lane}, {VSSParent}, {}, {}) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEVariableSpeedSignSymbol::~GNEVariableSpeedSignSymbol() {
}


GNEMoveOperation*
GNEVariableSpeedSignSymbol::getMoveOperation() {
    // GNEVariableSpeedSignSymbols cannot be moved
    return nullptr;
}


void GNEVariableSpeedSignSymbol::writeAdditional(OutputDevice& /*device*/) const {
    // nothing to write
}


bool
GNEVariableSpeedSignSymbol::isAdditionalValid() const {
    return true;
}


std::string
GNEVariableSpeedSignSymbol::getAdditionalProblem() const {
    return "";
}


void
GNEVariableSpeedSignSymbol::fixAdditionalProblem() {
    // nothing to fix
}


void
GNEVariableSpeedSignSymbol::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), 1.5, 0);
}


void
GNEVariableSpeedSignSymbol::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


Position
GNEVariableSpeedSignSymbol::getPositionInView() const {
    return myAdditionalGeometry.getShape().getCentroid();
}


void
GNEVariableSpeedSignSymbol::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/,
        const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to split
}


std::string
GNEVariableSpeedSignSymbol::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEVariableSpeedSignSymbol::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double VSSExaggeration = s.addSize.getExaggeration(s, getParentAdditionals().front());
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            (myAdditionalGeometry.getShape().size() > 0) && (myAdditionalGeometry.getShapeRotations().size() > 0)) {
        // get detail level
        const auto d = s.getDetailLevel(VSSExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (!s.drawForObjectUnderCursor) {
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // start drawing symbol
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(getParentAdditionals().front(), GLO_VSS);
            // translate to position
            glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
            // rotate over lane
            GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() + 90);
            // scale
            glScaled(VSSExaggeration, VSSExaggeration, 1);
            // set color
            RGBColor color;
            if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
            } else {
                GLHelper::setColor(RGBColor::RED);
            }
            // draw circle
            GLHelper::drawFilledCircleDetailled(d, (double) 1.3);
            // draw details
            if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
                // move to front
                glTranslated(0, 0, 0.1);
                // set color
                if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                    GLHelper::setColor(s.colorSettings.selectedAdditionalColor.changedBrightness(-32));
                } else {
                    GLHelper::setColor(RGBColor::BLACK);
                }
                // draw another circle
                GLHelper::drawFilledCircleDetailled(d, (double) 1.1);
                // move to front
                glTranslated(0, 0, 0.1);
                // draw speed
                if (d <= GUIVisualizationSettings::Detail::Text) {
                    if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                        GLHelper::drawText("S", Position(0, 0), .1, 1.2, s.colorSettings.selectedAdditionalColor, 180);
                    } else {
                        GLHelper::drawText("S", Position(0, 0), .1, 1.2, RGBColor::YELLOW, 180);
                    }
                }
            }
            // Pop symbol matrix
            GLHelper::popMatrix();
        }
        // draw dotted contour
        myContour.calculateContourCircleShape(s, d, myAdditionalGeometry.getShape().front(), 1.3, VSSExaggeration,
                                            s.dottedContourSettings.segmentWidth);
    }
}


std::string
GNEVariableSpeedSignSymbol::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEVariableSpeedSignSymbol::getAttributeDouble(SumoXMLAttr /*key*/) const {
    throw InvalidArgument("Symbols cannot be edited");
}


const Parameterised::Map&
GNEVariableSpeedSignSymbol::getACParametersMap() const {
    return PARAMETERS_EMPTY;
}


void
GNEVariableSpeedSignSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNEVariableSpeedSignSymbol::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


std::string
GNEVariableSpeedSignSymbol::getPopUpID() const {
    return getParentLanes().front()->getPopUpID();
}


std::string
GNEVariableSpeedSignSymbol::getHierarchyName() const {
    return getParentLanes().front()->getHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVariableSpeedSignSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


void
GNEVariableSpeedSignSymbol::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}

void
GNEVariableSpeedSignSymbol::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
