/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEParkingSpace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <foreign/fontstash/fontstash.h>
#include <iostream>
#include <string>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEChange_Attribute.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNEViewNet* viewNet, GNEParkingArea* parkingAreaParent, double x, double y, double z, double width, double length, double angle) :
    GNEAdditional(parkingAreaParent->generateParkingSpaceID(), viewNet, SUMO_TAG_PARKING_SPACE, ICON_PARKINGSPACE, true, parkingAreaParent),
    myPosition(Position(x,y)),
    myZ(z),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
}


GNEParkingSpace::~GNEParkingSpace() {}


void
GNEParkingSpace::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_X, myPosition.x());
    device.writeAttr(SUMO_ATTR_Y, myPosition.y());
    device.writeAttr(SUMO_ATTR_Z, myZ);
    device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    // Close tag
    device.closeTag();
}


void 
GNEParkingSpace::moveGeometry(const Position & oldPos, const Position & offset) {
    // restore old position, apply offset and update Geometry
    myPosition = oldPos;
    myPosition.add(offset);
    updateGeometry();
}


void 
GNEParkingSpace::commitGeometryMoving(const Position & oldPos, GNEUndoList * undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition), true, toString(oldPos)));
    undoList->p_end();
}


void
GNEParkingSpace::updateGeometry() {
    /**
    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Update common geometry of stopping place
    setStoppingPlaceGeometry();

    // Obtain a copy of the shape
    PositionVector tmpShape = myShape;

    // Move shape to side
    tmpShape.move2side(1.5 * offsetSign);

    // Get position of the sign
    mySignPos = tmpShape.getLineCenter();

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set block icon rotation, and using their rotation for sign
    setBlockIconRotation(myLane);
    **/
    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position 
GNEParkingSpace::getPositionInView() const {
    return myPosition;
}


const std::string&
GNEParkingSpace::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNEParkingSpace::drawGL(const GUIVisualizationSettings& s) const {
    /**
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType());

    // Set color of the base
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(RGBColor(76, 170, 50));
    }

    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s);

    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {

        // Add a draw matrix for details
        glPushMatrix();

        // Iterate over every line
        for (int i = 0; i < (int)myLines.size(); ++i) {
            // push a new matrix for every line
            glPushMatrix();

            // Rotate and traslaste
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            glRotated(-1 * myBlockIconRotation, 0, 0, 1);

            // draw line with a color depending of the selection status
            if (isAdditionalSelected()) {
                GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, myViewNet->getNet()->selectionColor, 0, FONS_ALIGN_LEFT);
            } else {
                GLHelper::drawText(myLines[i].c_str(), Position(1.2, (double)i), .1, 1.f, RGBColor(76, 170, 50), 0, FONS_ALIGN_LEFT);
            }

            // pop matrix for every line
            glPopMatrix();
        }

        // Start drawing sign traslating matrix to signal position
        glTranslated(mySignPos.x(), mySignPos.y(), 0);

        // Define number of points (for efficiency)
        int noPoints = 9;

        // If the scale * exaggeration is more than 25, recalculate number of points
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        // scale matrix depending of the exaggeration
        glScaled(exaggeration, exaggeration, 1);

        // Set color of the externe circle
        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
        } else {
            GLHelper::setColor(RGBColor(76, 170, 50));
        }

        // Draw circle
        GLHelper::drawFilledCircle((double) 1.1, noPoints);

        // Traslate to front
        glTranslated(0, 0, .1);

        // Set color of the interne circle
        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectionColor);
        } else {
            GLHelper::setColor(RGBColor(255, 235, 0));
        }

        // draw another circle in the same position, but a little bit more small
        GLHelper::drawFilledCircle((double) 0.9, noPoints);

        // If the scale * exageration is equal or more than 4.5, draw H
        if (s.scale * exaggeration >= 4.5) {
            if (isAdditionalSelected()) {
                GLHelper::drawText("H", Position(), .1, 1.6, myViewNet->getNet()->selectedAdditionalColor, myBlockIconRotation);
            } else {
                GLHelper::drawText("H", Position(), .1, 1.6, RGBColor(76, 170, 50, 255), myBlockIconRotation);
            }
        }

        // pop draw matrix
        glPopMatrix();

        // Show Lock icon depending of the Edit mode
        drawLockIcon();
    }

    // pop draw matrix
    glPopMatrix();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    if (s.addFullName.show && myName != "") {
        GLHelper::drawText(myName, mySignPos, GLO_MAX - getType(), s.addFullName.size / s.scale, s.addFullName.color, myBlockIconRotation);
    }
    **/
}


std::string
GNEParkingSpace::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_Z:
            return toString(myZ);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_ANGLE:
            return toString(myAngle);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_Z:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingSpace::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_Z:
            return canParse<double>(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
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
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            break;
        case SUMO_ATTR_Z:
            myZ = parse<double>(value);
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_ANGLE:
            myAngle = parse<double>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}


/****************************************************************************/
