/****************************************************************************/
/// @file    GNEVariableSpeedSignal.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/geom/GeomConvHelper.h>
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

#include "GNEVariableSpeedSignal.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEVariableSpeedSignalDialog.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignal::GNEVariableSpeedSignal(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNELane*> lanes, const std::string& filename, const std::map<SUMOTime, SUMOReal>& VSSValues, bool blocked) :
    GNEAdditionalSet(id, viewNet, pos, SUMO_TAG_VSS, blocked, std::vector<GNEAdditional * >(), std::vector<GNEEdge * >(), lanes),
    myFilename(filename),
    myVSSValues(VSSValues),
    mySaveInFilename(false) {
    // Update geometry;
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(76, 170, 50, 255);
    myBaseColorSelected = RGBColor(161, 255, 135, 255);
}


GNEVariableSpeedSignal::~GNEVariableSpeedSignal() {
}


void
GNEVariableSpeedSignal::updateGeometry() {
    // Clear shape
    myShape.clear();

    // Set block icon position
    myBlockIconPosition = myPosition;

    // Set block icon offset
    myBlockIconOffset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation();

    // Set position
    myShape.push_back(myPosition);

    // Add shape of childs (To avoid graphics errors)
    for (childLanes::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        myShape.append(i->lane->getShape());
    }

    // Update connections
    updateConnections();
}


Position
GNEVariableSpeedSignal::getPositionInView() const {
    return myPosition;
}


void
GNEVariableSpeedSignal::openAdditionalDialog() {
    GNEVariableSpeedSignalDialog variableSpeedSignalDialog(this);
}


void
GNEVariableSpeedSignal::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList* undoList) {
    // if item isn't blocked
    if (myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void
GNEVariableSpeedSignal::writeAdditional(OutputDevice& device, const std::string& currentDirectory) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANES, joinToString(getLaneChildIds(), " ").c_str());
    device.writeAttr(SUMO_ATTR_X, myPosition.x());
    device.writeAttr(SUMO_ATTR_Y, myPosition.y());
    // If filenam isn't empty and save in filename is enabled, save in a different file. In other case, save in the same additional XML
    if (!myFilename.empty() && mySaveInFilename == true) {
        // Write filename attribute
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
        // Save values in a different file
        OutputDevice& deviceVSS = OutputDevice::getDevice(currentDirectory + myFilename);
        deviceVSS.openTag("VSS");
        for (std::map<SUMOTime, SUMOReal>::const_iterator i = myVSSValues.begin(); i != myVSSValues.end(); ++i) {
            // Open VSS tag
            deviceVSS.openTag(SUMO_TAG_STEP);
            // Write TimeSTep
            deviceVSS.writeAttr(SUMO_ATTR_TIME, i->first);
            // Write speed
            deviceVSS.writeAttr(SUMO_ATTR_SPEED, i->second);
            // Close VSS tag
            deviceVSS.closeTag();
        }
        deviceVSS.close();
    } else {
        for (std::map<SUMOTime, SUMOReal>::const_iterator i = myVSSValues.begin(); i != myVSSValues.end(); ++i) {
            // Open VSS tag
            device.openTag(SUMO_TAG_STEP);
            // Write TimeSTep
            device.writeAttr(SUMO_ATTR_TIME, i->first);
            // Write speed
            device.writeAttr(SUMO_ATTR_SPEED, i->second);
            // Close VSS tag
            device.closeTag();
        }
    }
    // Close tag
    device.closeTag();
}


std::string
GNEVariableSpeedSignal::getFilename() const {
    return myFilename;
}


std::map<SUMOTime, SUMOReal>
GNEVariableSpeedSignal::getVariableSpeedSignalSteps() const {
    return myVSSValues;
}


void
GNEVariableSpeedSignal::setFilename(std::string filename) {
    myFilename = filename;
}


void
GNEVariableSpeedSignal::setVariableSpeedSignalSteps(const std::map<SUMOTime, SUMOReal>& vssValues) {
    myVSSValues = vssValues;
}


