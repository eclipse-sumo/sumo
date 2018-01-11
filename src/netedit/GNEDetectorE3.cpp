/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorE3.cpp
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
#include <utils/geom/GeomConvHelper.h>
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
#include <utils/common/MsgHandler.h>

#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEEdge.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, double freq, const std::string& filename, const double timeThreshold, double speedThreshold) :
    GNEAdditional(id, viewNet, SUMO_TAG_E3DETECTOR, ICON_E3, true),
    myPosition(pos),
    myFreq(freq),
    myFilename(filename),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold) {
}


GNEDetectorE3::~GNEDetectorE3() {}


void
GNEDetectorE3::updateGeometry() {
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

    // Update connection's geometry
    updateChildConnections();

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position
GNEDetectorE3::getPositionInView() const {
    return myPosition;
}


void
GNEDetectorE3::moveGeometry(const Position& oldPos, const Position& offset) {
    // restore old position, apply offset and update Geometry
    myPosition = oldPos;
    myPosition.add(offset);
    updateGeometry();
}


void
GNEDetectorE3::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_X, toString(myPosition.x()), true, toString(oldPos.x())));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_Y, toString(myPosition.y()), true, toString(oldPos.y())));
    undoList->p_end();
}


void
GNEDetectorE3::writeAdditional(OutputDevice& device) const {
    // Only save E3 if have Entry/Exits
    if (myAdditionalChilds.size() > 0) {
        // Write parameters
        device.openTag(getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
        if (!myFilename.empty()) {
            device.writeAttr(SUMO_ATTR_FILE, myFilename);
        }
        device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, myTimeThreshold);
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
        device.writeAttr(SUMO_ATTR_X, myPosition.x());
        device.writeAttr(SUMO_ATTR_Y, myPosition.y());

        // Write entrys and exits
        for (auto i : myAdditionalChilds) {
            i->writeAdditional(device);
        }

        // Close E3 tag
        device.closeTag();
    } else {
        WRITE_WARNING(toString(getTag()) + " with ID '" + getID() + "' cannot be writed in additional file because doesn't have childs.");
    }
}


std::string
GNEDetectorE3::generateEntryID() {
    int counter = 0;
    while (myViewNet->getNet()->getAdditional(SUMO_TAG_DET_ENTRY, getID() + toString(SUMO_TAG_DET_ENTRY) + toString(counter)) != NULL) {
        counter++;
    }
    return (getID() + toString(SUMO_TAG_DET_ENTRY) + toString(counter));
}


std::string
GNEDetectorE3::generateExitID() {
    int counter = 0;
    while (myViewNet->getNet()->getAdditional(SUMO_TAG_DET_EXIT, getID() + toString(SUMO_TAG_DET_EXIT) + toString(counter)) != NULL) {
        counter++;
    }
    return (getID() + toString(SUMO_TAG_DET_EXIT) + toString(counter));
}


const std::string&
GNEDetectorE3::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNEDetectorE3::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    // Draw icon depending of detector is or isn't selected
    if (isAdditionalSelected()) {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_E3SELECTED), 1);
    } else {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_E3), 1);
    }

    // Pop logo matrix
    glPopMatrix();

    // Show Lock icon depending of the Edit mode
    drawLockIcon(0.4);

    // Draw connections
    drawChildConnections();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string
GNEDetectorE3::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_X:
            return toString(myPosition.x());
        case SUMO_ATTR_Y:
            return toString(myPosition.y());
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return toString(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            // change ID of Entry
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            // Change Ids of all Entry/Exits childs
            for (auto i : myAdditionalChilds) {
                if (i->getTag() == SUMO_TAG_ENTRY) {
                    i->setAttribute(SUMO_ATTR_ID, generateEntryID(), undoList);
                } else {
                    i->setAttribute(SUMO_ATTR_ID, generateExitID(), undoList);
                }
            }
            break;
        }
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_X:
        case SUMO_ATTR_Y:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE3::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_X:
            return canParse<double>(value);
        case SUMO_ATTR_Y:
            return canParse<double>(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFilename(value);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_X:
            myPosition.setx(parse<double>(value));
            break;
        case SUMO_ATTR_Y:
            myPosition.sety(parse<double>(value));
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<double>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
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
