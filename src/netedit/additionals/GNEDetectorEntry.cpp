/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorEntry.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
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
#include <netbuild/NBEdge.h>
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
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNEEdge.h>

#include "GNEDetectorEntry.h"
#include "GNEDetectorE3.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorEntry::GNEDetectorEntry(GNEViewNet* viewNet, GNEAdditional* parent, GNELane* lane, double pos, bool friendlyPos, bool blockMovement) :
    GNEDetector(parent, viewNet, GLO_DET_ENTRY, SUMO_TAG_DET_ENTRY, lane, pos, 0, "", "", friendlyPos, blockMovement) {
}


GNEDetectorEntry::~GNEDetectorEntry() {}


void
GNEDetectorEntry::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // obtain position over lane
    double fixedPositionOverLane = myPositionOverLane > myLane->getParentEdge().getNBEdge()->getFinalLength() ? myLane->getParentEdge().getNBEdge()->getFinalLength() : myPositionOverLane < 0 ? 0 : myPositionOverLane;
    myShape.push_back(myLane->getShape().positionAtOffset(fixedPositionOverLane * myLane->getLengthGeometryFactor()));

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(myLane->getShape().rotationDegreeAtOffset(fixedPositionOverLane) * -1);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(myLane);

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }

    // update E3 parent Geometry
    myFirstAdditionalParent->updateGeometry(updateGrid);
}


bool GNEDetectorEntry::isDetectorPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return (myPositionOverLane >= 0) && (myPositionOverLane <= myLane->getParentEdge().getNBEdge()->getFinalLength());
    }
}


void
GNEDetectorEntry::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding gl identificator
    glPushName(getGlID());

    // Push detector matrix
    glPushMatrix();
    glTranslated(0, 0, getType());

    // Set initial values
    if (isAttributeCarrierSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.SUMO_color_E3Entry);
    }
    const double exaggeration = s.addSize.getExaggeration(s);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Push polygon matrix
    glPushMatrix();
    glScaled(exaggeration, exaggeration, 1);
    glTranslated(myShape[0].x(), myShape[0].y(), 0);
    glRotated(myShapeRotations[0], 0, 0, 1);

    // Draw polygon
    glBegin(GL_LINES);
    glVertex2d(1.7, 0);
    glVertex2d(-1.7, 0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2d(-1.7, .5);
    glVertex2d(-1.7, -.5);
    glVertex2d(1.7, -.5);
    glVertex2d(1.7, .5);
    glEnd();

    // draw details if isn't being drawn for selecting
    if (!s.drawForSelecting) {
        // first Arrow
        glTranslated(1.5, 0, 0);
        GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
        GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);

        // second Arrow
        glTranslated(-3, 0, 0);
        GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
        GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
    }

    // Pop polygon matrix
    glPopMatrix();

    // Pop detector matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (((s.scale * exaggeration) >= 10)) {
        // Push matrix
        glPushMatrix();
        // Traslate to center of detector
        glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        //move to logo position
        glTranslated(1.9, 0, 0);
        // draw Entry logo if isn't being drawn for selecting
        if (s.drawForSelecting) {
            GLHelper::setColor(s.SUMO_color_E3Entry);
            GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
        } else if (isAttributeCarrierSelected()) {
            GLHelper::drawText("E3", Position(), .1, 2.8, myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::drawText("E3", Position(), .1, 2.8, s.SUMO_color_E3Entry);
        }
        //move to logo position
        glTranslated(1.7, 0, 0);
        // Rotate depending of myBlockIconRotation
        glRotated(90, 0, 0, 1);
        // draw Entry text if isn't being drawn for selecting
        if (s.drawForSelecting) {
            GLHelper::setColor(s.SUMO_color_E3Entry);
            GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
        } else if (isAttributeCarrierSelected()) {
            GLHelper::drawText("Entry", Position(), .1, 1, myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::drawText("Entry", Position(), .1, 1, s.SUMO_color_E3Entry);
        }
        // pop matrix
        glPopMatrix();
        // Show Lock icon depending of the Edit mode and if isn't being drawn for selecting
        if (!s.drawForSelecting) {
            drawLockIcon(0.4);
        }
    }
    // Draw name if isn't being drawn for selecting
    if (!s.drawForSelecting) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }
    // check if dotted contour has to be drawn
    if (!s.drawForSelecting && (myViewNet->getACUnderCursor() == this)) {
        GLHelper::drawShapeDottedContour(getType(), myShape[0], 3.4, 5, myShapeRotations[0], 0, 2);
    }
    // pop gl identificator
    glPopName();
}


std::string
GNEDetectorEntry::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_PARENT:
            return myFirstAdditionalParent->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorEntry::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_PARENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorEntry::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            return (myViewNet->getNet()->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_PARENT:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, value, false) != nullptr);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

void
GNEDetectorEntry::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            changeFirstAdditionalParent(value);
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
    // After setting attribute always update Geometry
    updateGeometry(true);
}

/****************************************************************************/
