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
/// @file    GNEAdditionalSquared.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional squared elements
/****************************************************************************/

#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEAdditionalSquared.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalSquared::GNEAdditionalSquared(GNEAdditional* additional) :
    myAdditional(additional),
    myMoveElementView(new GNEMoveElementView(additional, GNEMoveElementView::AttributesFormat::POSITION,
                      SUMO_ATTR_POSITION, myPosOverView)) {
}


GNEAdditionalSquared::GNEAdditionalSquared(GNEAdditional* additional, const Position pos) :
    myAdditional(additional),
    myPosOverView(pos),
    myMoveElementView(new GNEMoveElementView(additional, GNEMoveElementView::AttributesFormat::POSITION,
                      SUMO_ATTR_POSITION, myPosOverView)) {
}


GNEAdditionalSquared::~GNEAdditionalSquared() {
    delete myMoveElementView;
}


void
GNEAdditionalSquared::updatedSquaredGeometry() {
    // update additional geometry
    myAdditional->myAdditionalGeometry.updateGeometry({myPosOverView - Position(1, 0), myPosOverView + Position(1, 0)});
    // update geometries of all children
    for (const auto& rerouterElement : myAdditional->getChildAdditionals()) {
        rerouterElement->updateGeometry();
    }
}


void
GNEAdditionalSquared::updatedSquaredCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myAdditional->getNet()->removeGLObjectFromGrid(myAdditional);
    }
    // now update geometry
    myAdditional->updateGeometry();
    // add shape boundary
    myAdditional->myAdditionalBoundary = myAdditional->myAdditionalGeometry.getShape().getBoxBoundary();
    // grow
    myAdditional->myAdditionalBoundary.grow(5);
    // add positions of all childrens (intervals and symbols)
    for (const auto& additionalChild : myAdditional->getChildAdditionals()) {
        myAdditional->myAdditionalBoundary.add(additionalChild->getCenteringBoundary());
        for (const auto& secondAdditionalChild : additionalChild->getChildAdditionals()) {
            myAdditional->myAdditionalBoundary.add(secondAdditionalChild->getCenteringBoundary());
            // special case for parking area rerouter
            if (secondAdditionalChild->getParentAdditionals().size() > 1) {
                myAdditional->myAdditionalBoundary.add(secondAdditionalChild->getParentAdditionals().at(1)->getCenteringBoundary());
            }
        }
    }
    // add additional into RTREE again
    if (updateGrid) {
        myAdditional->getNet()->addGLObjectIntoGrid(myAdditional);
    }
}


void
GNEAdditionalSquared::drawSquaredAdditional(const GUIVisualizationSettings& s, const double size,
        GUITexture texture, GUITexture selectedTexture) const {
    // draw boundaries
    GLHelper::drawBoundary(s, myAdditional->getCenteringBoundary());
    // Obtain drawing exaggeration
    const double exaggeration = myAdditional->getExaggeration(s);
    // get detail level
    const auto d = s.getDetailLevel(exaggeration);
    // draw geometry only if we'rent in drawForObjectUnderCursor mode
    if (s.checkDrawAdditional(d, myAdditional->isAttributeCarrierSelected())) {
        // Add layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myAdditional->drawInLayer(myAdditional->getType());
        // translate to position
        glTranslated(myPosOverView.x(), myPosOverView.y(), 0);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // set White color
        glColor3d(1, 1, 1);
        // rotate
        glRotated(180, 0, 0, 1);
        // draw texture
        if (myAdditional->drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(selectedTexture), size);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), size);
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(d, myAdditional, myAdditional->getType(), myPosOverView, exaggeration, 0.4, 0.5, 0.5);
        // Draw additional ID
        myAdditional->drawAdditionalID(s);
        // draw additional name
        myAdditional->drawAdditionalName(s);
        // draw dotted contour
        myAdditional->myAdditionalContour.drawDottedContours(s, d, myAdditional, s.dottedContourSettings.segmentWidth, true);
    }
    // calculate contour
    myAdditional->myAdditionalContour.calculateContourRectangleShape(s, d, myAdditional, myPosOverView, size, size, myAdditional->getType(), 0, 0, 0, exaggeration, nullptr);
}



/****************************************************************************/
