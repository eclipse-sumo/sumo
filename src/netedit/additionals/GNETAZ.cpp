/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZ.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/GNEViewParent.h>

#include "GNETAZ.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNETAZ::myHintSize = 0.8;
const double GNETAZ::myHintSizeSquared = 0.64;

// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZ::GNETAZ(const std::string& id, GNEViewNet* viewNet, PositionVector shape, RGBColor color, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_E3DETECTOR, SUMO_TAG_TAZ, "", blockMovement),
    myColor(color),
    myBlockShape(false) {
    myGeometry.shape = shape;
}


GNETAZ::~GNETAZ() {}


void
GNETAZ::updateGeometry(bool /*updateGrid*/) {
    // Nothing to do
}


Position
GNETAZ::getPositionInView() const {
    return myGeometry.shape.getCentroid();
}


void
GNETAZ::moveGeometry(const Position& offset) {
    // restore old position, apply offset and update Geometry
    myGeometry.shape[0] = myMove.originalViewPosition;
    myGeometry.shape[0].add(offset);
    myGeometry.shape[0] = myViewNet->snapToActiveGrid(myGeometry.shape[0]);
    updateGeometry(false);
}


void
GNETAZ::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(myGeometry.shape[0]), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


std::string
GNETAZ::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNETAZ::drawGL(const GUIVisualizationSettings& s) const {
    if (s.polySize.getExaggeration(s) == 0) {
        return;
    }
    Boundary boundary = myGeometry.shape.getBoxBoundary();
    int circleResolution = GNEAttributeCarrier::getCircleResolution(s);
    if (s.scale * MAX2(boundary.getWidth(), boundary.getHeight()) < s.polySize.minSize) {
        return;
    }
        glPushName(getGlID());
    if (myGeometry.shape.size() > 1) {
        glPushMatrix();
        glTranslated(0, 0, 128);
        GLHelper::setColor(myColor);
        GLHelper::drawLine(myGeometry.shape);
        GLHelper::drawBoxLines(myGeometry.shape, 1);
        glPopMatrix();
        const Position namePos = myGeometry.shape.getPolygonCenter();
        drawName(namePos, s.scale, s.polyName, s.angle);
    }
    // draw geometry details hints if is not too small and isn't in selecting mode
    if (s.scale * myHintSize > 1.) {
        // set values relative to mouse position regarding to shape
        bool mouseOverVertex = false;
        bool modeMove = myViewNet->getCurrentEditMode() == GNE_MODE_MOVE;
        Position mousePosition = myViewNet->getPositionInformation();
        double distanceToShape = myGeometry.shape.distance2D(mousePosition);
        // set colors
        RGBColor invertedColor, darkerColor;
        if (isAttributeCarrierSelected()) {
            invertedColor = myViewNet->getNet()->selectionColor.invertedColor();
            darkerColor = myViewNet->getNet()->selectedLaneColor;
        } else {
            invertedColor = GLHelper::getColor().invertedColor();
            darkerColor = GLHelper::getColor().changedBrightness(-32);
        }
        // Draw geometry hints if polygon's shape isn't blocked
        if (myBlockShape == false) {
            // draw a boundary for moving using darkerColor
            glPushMatrix();
            glTranslated(0, 0, GLO_POLYGON + 0.01);
            GLHelper::setColor(darkerColor);
            GLHelper::drawBoxLines(myGeometry.shape, (myHintSize / 4) * s.polySize.getExaggeration(s));
            glPopMatrix();
            // draw points of shape
            for (auto i : myGeometry.shape) {
                if (!s.drawForSelecting || (myViewNet->getPositionInformation().distanceSquaredTo(i) <= (myHintSizeSquared + 2))) {
                    glPushMatrix();
                    glTranslated(i.x(), i.y(), GLO_POLYGON + 0.02);
                    // Change color of vertex and flag mouseOverVertex if mouse is over vertex
                    if (modeMove && (i.distanceTo(mousePosition) < myHintSize)) {
                        mouseOverVertex = true;
                        GLHelper::setColor(invertedColor);
                    } else {
                        GLHelper::setColor(darkerColor);
                    }
                    GLHelper::drawFilledCircle(myHintSize, circleResolution);
                    glPopMatrix();
                    // draw special symbols (Start, End and Block)
                    if ((i == myGeometry.shape.front()) && !s.drawForSelecting) {
                        // draw a "s" over first point
                        glPushMatrix();
                        glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                        GLHelper::drawText("S", Position(), .1, 2 * myHintSize, invertedColor);
                        glPopMatrix();
                    } else if ((i == myGeometry.shape.back()) && !s.drawForSelecting) {
                        // draw a "e" over last point if polygon isn't closed
                        glPushMatrix();
                        glTranslated(i.x(), i.y(), GLO_POLYGON + 0.03);
                        GLHelper::drawText("E", Position(), .1, 2 * myHintSize, invertedColor);
                        glPopMatrix();
                    }
                }
            }
            // check if draw moving hint has to be drawed
            if (modeMove && (mouseOverVertex == false) && (myBlockMovement == false) && (distanceToShape < myHintSize)) {
                // push matrix
                glPushMatrix();
                Position hintPos = myGeometry.shape.size() > 1 ? myGeometry.shape.positionAtOffset2D(myGeometry.shape.nearest_offset_to_point2D(mousePosition)) : myGeometry.shape[0];
                glTranslated(hintPos.x(), hintPos.y(), GLO_POLYGON + 0.04);
                GLHelper::setColor(invertedColor);
                GLHelper:: drawFilledCircle(myHintSize, circleResolution);
                glPopMatrix();
            }
        }
    }
    // check if dotted contour has to be drawn
    if (myViewNet->getACUnderCursor() == this) {
        GLHelper::drawShapeDottedContour(getType(), getShape());
    }
    // pop name
    glPopName();
}


std::string
GNETAZ::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_SHAPE:
            return toString(myGeometry.shape);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
         case GNE_ATTR_BLOCK_SHAPE:
            return toString(myBlockShape);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_COLOR:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_BLOCK_SHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZ::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_SHAPE:
            return canParse<PositionVector>(value);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_SHAPE:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNETAZ::getPopUpID() const {
    return toString(getTag()) + ":" + getID();
}


std::string
GNETAZ::getHierarchyName() const {
    return toString(getTag());
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_SHAPE:
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            myGeometry.shape = parse<PositionVector>(value);
            myViewNet->getNet()->addGLObjectIntoGrid(this);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
        case GNE_ATTR_BLOCK_SHAPE:
            myBlockShape = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
