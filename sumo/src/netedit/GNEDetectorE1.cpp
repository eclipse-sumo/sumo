/****************************************************************************/
/// @file    GNEDetectorE1.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEDetectorE1.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE1::GNEDetectorE1(const std::string& id, GNELane* lane, GNEViewNet* viewNet, SUMOReal pos, SUMOReal freq, const std::string& filename, bool splitByType) :
    GNEDetector(id, viewNet, SUMO_TAG_E1DETECTOR, ICON_E1, lane, pos, freq, filename),
    mySplitByType(splitByType) {
    // Update geometry;
    updateGeometry();
    // Set Colors
    myBaseColor = RGBColor(255, 255, 50, 0);
    myBaseColorSelected = RGBColor(255, 255, 125, 255);
}


GNEDetectorE1::~GNEDetectorE1() {
}


void
GNEDetectorE1::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // Get shape of lane parent
    myShape.push_back(myLane->getShape().positionAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x())));

    // Obtain first position
    Position f = myShape[0] - Position(1, 0);

    // Obtain next position
    Position s = myShape[0] + Position(1, 0);

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(myLane->getShape().rotationDegreeAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x())) * -1);

    // Set offset of logo
    myDetectorLogoOffset = Position(1, 0);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(-1, 0);

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


Position
GNEDetectorE1::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x()));
}


void
GNEDetectorE1::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition.x());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
    if (!myFilename.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    device.writeAttr(SUMO_ATTR_SPLIT_VTYPE, mySplitByType);
    if (myBlocked) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlocked);
    }
    // Close tag
    device.closeTag();
}


void
GNEDetectorE1::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // draw shape
    glColor3d(1, 1, 0);
    glPushMatrix();
    glTranslated(0, 0, getType());
    glTranslated(myShape[0].x(), myShape[0].y(), 0);
    glRotated(myShapeRotations[0], 0, 0, 1);
    glScaled(exaggeration, exaggeration, 1);
    glBegin(GL_QUADS);
    glVertex2d(-1.0,  2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0,  2);
    glEnd();
    glTranslated(0, 0, .01);
    glBegin(GL_LINES);
    glVertex2d(0, 2 - .1);
    glVertex2d(0, -2 + .1);
    glEnd();

    // outline
    if (width * exaggeration > 1) {
        glColor3d(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2f(-1.0,  2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0,  2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if (width * exaggeration > 1) {
        glRotated(90, 0, 0, -1);
        glColor3d(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }

    // Pop shape matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        // Add a draw matrix
        drawDetectorIcon(GUITextureSubSys::getTexture(GNETEXTURE_E1));

        // Show Lock icon depending of the Edit mode
        drawLockIcon();
    }

    // Finish draw
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


std::string
GNEDetectorE1::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return toString(myLane->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosition.x());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_SPLIT_VTYPE:
            return toString(mySplitByType);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE1::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_SPLIT_VTYPE:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNEDetectorE1::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) <= (myLane->getLaneParametricLenght()));
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) > 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_SPLIT_VTYPE:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE1::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            changeLane(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = Position(parse<SUMOReal>(value), 0);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_SPLIT_VTYPE:
            mySplitByType = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            getViewNet()->update();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
