/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorExit.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEDetectorExit.h"
#include "GNEDetectorE3.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorExit::GNEDetectorExit(GNEViewNet* viewNet, GNEDetectorE3* parent, GNELane* lane, double pos, bool friendlyPos) :
    GNEDetector(parent->generateExitID(), viewNet, SUMO_TAG_DET_EXIT, ICON_E3EXIT, lane, pos, 0, "", friendlyPos, parent) {
}


GNEDetectorExit::~GNEDetectorExit() {}


void
GNEDetectorExit::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // obtain position over lane
    double fixedPositionOverLane = myPositionOverLane > 1 ? 1 : myPositionOverLane < 0 ? 0 : myPositionOverLane;
    myShape.push_back(myLane->getShape().positionAtOffset(fixedPositionOverLane * myLane->getShape().length()));

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(myLane->getShape().rotationDegreeAtOffset(fixedPositionOverLane * myLane->getShape().length()) * -1);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);

    // update E3 parent Geometry
    myAdditionalParent->updateGeometry();
}


void
GNEDetectorExit::writeAdditional(OutputDevice& device) const {
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_POSITION, getAbsolutePositionOverLane());
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    device.closeTag();
}


bool GNEDetectorExit::isDetectorPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        // floors are needed to avoid precision problems
        return ((floor(myPositionOverLane * 1000) / 1000) >= 0) && ((floor(myPositionOverLane * 1000) / 1000) <= 1);
    }
}


void
GNEDetectorExit::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding gl identificator
    glPushName(getGlID());

    // Push detector matrix
    glPushMatrix();
    glTranslated(0, 0, getType());

    // Set initial values
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.SUMO_color_E3Exit);
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

    // first Arrow
    glTranslated(1.5, 0, 0);
    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);

    // second Arrow
    glTranslated(-3, 0, 0);
    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);

    // Pop polygon matrix
    glPopMatrix();

    // Pop detector matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        // Push matrix
        glPushMatrix();
        // Traslate to center of detector
        glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        //move to logo position
        glTranslated(1.9, 0, 0);
        // draw Exit logo
        if (isAdditionalSelected()) {
            GLHelper::drawText("E3", Position(), .1, 2.8, myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::drawText("E3", Position(), .1, 2.8, RGBColor(204, 0, 0));
        }
        //move to logo position
        glTranslated(1.7, 0, 0);
        // Rotate depending of myBlockIconRotation
        glRotated(90, 0, 0, 1);
        // draw Exit text
        if (isAdditionalSelected()) {
            GLHelper::drawText("Exit", Position(), .1, 1, myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::drawText("Exit", Position(), .1, 1, RGBColor(204, 0, 0));
        }
        // pop matrix
        glPopMatrix();

        // Show Lock icon depending of the Edit mode
        drawLockIcon(0.4);
    }
    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // pop gl identificator
    glPopName();
}


std::string
GNEDetectorExit::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
        case SUMO_ATTR_POSITION:
            return toString(getAbsolutePositionOverLane());
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        case GNE_ATTR_PARENT:
            return myAdditionalParent->getID();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorExit::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorExit::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            return (myViewNet->getNet()->retrieveLane(value, false) != NULL);
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_PARENT:
            return (myViewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, value) != NULL);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

void
GNEDetectorExit::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            changeAdditionalParent(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}

/****************************************************************************/
