/****************************************************************************/
/// @file    GNEContainerStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
/// A lane area vehicles can halt at (GNE version)
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
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEContainerStop.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================

GNEContainerStop::GNEContainerStop(const std::string& id, GNELane* lane, GNEViewNet* viewNet, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines) :
    GNEStoppingPlace(id, viewNet, SUMO_TAG_CONTAINER_STOP, ICON_CONTAINERSTOP, lane, startPos, endPos),
    myLines(lines) {
    // When a new additional element is created, updateGeometry() must be called
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(83, 89, 172, 255);
    myBaseColorSelected = RGBColor(103, 109, 192, 255);
    mySignColor = RGBColor(177, 184, 186, 171);
    mySignColorSelected = RGBColor(197, 204, 206, 171);
    myTextColor = RGBColor(83, 89, 172, 255);
    myTextColorSelected = RGBColor(103, 109, 192, 255);
}


GNEContainerStop::~GNEContainerStop() {}


void
GNEContainerStop::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myShape = myLane->getShape();

    // Move shape to side
    myShape.move2side(1.65 * offsetSign);

    // Cut shape using as delimitators from start position and end position
    myShape = myShape.getSubpart(myLane->getPositionRelativeToParametricLenght(myStartPos), myLane->getPositionRelativeToParametricLenght(myEndPos));

    // Get number of parts of the shape
    int numberOfSegments = (int) myShape.size() - 1;

    // If number of segments is more than 0
    if (numberOfSegments >= 0) {

        // Reserve memory (To improve efficiency)
        myShapeRotations.reserve(numberOfSegments);
        myShapeLengths.reserve(numberOfSegments);

        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {

            // Obtain first position
            const Position& f = myShape[i];

            // Obtain next position
            const Position& s = myShape[i + 1];

            // Save distance between position into myShapeLengths
            myShapeLengths.push_back(f.distanceTo(s));

            // Save rotation (angle) of the vector constructed by points f and s
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }

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
    myViewNet->getNet()->refreshAdditional(this);
}


void
GNEContainerStop::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
    if (myLines.size() > 0) {
        device.writeAttr(SUMO_ATTR_LINES, getAttribute(SUMO_ATTR_LINES));
    }
    if (myBlocked) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlocked);
    }
    // Close tag
    device.closeTag();
}


std::vector<std::string>
GNEContainerStop::getLines() const {
    return myLines;
}


void
GNEContainerStop::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType());

    // Set color of the base
    if (isAdditionalSelected()) {
        GLHelper::setColor(myBaseColorSelected);
    } else {
        GLHelper::setColor(myBaseColor);
    }

    // Obtain exaggeration of the draw
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // Draw the area using shape, shapeRotations, shapeLenghts and value of exaggeration
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {

        // Add a draw matrix
        glPushMatrix();

        // Obtain rotation of the sing depeding of the option "lefthand"
        SUMOReal rotSign = OptionsCont::getOptions().getBool("lefthand");

        // Set color of the lines
        if (isAdditionalSelected()) {
            GLHelper::setColor(myTextColorSelected);
        } else {
            GLHelper::setColor(myTextColor);
        }

        // Iterate over every line
        for (int i = 0; i < (int)myLines.size(); ++i) {
            // Add a new push matrix
            glPushMatrix();

            // Traslate End positionof signal
            glTranslated(mySignPos.x(), mySignPos.y(), 0);

            // Rotate 180 (Eje X -> Mirror)
            glRotated(180, 1, 0, 0);

            // Rotate again depending of the option rotSign
            glRotated(rotSign * myBlockIconRotation, 0, 0, 1);

            // Set poligon mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // set polyfront position ot 0
            pfSetPosition(0, 0);

            // Set polyfront scale to 1
            pfSetScale(1.f);

            // traslate matrix for every line
            glTranslated(1.2, -(double)i, 0);

            // draw line
            pfDrawString(myLines[i].c_str());

            // pop matrix
            glPopMatrix();
        }

        // Start drawing sign traslating matrix to signal position
        glTranslated(mySignPos.x(), mySignPos.y(), 0);

        // Define nº points (for efficiency)
        int noPoints = 9;

        // If the scale * exaggeration is more than 25, recalculate nº points
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        // scale matrix depending of the exaggeration
        glScaled(exaggeration, exaggeration, 1);

        // Draw green circle
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);

        // Traslate to front
        glTranslated(0, 0, .1);

        // Set color of the lines
        if (isAdditionalSelected()) {
            GLHelper::setColor(mySignColorSelected);
        } else {
            GLHelper::setColor(mySignColor);
        }

        // draw another circle in the same position, but a little bit more small
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        // If the scale * exageration is equal or more than 4.5, draw H
        if (s.scale * exaggeration >= 4.5) {
            if (isAdditionalSelected()) {
                GLHelper::drawText("C", Position(), .1, 1.6, myBaseColorSelected, myBlockIconRotation);
            } else {
                GLHelper::drawText("C", Position(), .1, 1.6, myBaseColor, myBlockIconRotation);
            }
        }

        // pop draw matrix
        glPopMatrix();

        // Show Lock icon depending of the Edit mode
        drawLockIcon();
    }

    // pop draw matrix
    glPopMatrix();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // Pop name
    glPopName();
}


std::string
GNEContainerStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return toString(myLane->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(myStartPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEContainerStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_LINES:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEContainerStop::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_STARTPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) < (myEndPos - 1));
        case SUMO_ATTR_ENDPOS: {
            if (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 1 && parse<SUMOReal>(value) > myStartPos) {
                // If extension is larger than Lane
                if (parse<SUMOReal>(value) > myLane->getLaneParametricLenght()) {
                    // Ask user if want to assign the lenght of lane as endPosition
                    FXuint answer = FXMessageBox::question(getViewNet()->getApp(), MBOX_YES_NO,
                                                           (toString(SUMO_ATTR_ENDPOS) + " exceeds the size of the " + toString(SUMO_TAG_LANE)).c_str(), "%s",
                                                           (toString(SUMO_ATTR_ENDPOS) + " exceeds the size of the " + toString(SUMO_TAG_LANE) +
                                                            ". Do you want to assign the lenght of the " + toString(SUMO_TAG_LANE) + " as " + toString(SUMO_ATTR_ENDPOS) + "?").c_str());
                    if (answer == 1) { //1:yes, 2:no, 4:esc
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_LINES:
            return isValidStringVector(value);
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
GNEContainerStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            changeLane(value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPos = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_ENDPOS:
            if (parse<SUMOReal>(value) > myLane->getLaneParametricLenght()) {
                myEndPos = myLane->getLaneParametricLenght();
            } else {
                myEndPos = parse<SUMOReal>(value);
            }
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_LINES:
            myLines.clear();
            SUMOSAXAttributes::parseStringVector(value, myLines);
            getViewNet()->update();
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
