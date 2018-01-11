/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorE2.cpp
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

#include "GNEDetectorE2.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEEdge.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos, double length, double freq, const std::string& filename,
                             bool cont, const double timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos) :
    GNEDetector(id, viewNet, SUMO_TAG_E2DETECTOR, ICON_E2, lane, pos, freq, filename, friendlyPos, NULL),
    myRelativeLength(length / lane->getLaneParametricLength()),
    myCont(cont),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold) {
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

    // Cut shape using as delimitators fixed start position and fixed end position
    double startPosFixed = (myPositionOverLane < 0) ? 0 : myPositionOverLane;
    double endPosFixed = ((myPositionOverLane + myRelativeLength) > 1) ? 1 : (myPositionOverLane + myRelativeLength);
    myShape = myShape.getSubpart(startPosFixed * myShape.length(), endPosFixed * myShape.length());

    // Get number of parts of the shape
    int numberOfSegments = (int)myShape.size() - 1;

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
            myShapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(-0.75, 0);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation(myLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


void
GNEDetectorE2::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_POSITION, getAbsolutePositionOverLane());
    device.writeAttr(SUMO_ATTR_LENGTH, myRelativeLength * myLane->getLaneParametricLength());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
    if (!myFilename.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    device.writeAttr(SUMO_ATTR_CONT, myCont);
    device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, myTimeThreshold);
    device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, myJamThreshold);
    device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    // Close tag
    device.closeTag();
}


bool GNEDetectorE2::isDetectorPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        // floors are needed to avoid precision problems
        return ((floor(myPositionOverLane * 1000) / 1000) >= 0) && ((floor((myPositionOverLane + myRelativeLength) * 1000) / 1000) <= 1);
    }
}


double GNEDetectorE2::getAbsoluteLenght() const {
    return myRelativeLength * myLane->getLaneParametricLength();
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
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(RGBColor(0, 204, 204));
    }

    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s);

    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Pop last matrix
    glPopMatrix();

    // Check if the distance is enougth to draw details
    if (s.scale * exaggeration >= 10) {
        // Push matrix
        glPushMatrix();
        // Traslate to center of detector
        glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        //move to logo position
        glTranslated(-0.75, 0, 0);
        // draw E2 logo
        if (isAdditionalSelected()) {
            GLHelper::drawText("E2", Position(), .1, 1.5, myViewNet->getNet()->selectionColor);
        } else {
            GLHelper::drawText("E2", Position(), .1, 1.5, RGBColor::BLACK);
        }
        // pop matrix
        glPopMatrix();

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
            return myLane->getID();
        case SUMO_ATTR_POSITION:
            return toString(getAbsolutePositionOverLane());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_LENGTH:
            return toString(myRelativeLength * myLane->getLaneParametricLength());
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
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
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
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_FREQUENCY:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_LENGTH:
            if (canParse<double>(value)) {
                // obtain relative new start position
                double newLength = parse<double>(value);
                // lengths withs size 0 aren't valid
                if (newLength <= 0) {
                    return false;
                } else {
                    newLength = newLength / myLane->getLaneParametricLength();
                    if ((myPositionOverLane + newLength) > 1) {
                        return false;
                    } else {
                        return true;
                    }
                }
            }
        case SUMO_ATTR_FILE:
            return isValidFilename(value);
        case SUMO_ATTR_CONT:
            return canParse<bool>(value);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_FRIENDLY_POS:
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
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myRelativeLength = parse<double>(value) / myLane->getLaneParametricLength();
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_CONT:
            myCont = parse<bool>(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);;
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
