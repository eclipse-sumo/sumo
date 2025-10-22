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
/// @file    GNERerouterSymbol.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <utils/gui/div/GLHelper.h>

#include "GNERerouterSymbol.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterSymbol::GNERerouterSymbol(GNENet* net) :
    GNEAdditional("", net, "", GNE_TAG_REROUTER_SYMBOL, ""),
    mySymbolContours(new std::vector<GNEContour*>()) {
}


GNERerouterSymbol::GNERerouterSymbol(GNEAdditional* rerouterParent, GNEEdge* edge) :
    GNEAdditional(rerouterParent, GNE_TAG_REROUTER_SYMBOL, ""),
    mySymbolContours(new std::vector<GNEContour*>()) {
    // set parents
    setParent<GNEEdge*>(edge);
    setParent<GNEAdditional*>(rerouterParent);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNERerouterSymbol::~GNERerouterSymbol() {
    for (auto it = mySymbolContours->begin(); it != mySymbolContours->end(); it++) {
        delete *it;
    }
    delete mySymbolContours;
}


GNEMoveElement*
GNERerouterSymbol::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERerouterSymbol::getParameters() {
    return nullptr;
}


const Parameterised*
GNERerouterSymbol::getParameters() const {
    return nullptr;
}


void
GNERerouterSymbol::writeAdditional(OutputDevice& /*device*/) const {
    // noting to write
}


bool
GNERerouterSymbol::isAdditionalValid() const {
    return true;
}


std::string
GNERerouterSymbol::getAdditionalProblem() const {
    return "";
}


void
GNERerouterSymbol::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNERerouterSymbol::checkDrawMoveContour() const {
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
GNERerouterSymbol::updateGeometry() {
    // clear geometries
    mySymbolGeometries.clear();
    // iterate over all lanes
    NBEdge* nbe = getParentEdges().front()->getNBEdge();
    // clear all contours
    for (auto it = mySymbolContours->begin(); it != mySymbolContours->end(); it++) {
        delete *it;
    }
    // clear all edge geometries
    mySymbolGeometries.clear();
    mySymbolContours->clear();
    for (const auto& lane : getParentEdges().front()->getChildLanes()) {
        if ((nbe->getPermissions(lane->getIndex()) & ~SVC_PEDESTRIAN) == 0) {
            continue;
        }
        // declare geometry
        GUIGeometry symbolGeometry;
        // update it with lane and pos over lane
        symbolGeometry.updateGeometry(lane->getLaneShape(), lane->getLaneShape().length2D() - 6, 0);
        // add in mySymbolGeometries
        mySymbolGeometries.push_back(symbolGeometry);
        // also add a new contour
        mySymbolContours->push_back(new GNEContour());
        // use last symbol geometry as additional geometry
        myAdditionalGeometry = mySymbolGeometries.back();
    }
}


Position
GNERerouterSymbol::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNERerouterSymbol::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
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
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double rerouteExaggeration = s.addSize.getExaggeration(s, getParentAdditionals().front());
        // get detail level
        const auto d = s.getDetailLevel(rerouteExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw rerouter symbol
            drawRerouterSymbol(s, d, rerouteExaggeration);
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // draw dotted contour
            for (const auto symbolContour : *mySymbolContours) {
                symbolContour->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour rectangle shape
        for (int i = 0; i < (int)mySymbolContours->size(); i++) {
            mySymbolContours->at(i)->calculateContourRectangleShape(s, d, this, mySymbolGeometries.at(i).getShape().front(), 1, 3, getType(), 0, 3,
                    mySymbolGeometries.at(i).getShapeRotations().front() + 90, rerouteExaggeration, getParentEdges().front());
        }
    }
}


std::string
GNERerouterSymbol::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNERerouterSymbol::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNERerouterSymbol::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNERerouterSymbol::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNERerouterSymbol::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    setCommonAttribute(key, value, undoList);
}


bool
GNERerouterSymbol::isValid(SumoXMLAttr key, const std::string& value) {
    return isCommonAttributeValid(key, value);
}


std::string
GNERerouterSymbol::getPopUpID() const {
    return getParentEdges().front()->getPopUpID();
}


std::string
GNERerouterSymbol::getHierarchyName() const {
    return getParentEdges().front()->getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouterSymbol::drawRerouterSymbol(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                      const double exaggeration) const {
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to front
    getParentAdditionals().front()->drawInLayer(GLO_REROUTER);
    // set color
    RGBColor color;
    if (getParentAdditionals().front()->isAttributeCarrierSelected()) {
        color = s.colorSettings.selectedAdditionalColor;
    } else {
        color = RGBColor(255, 231, 0);
    }
    // draw rerouter symbol over all lanes
    for (const auto& symbolGeometry : mySymbolGeometries) {
        // push symbol matrix
        GLHelper::pushMatrix();
        // translate to position
        glTranslated(symbolGeometry.getShape().front().x(), symbolGeometry.getShape().front().y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(symbolGeometry.getShapeRotations().front() + 90);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // set color
        GLHelper::setColor(color);
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
        if (d <= GUIVisualizationSettings::Detail::Text) {
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
}


void
GNERerouterSymbol::setAttribute(SumoXMLAttr key, const std::string& value) {
    setCommonAttribute(key, value);
}

/****************************************************************************/
