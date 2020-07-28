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
/// @file    GNERerouterSymbol.cpp
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

#include "GNERerouterSymbol.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterSymbol::GNERerouterSymbol(GNEAdditional* rerouterParent, GNEEdge* edge) :
    GNEAdditional(rerouterParent->getNet(), GLO_REROUTER, GNE_TAG_REROUTER_SYMBOL, "", false,
        {}, {edge}, {}, {rerouterParent}, {}, {}, {}, {}) {             
}


GNERerouterSymbol::~GNERerouterSymbol() {
}


void
GNERerouterSymbol::moveGeometry(const Position& /*offset*/) {
    // Nothing to do
}


void
GNERerouterSymbol::commitGeometryMoving(GNEUndoList* /*undoList*/) {
    // Nothing to do
}


void
GNERerouterSymbol::updateGeometry() {

}


Position
GNERerouterSymbol::getPositionInView() const {
    const GNELane* firstLane = getParentEdges().front()->getLanes().at(0);
    return firstLane->getLaneShape().positionAtOffset(firstLane->getLaneShape().length());
}


Boundary
GNERerouterSymbol::getCenteringBoundary() const {
    return myAdditionalGeometry.getShape().getBoxBoundary().grow(10);
}


void 
GNERerouterSymbol::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, 
    const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to split
}


std::string
GNERerouterSymbol::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNERerouterSymbol::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double rerouteExaggeration = s.addSize.getExaggeration(s, getParentAdditionals().front());
    // first check if additional has to be drawn
    if (s.drawAdditionals(rerouteExaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Start drawing adding an gl identificator
        glPushName(getParentAdditionals().front()->getGlID());
        // draw rerouter symbol over all lanes
        for (const auto& lane : getParentEdges().front()->getLanes()) {
            // calculate lane position and rotation
            const Position& lanePos = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
            const double laneRot = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
            // draw rerouter symbol
            glPushMatrix();
            glTranslated(lanePos.x(), lanePos.y(), GLO_REROUTER);
            glRotated(-1 * laneRot, 0, 0, 1);
            glScaled(rerouteExaggeration, rerouteExaggeration, 1);
            // mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_TRIANGLES);
            glColor3d(1, .8f, 0);
            // base
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 - 1.4, 6);
            glVertex2d(0 + 1.4, 6);
            glVertex2d(0 + 1.4, 0);
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 + 1.4, 6);
            glEnd();
            // draw "U"
            if (!s.drawForRectangleSelection) {
                GLHelper::drawText("U", Position(0, 2), .1, 3, RGBColor::BLACK, 180);
                double probability = parse<double>(getParentAdditionals().front()->getAttribute(SUMO_ATTR_PROB)) * 100;
                // draw Probability
                GLHelper::drawText((toString(probability) + "%").c_str(), Position(0, 4), .1, 0.7, RGBColor::BLACK, 180);
            }
            // finish draw
            glPopMatrix();
            // draw contour if is selected
            if (myNet->getViewNet()->getInspectedAttributeCarrier() == getParentAdditionals().front()) {
                // GLHelper::drawShapeDottedContourRectangle(s, getType(), lanePos, 2.8, 6, -1 * laneRot, 0, 3);
            }
        }
        // Pop name
        glPopName();
        // Draw connections
        getParentAdditionals().front()->drawHierarchicalConnections(s, getType(), rerouteExaggeration);
    }
}


std::string
GNERerouterSymbol::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERerouterSymbol::getAttributeDouble(SumoXMLAttr /*key*/) const {
    throw InvalidArgument("Symbols cannot be edited");
}


void
GNERerouterSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNERerouterSymbol::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


bool
GNERerouterSymbol::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNERerouterSymbol::getPopUpID() const {
    return getParentAdditionals().at(0)->getPopUpID();
}


std::string
GNERerouterSymbol::getHierarchyName() const {
    return getParentAdditionals().at(0)->getHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouterSymbol::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("Symbols cannot be edited");
}


/****************************************************************************/
