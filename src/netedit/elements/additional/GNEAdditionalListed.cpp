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
/// @file    GNEAdditionalListed.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// A abstract class for representation of additional listed elements
/****************************************************************************/

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEPathCreator.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEAdditionalListed.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalListed::GNEAdditionalListed(GNEAdditional* additional) :
    myAdditional(additional) {
}



void
GNEAdditionalListed::updateGeometryListedAdditional(GUIGeometry& additionalGeometry, const Position& parentPosition, const int level) {
    // we assume that the radius of parent element is 1
    const int radiusParent = 1;
    // get draw position index
    myDrawPositionIndex = getDrawPositionIndex();
    // calculate x and y position
    const double xPosition = radiusParent + lineLenght + (lineLenght + shapeWidth) * level;
    const double yPosition = ((2 * shapeHeight) + ySeparation) * myDrawPositionIndex * -1;
    // calculate y position
    const auto startPos = parentPosition + Position(xPosition, yPosition);
    const auto endPos = parentPosition + Position(xPosition + shapeWidth, yPosition);
    // set geometries
    additionalGeometry.updateGeometry({startPos, endPos});
    myInternalRectangle.updateGeometry({startPos + Position(padding, 0), endPos - Position(padding, 0)});
    // calculate icon size
    myIconSize = shapeHeight - (2 * iconPadding);
    // calculate signPosition
    myIconPosition = startPos + Position((2 * iconPadding) + myIconSize, 0);
    // calculate text position
    myTextPosition = myIconPosition + Position(shapeHeight, 0);
    // now calculate lines
    PositionVector linePositions;
    linePositions.push_back(parentPosition + Position(xPosition - lineLenght, 0));
    linePositions.push_back(parentPosition + Position(xPosition - (lineLenght * 0.5), 0));
    linePositions.push_back(startPos - Position((lineLenght * 0.5), 0));
    linePositions.push_back(startPos);
    myLineGeometry.updateGeometry(linePositions);
    // update centering boundary (needed for centering)
    myAdditional->updateCenteringBoundary(false);
}


void
GNEAdditionalListed::drawListedAdditional(const GUIVisualizationSettings& s, const RGBColor baseCol, const RGBColor textCol,
        GUITexture texture, const std::string text, const GNEContour& additionalContour) const {
    // check if additional has to be drawn
    if (myAdditional->getNet()->getViewNet()->getDataViewOptions().showAdditionals()) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw boundaries
        GLHelper::drawBoundary(s, myAdditional->getCenteringBoundary());
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, myAdditional->isAttributeCarrierSelected())) {
            // calculate colors
            const RGBColor baseColor = myAdditional->isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor : baseCol;
            const RGBColor secondColor = baseColor.changedBrightness(-30);
            const RGBColor textColor = myAdditional->isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor.changedBrightness(30) : textCol;
            // Add layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myAdditional->drawInLayer(myAdditional->getType());
            // draw lines
            GLHelper::setColor(s.additionalSettings.connectionColor);
            GUIGeometry::drawGeometry(d, myLineGeometry, lineWidth);
            // draw extern rectangle
            GLHelper::setColor(secondColor);
            GUIGeometry::drawGeometry(d, myAdditional->getAdditionalGeometry(), shapeHeight);
            // move to front
            glTranslated(0, 0, 0.1);
            // draw intern rectangle
            GLHelper::setColor(baseColor);
            GUIGeometry::drawGeometry(d, myInternalRectangle, shapeHeight - padding);
            // draw interval
            GLHelper::drawText(adjustListedAdditionalText(text), myTextPosition, .1, 0.5, textColor, 0, (FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE));
            // check if draw lock icon or rerouter interval icon
            if (GNEViewNetHelper::LockIcon::checkDrawing(d, myAdditional, myAdditional->getType(), 1)) {
                // pop layer matrix
                GLHelper::popMatrix();
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(d, myAdditional, myAdditional->getType(), myIconPosition, 1, myIconSize * 0.75);
            } else {
                // translate to front
                glTranslated(myIconPosition.x(), myIconPosition.y(), 0.1);
                // set White color
                glColor3d(1, 1, 1);
                // rotate
                glRotated(180, 0, 0, 1);
                // draw texture
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), myIconSize);
                // pop layer matrix
                GLHelper::popMatrix();
            }
            // draw dotted contour
            additionalContour.drawDottedContours(s, d, myAdditional, s.dottedContourSettings.segmentWidthSmall, true);
        }
        // calculate contour
        additionalContour.calculateContourExtrudedShape(s, d, myAdditional, myAdditional->getAdditionalGeometry().getShape(), myAdditional->getType(), shapeHeight, 1, true, true, 0,
                nullptr, nullptr);
    }
}


Position
GNEAdditionalListed::getListedPositionInView() const {
    return myIconPosition;
}


int
GNEAdditionalListed::getDrawPositionIndex() const {
    // filter symbols
    std::vector<GNEAdditional*> children;
    for (const auto& child : myAdditional->getParentAdditionals().front()->getChildAdditionals()) {
        if (!child->getTagProperty()->isSymbol()) {
            children.push_back(child);
        }
    }
    // now get index
    for (int i = 0; i < (int)children.size(); i++) {
        if (children.at(i) == myAdditional) {
            return i;
        }
    }
    return 0;
}


std::string
GNEAdditionalListed::adjustListedAdditionalText(const std::string& text) const {
    // 10 + 3 + 10
    if (text.size() <= 23) {
        return text;
    } else {
        // get text size
        const int textPosition = (int)text.size() - 10;
        // declare strings
        std::string partA, partB;
        // resize
        partA.reserve(10);
        partB.reserve(10);
        // fill both
        for (int i = 0; i < 10; i++) {
            partA.push_back(text.at(i));
            partB.push_back(text.at(textPosition + i));
        }
        // return composition
        return (partA + "..." + partB);
    }
}

/****************************************************************************/
