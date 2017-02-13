/****************************************************************************/
/// @file    GNEDetectorE2.cpp
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

#include "GNEDetectorE2.h"
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

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNEViewNet* viewNet, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename,
                             bool cont, const SUMOReal timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold) :
    GNEDetector(id, viewNet, SUMO_TAG_E2DETECTOR, ICON_E2, lane, pos, freq, filename),
    myLength(length),
    myCont(cont),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold) {
    // Update geometry;
    updateGeometry();
    // Set Colors
    myBaseColor = RGBColor(0, 204, 204, 255);
    myBaseColorSelected = RGBColor(125, 204, 204, 255);
}


GNEDetectorE2::~GNEDetectorE2() {
}


void
GNEDetectorE2::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // Get shape of lane parent
    myShape = myLane->getShape();

    // Cut shape using as delimitators myPos and their length (myPos + length)
    myShape = myShape.getSubpart(myLane->getPositionRelativeToParametricLenght(myPosition.x()), myLane->getPositionRelativeToParametricLenght(myPosition.x() + myLength));

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

    // Set offset of logo
    myDetectorLogoOffset = Position(0.5, 0);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(-0.5, 0);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


Position
GNEDetectorE2::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x()));
}


void
GNEDetectorE2::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition.x());
    device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
    if (!myFilename.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    device.writeAttr(SUMO_ATTR_CONT, myCont);
    device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, myTimeThreshold);
    device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, myJamThreshold);
    if (myBlocked) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlocked);
    }
    // Close tag
    device.closeTag();
}


void
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origing
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

    // Pop last matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        // Draw icon
        this->drawDetectorIcon(GUITextureSubSys::getTexture(GNETEXTURE_E2));

        // Show Lock icon depending of the Edit mode
        drawLockIcon();
    }

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // Pop name
    glPopName();
}


std::string
GNEDetectorE2::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return toString(myLane->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosition.x());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_CONT:
            return toString(myCont);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return toString(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_CONT:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_LENGTH:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_CONT:
            return canParse<bool>(value);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return canParse<SUMOReal>(value);
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
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_LENGTH:
            myLength = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_CONT:
            myCont = parse<bool>(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<SUMOReal>(value);
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
