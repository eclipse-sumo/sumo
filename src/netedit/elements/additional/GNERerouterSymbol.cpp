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
/// @file    GNERerouterSymbol.cpp
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

#include "GNERerouterSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterSymbol::GNERerouterSymbol(GNEAdditional* rerouterParent, GNEEdge* edge) :
    GNEAdditional(rerouterParent->getNet(), GLO_REROUTER, GNE_TAG_REROUTER_SYMBOL, "",
        {}, {edge}, {}, {rerouterParent}, {}, {}, {}, {},
        std::map<std::string, std::string>(), false) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNERerouterSymbol::~GNERerouterSymbol() {
}


GNEMoveOperation*
GNERerouterSymbol::getMoveOperation(const double /*shapeOffset*/) {
    // GNERerouterSymbols cannot be moved
    return nullptr;
}


void
GNERerouterSymbol::updateGeometry() {
    // clear geometries
    mySymbolGeometries.clear();
    // iterate over all lanes
    for (const auto& lane : getParentEdges().front()->getLanes()) {
        // declare geometry
        GNEGeometry::Geometry symbolGeometry;
        // update it with lane and pos over lane
        symbolGeometry.updateGeometry(lane->getLaneShape(), lane->getLaneShape().length2D() - 6, 0);
        // add in mySymbolGeometries
        mySymbolGeometries.push_back(symbolGeometry);
    }
    // add shape boundary
    myBoundary = mySymbolGeometries.front().getShape().getBoxBoundary();
    // grow
    myBoundary.grow(10);
    // update connections
    getParentAdditionals().front()->updateHierarchicalConnections();
}


Position
GNERerouterSymbol::getPositionInView() const {
    return mySymbolGeometries.front().getShape().getCentroid();
}


void
GNERerouterSymbol::updateCenteringBoundary(const bool /*updateGrid*/) {
    // just update geometry
    updateGeometry();
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
        // Start drawing adding an gl identificator (except in Move mode)
        if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE) {
            GLHelper::pushName(getParentAdditionals().front()->getGlID());
        }
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(getParentAdditionals().front(), GLO_REROUTER);
        // draw rerouter symbol over all lanes
        for (const auto& symbolGeometry : mySymbolGeometries) {
            // push symbol matrix
            GLHelper::pushMatrix();
            // translate to position
            glTranslated(symbolGeometry.getShape().front().x(), symbolGeometry.getShape().front().y(), 0);
            // rotate over lane
            GNEGeometry::rotateOverLane(symbolGeometry.getShapeRotations().front() + 90);
            // scale
            glScaled(rerouteExaggeration, rerouteExaggeration, 1);
            // set color
            if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
            } else {
                glColor3d(1, .8f, 0);
            }
            // set draw mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_TRIANGLES);
            // base
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 - 1.4, 6);
            glVertex2d(0 + 1.4, 6);
            glVertex2d(0 + 1.4, 0);
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 + 1.4, 6);
            glEnd();
            // draw "U"
            if (!s.drawForPositionSelection) {
                // set text color
                RGBColor textColor;
                if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
                    textColor = s.colorSettings.selectedAdditionalColor.changedBrightness(-32);
                } else {
                    textColor = RGBColor::BLACK;
                }
                // get probability
                const std::string probability = toString(getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_PROB) * 100) + "%";
                // draw U
                GLHelper::drawText("U", Position(0, 2), .1, 3, textColor, 180);
                // draw Probability
                GLHelper::drawText(probability.c_str(), Position(0, 4), .1, 0.7, textColor, 180);
            }
            // pop symbol matrix
            GLHelper::popMatrix();
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_MOVE) {
            GLHelper::popName();
        }
        // check if dotted contour has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(getParentAdditionals().front())) {
            // iterate over symbol geometries
            for (const auto& symbolGeometry : mySymbolGeometries) {
                GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::INSPECT, s, symbolGeometry.getShape().front(), 1, 3, 0, 3, symbolGeometry.getShapeRotations().front() + 90, rerouteExaggeration);
            }
        }
        if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == getParentAdditionals().front())) {
            // iterate over symbol geometries
            for (const auto& symbolGeometry : mySymbolGeometries) {
                GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::FRONT, s, symbolGeometry.getShape().front(), 1, 3, 0, 3, symbolGeometry.getShapeRotations().front() + 90, rerouteExaggeration);
            }
        }
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


void
GNERerouterSymbol::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNERerouterSymbol::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/)  {
    // nothing to do
}

/****************************************************************************/
