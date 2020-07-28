/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEVariableSpeedSignSymbol.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignSymbol::GNEVariableSpeedSignSymbol(GNEAdditional* VSSParent, GNELane* lane) :
    GNEAdditional(VSSParent->getNet(), GLO_VSS, GNE_TAG_VSS_SYMBOL, "", false,
        {}, {}, {lane}, {VSSParent}, {}, {}, {}, {}) {             
}


GNEVariableSpeedSignSymbol::~GNEVariableSpeedSignSymbol() {
}


void
GNEVariableSpeedSignSymbol::moveGeometry(const Position& /*offset*/) {
    // Nothing to do
}


void
GNEVariableSpeedSignSymbol::commitGeometryMoving(GNEUndoList* /*undoList*/) {
    // Nothing to do
}


void
GNEVariableSpeedSignSymbol::updateGeometry() {

}


Position
GNEVariableSpeedSignSymbol::getPositionInView() const {
    return getParentLanes().front()->getLaneShape().positionAtOffset(getParentLanes().front()->getLaneShape().length());
}


Boundary
GNEVariableSpeedSignSymbol::getCenteringBoundary() const {
    return myAdditionalGeometry.getShape().getBoxBoundary().grow(10);
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
    if (s.drawAdditionals(VSSExaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // calulate position and rotation
        // obtain lanePos and route
        const Position& lanePos = getParentLanes().front()->getLaneShape().positionAtOffset(getParentLanes().front()->getLaneShape().length());
        const double laneRot = getParentLanes().front()->getLaneShape().rotationDegreeAtOffset(getParentLanes().front()->getLaneShape().length());
        // Start drawing adding an VSS gl identificator (used to identify element after clicking)
        glPushName(getParentAdditionals().front()->getGlID());
        // start drawing symbol
        glPushMatrix();
        glTranslated(lanePos.x(), lanePos.y(), GLO_VSS);
        glRotated(-1 * laneRot, 0, 0, 1);
        glTranslated(0, -1.5, 0);
        glScaled(VSSExaggeration, VSSExaggeration, 1);
        // draw circle
        int noPoints = 9;
        if (s.scale > 25) {
            noPoints = (int)(9.0 + s.scale / 10.0);
            if (noPoints > 36) {
                noPoints = 36;
            }
        }
        glColor3d(1, 0, 0);
        GLHelper::drawFilledCircle((double) 1.3, noPoints);
        if (!s.drawForRectangleSelection && (s.scale >= 5)) {
            glTranslated(0, 0, 0.1);
            glColor3d(0, 0, 0);
            GLHelper::drawFilledCircle((double) 1.1, noPoints);
            // draw the speed string
            glColor3d(1, 1, 0);
            glTranslated(0, 0, 0.1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // draw last value string
            GLHelper::drawText("S", Position(0, 0), .1, 1.2, RGBColor(255, 255, 0), 180);
        }
        // Pop symbol matrix
        glPopMatrix();
        // Pop VSS name
        glPopName();
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->getInspectedAttributeCarrier() == getParentAdditionals().front()) {
            // GLHelper::drawShapeDottedContourRectangle(s, getType(), lanePos, 2.6, 2.6, -1 * laneRot, 0, -1.5);
        }
        // Draw connections
        getParentAdditionals().front()->drawHierarchicalConnections(s, getType(), VSSExaggeration);
    }
}


std::string
GNEVariableSpeedSignSymbol::getAttribute(SumoXMLAttr key) const {
    switch (key) {
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


void
GNEVariableSpeedSignSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNEVariableSpeedSignSymbol::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNEVariableSpeedSignSymbol::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEVariableSpeedSignSymbol::getPopUpID() const {
    return getParentAdditionals().at(0)->getPopUpID();
}


std::string
GNEVariableSpeedSignSymbol::getHierarchyName() const {
    return getParentAdditionals().at(0)->getHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVariableSpeedSignSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


/****************************************************************************/
