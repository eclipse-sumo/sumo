/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEParkingArea.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// A lane area vehicles can park at (GNE version)
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
#include <foreign/fontstash/fontstash.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>

#include "GNEParkingArea.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingArea::GNEParkingArea(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double startPos, double endPos, const std::string& name, bool friendlyPosition, double width, double length, double angle) :
    GNEStoppingPlace(id, viewNet, SUMO_TAG_PARKING_AREA, ICON_PARKINGAREA, lane, startPos, endPos, name, friendlyPosition),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
}


GNEParkingArea::~GNEParkingArea() {}


void
GNEParkingArea::updateGeometry() {
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

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


void
GNEParkingArea::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, getAbsoluteStartPosition());
    device.writeAttr(SUMO_ATTR_ENDPOS, getAbsoluteEndPosition());
    if (myName.empty() == false) {
        device.writeAttr(SUMO_ATTR_NAME, myName);
    }
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    // Close tag
    device.closeTag();
}


void
GNEParkingArea::drawGL(const GUIVisualizationSettings& s) const {
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
                GLHelper::drawText("P", Position(), .1, 1.6, myViewNet->getNet()->selectedAdditionalColor, myBlockIconRotation);
            } else {
                GLHelper::drawText("P", Position(), .1, 1.6, RGBColor(76, 170, 50, 255), myBlockIconRotation);
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
}


std::string
GNEParkingArea::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return myLane->getID();
        case SUMO_ATTR_STARTPOS:
            return toString(getAbsoluteStartPosition());
        case SUMO_ATTR_ENDPOS:
            return toString(getAbsoluteEndPosition());
        case SUMO_ATTR_NAME:
            return myName;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
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
GNEParkingArea::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FRIENDLY_POS:
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
GNEParkingArea::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            if (canParse<double>(value)) {
                // Check that new start Position is smaller that end position
                return ((parse<double>(value) / myLane->getLaneParametricLength()) < myEndPosRelative);
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            if (canParse<double>(value)) {
                // Check that new end Position is larger that end position
                return ((parse<double>(value) / myLane->getLaneParametricLength()) > myStartPosRelative);
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return true;
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
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
GNEParkingArea::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            myLane = changeLane(myLane, value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPosRelative = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosRelative = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_NAME:
            myName = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
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
