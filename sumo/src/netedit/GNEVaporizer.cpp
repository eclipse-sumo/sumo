/****************************************************************************/
/// @file    GNEVaporizer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
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

#include "GNEViewNet.h"
#include "GNEVaporizer.h"
#include "GNEEdge.h"
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

GNEVaporizer::GNEVaporizer(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, SUMOReal startTime, SUMOReal end) :
    GNEAdditional(id, viewNet, Position(), SUMO_TAG_VAPORIZER, ICON_VAPORIZER),
    myStartTime(startTime),
    myEnd(end) {
    // This additional belongs to a edge
    myEdge = edge;
    // this additional ISN'T movable
    myMovable = false;
    // Update geometry;
    updateGeometry();
    // Center view in the position of Vaporizer
    myViewNet->centerTo(getGlID(), false);
}


GNEVaporizer::~GNEVaporizer() {
}


void
GNEVaporizer::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // get lanes of edge
    GNELane* firstLane = myEdge->getLanes().at(0);

    // Get shape of lane parent
    myShape.push_back(firstLane->getShape().positionAtOffset(3));

    // Obtain first position
    Position f = myShape[0] - Position(1, 0);

    // Obtain next position
    Position s = myShape[0] + Position(1, 0);

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(firstLane->getShape().rotationDegreeAtOffset(5) * -1);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(1.1, -3.06);

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(firstLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


Position
GNEVaporizer::getPositionInView() const {
    Position A = myEdge->getLanes().front()->getShape().positionAtOffset(myPosition.x());
    Position B = myEdge->getLanes().back()->getShape().positionAtOffset(myPosition.x());

    // return Middle point
    return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
}


void
GNEVaporizer::moveAdditionalGeometry(SUMOReal, SUMOReal) {
    // This additional cannot be moved
}


void
GNEVaporizer::commmitAdditionalGeometryMoved(SUMOReal, SUMOReal, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNEVaporizer::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_EDGE, myEdge->getID());
    device.writeAttr(SUMO_ATTR_STARTTIME, myStartTime);
    device.writeAttr(SUMO_ATTR_END, myEnd);
    // Close tag
    device.closeTag();
}


SUMOReal
GNEVaporizer::getStartTime() const {
    return myStartTime;
}


SUMOReal
GNEVaporizer::getEnd() const {
    return myEnd;
}


void
GNEVaporizer::setStartTime(SUMOReal startTime) {
    myStartTime = startTime;
}


void
GNEVaporizer::setEndTime(SUMOReal end) {
    myEnd = end;
}


const std::string&
GNEVaporizer::getParentName() const {
    return myEdge->getMicrosimID();
}


void
GNEVaporizer::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    glLineWidth(1.0);

    // Declare auxiliar values
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    int numberOfLanes = int(myEdge->getLanes().size());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;

    // draw shape
    glColor3ub(120, 216, 0);
    glPushMatrix();
    glTranslated(0, 0, getType());
    glTranslated(myShape[0].x(), myShape[0].y(), 0);
    glRotated(myShapeRotations[0], 0, 0, 1);
    glScaled(exaggeration, exaggeration, 1);
    glTranslated(-1.6, -1.6, 0);
    glBegin(GL_QUADS);
    glVertex2d(0,  0.25);
    glVertex2d(0, -0.25);
    glVertex2d((numberOfLanes * 3.3), -0.25);
    glVertex2d((numberOfLanes * 3.3),  0.25);
    glEnd();
    glTranslated(0, 0, .01);
    glBegin(GL_LINES);
    glVertex2d(0, 0.25 - .1);
    glVertex2d(0, -0.25 + .1);
    glEnd();

    // position indicator (White)
    if (width * exaggeration > 1) {
        glRotated(90, 0, 0, -1);
        glColor3d(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 0);
        glVertex2d(0, (numberOfLanes * 3.3));
        glEnd();
    }

    // Pop shape matrix
    glPopMatrix();

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glRotated(myShapeRotations[0], 0, 0, 1);
    glTranslated(-2.56, - 1.6, 0);
    glColor3d(1, 1, 1);
    glRotated(-90, 0, 0, 1);

    // Draw icon depending of detector is or isn't selected
    if (isAdditionalSelected()) {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_VAPORIZERSELECTED), 1);
    } else {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_VAPORIZER), 1);
    }

    // Pop logo matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        // Show Lock icon depending of the Edit mode
        drawLockIcon(0.4);
    }

    // Finish draw
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


std::string
GNEVaporizer::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_STARTTIME:
            return toString(myStartTime);
        case SUMO_ATTR_END:
            return toString(myEnd);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVaporizer::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_STARTTIME:
        case SUMO_ATTR_END:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVaporizer::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_STARTTIME:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_END:
            return canParse<SUMOReal>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVaporizer::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            changeEdge(value);
            break;
        case SUMO_ATTR_STARTTIME:
            myStartTime = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<SUMOReal>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