bool
GNEVariableSpeedSignal::insertStep(const SUMOTime time, const SUMOReal speed) {
    if (myVSSValues.find(time) == myVSSValues.end()) {
        myVSSValues[time] = speed;
        return true;
    } else {
        return false;
    }
}


const std::string&
GNEVariableSpeedSignal::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNEVariableSpeedSignal::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);

    // Draw icon depending of rerouter is or isn't selected
    if (isAdditionalSelected()) {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getGif(GNETEXTURE_VARIABLESPEEDSIGNALSELECTED), 1);
    } else {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getGif(GNETEXTURE_VARIABLESPEEDSIGNAL), 1);
    }

    // Pop draw icon matrix
    glPopMatrix();

    // Show Lock icon depending of the Edit mode
    drawLockIcon(0.4);

    // Push matrix to draw every symbol over lane
    glPushMatrix();

    // Traslate to 0,0
    glTranslated(0, 0, getType());

    // Obtain exaggeration
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // Iterate over lanes
    for (childLanes::const_iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Draw every signal over Lane
        glPushMatrix();
        glScaled(exaggeration, exaggeration, 1);
        glTranslated(i->positionOverLane.x(), i->positionOverLane.y(), 0);
        glRotated(i->rotationOverLane, 0, 0, 1);
        glTranslated(0, -1.5, 0);

        int noPoints = 9;
        if (s.scale > 25) {
            noPoints = (int)(9.0 + s.scale / 10.0);
            if (noPoints > 36) {
                noPoints = 36;
            }
        }
        glColor3d(1, 0, 0);
        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
        if (s.scale >= 5) {
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
            // Draw speed
            SUMOReal speed = i->lane->getSpeed();
            // Show as Km/h
            speed *= 3.6f;
            if (((int) speed + 1) % 10 == 0) {
                speed = (SUMOReal)(((int) speed + 1) / 10 * 10);
            }
            // draw the speed string
            std::string speedToDraw = toString<SUMOReal>(speed);
            glColor3d(1, 1, 0);
            glTranslated(0, 0, .1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.2f);
            SUMOReal w = pfdkGetStringWidth(speedToDraw.c_str());
            glRotated(180, 0, 1, 0);
            glTranslated(-w / 2., 0.3, 0);
            pfDrawString(speedToDraw.c_str());
        }
        glPopMatrix();
    }

    // Pop symbol matrix
    glPopMatrix();

    // Draw connections
    drawConnections();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // Pop name
    glPopName();
}


std::string
GNEVariableSpeedSignal::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANES:
            return joinToString(getLaneChildIds(), " ");
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FILE:
            return myFilename;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEVariableSpeedSignal::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FILE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEVariableSpeedSignal::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        case SUMO_ATTR_LANES: {
            std::vector<std::string> laneIds;
            SUMOSAXAttributes::parseStringVector(value, laneIds);
            // Empty Lanes aren't valid
            if (laneIds.empty()) {
                return false;
            }
            // Iterate over parsed lanes
            for (int i = 0; i < (int)laneIds.size(); i++) {
                if (myViewNet->getNet()->retrieveLane(laneIds.at(i), false) == NULL) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEVariableSpeedSignal::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_LANES: {
            // Declare variables
            std::vector<std::string> laneIds;
            std::vector<GNELane*> lanes;
            GNELane* lane;
            SUMOSAXAttributes::parseStringVector(value, laneIds);
            // Iterate over parsed lanes and obtain pointer to lanes
            for (int i = 0; i < (int)laneIds.size(); i++) {
                lane = myViewNet->getNet()->retrieveLane(laneIds.at(i), false);
                if (lane) {
                    lanes.push_back(lane);
                }
            }
            // Set new childs
            setLaneChilds(lanes);
            break;
        }
        case SUMO_ATTR_POSITION:
            bool ok;
            myPosition = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false)[0];
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/