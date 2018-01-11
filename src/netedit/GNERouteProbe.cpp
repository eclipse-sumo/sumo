/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteProbe.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
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

#include "GNEViewNet.h"
#include "GNERouteProbe.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbe::GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, double frequency, const std::string& filename, double begin) :
    GNEAdditional(id, viewNet, SUMO_TAG_ROUTEPROBE, ICON_ROUTEPROBE, false),
    myEdge(edge),
    myFrequency(frequency),
    myFilename(filename),
    myBegin(begin),
    myNumberOfLanes(0),
    myRelativePositionY(0) {
}


GNERouteProbe::~GNERouteProbe() {
}


void
GNERouteProbe::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    // obtain relative position of routeProbe in edge
    myRelativePositionY = 2 * myEdge->getRouteProbeRelativePosition(this);

    // get lanes of edge
    GNELane* firstLane = myEdge->getLanes().at(0);

    // Save number of lanes
    myNumberOfLanes = int(myEdge->getLanes().size());

    // Get shape of lane parent
    myShape.push_back(firstLane->getShape().positionAtOffset(0));

    // Obtain first position
    Position f = myShape[0] - Position(1, 0);

    // Obtain next position
    Position s = myShape[0] + Position(1, 0);

    // Save rotation (angle) of the vector constructed by points f and s
    myShapeRotations.push_back(firstLane->getShape().rotationDegreeAtOffset(0) * -1);

    // Set block icon position
    myBlockIconPosition = myShape.getLineCenter();

    // Set offset of the block icon
    myBlockIconOffset = Position(1.1, (-3.06) - myRelativePositionY);

    // Set block icon rotation, and using their rotation for logo
    setBlockIconRotation(firstLane);

    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position
GNERouteProbe::getPositionInView() const {
    Position A = myEdge->getLanes().front()->getShape().positionAtOffset(5);
    Position B = myEdge->getLanes().back()->getShape().positionAtOffset(5);

    // return Middle point
    return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
}


void
GNERouteProbe::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void
GNERouteProbe::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}

void
GNERouteProbe::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_EDGE, myEdge->getID());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    if (!myFilename.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    device.writeAttr(SUMO_ATTR_BEGIN, myBegin);
    // Close tag
    device.closeTag();
}


const std::string&
GNERouteProbe::getFilename() const {
    return myFilename;
}


double
GNERouteProbe::getFrequency() const {
    return myFrequency;
}


double
GNERouteProbe::getBegin() const {
    return myBegin;
}


void
GNERouteProbe::setFilename(const std::string& filename) {
    myFilename = filename;
}


void
GNERouteProbe::setFrequency(double frequency) {
    myFrequency = frequency;
}


void
GNERouteProbe::setBegin(double begin) {
    myBegin = begin;
}


const std::string&
GNERouteProbe::getParentName() const {
    return myEdge->getMicrosimID();
}


void
GNERouteProbe::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    glPushName(getGlID());
    double width = (double) 2.0 * s.scale;
    glLineWidth(1.0);
    const double exaggeration = s.addSize.getExaggeration(s);

    // set color
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(RGBColor(255, 216, 0));
    }
    // draw shape
    glPushMatrix();
    glTranslated(0, 0, getType());
    glTranslated(myShape[0].x(), myShape[0].y(), 0);
    glRotated(myShapeRotations[0], 0, 0, 1);
    glScaled(exaggeration, exaggeration, 1);
    glTranslated(-1.6, -1.6, 0);
    glBegin(GL_QUADS);
    glVertex2d(0,  0.25);
    glVertex2d(0, -0.25);
    glVertex2d((myNumberOfLanes * 3.3), -0.25);
    glVertex2d((myNumberOfLanes * 3.3),  0.25);
    glEnd();
    glTranslated(0, 0, .01);
    glBegin(GL_LINES);
    glVertex2d(0, 0.25 - .1);
    glVertex2d(0, -0.25 + .1);
    glEnd();

    // position indicator (White)
    if (width * exaggeration > 1) {
        if (isAdditionalSelected()) {
            GLHelper::setColor(myViewNet->getNet()->selectionColor);
        } else {
            GLHelper::setColor(RGBColor::WHITE);
        }
        glRotated(90, 0, 0, -1);
        glBegin(GL_LINES);
        glVertex2d(0, 0);
        glVertex2d(0, (myNumberOfLanes * 3.3));
        glEnd();
    }

    // Pop shape matrix
    glPopMatrix();

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glRotated(myShapeRotations[0], 0, 0, 1);
    glTranslated((-2.56) - myRelativePositionY, (-1.6), 0);
    glColor3d(1, 1, 1);
    glRotated(-90, 0, 0, 1);

    // Draw icon depending of detector is or isn't selected
    if (isAdditionalSelected()) {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBESELECTED), 1);
    } else {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBE), 1);
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
GNERouteProbe::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_BEGIN:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERouteProbe::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_FILE:
            return isValidFilename(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            myEdge = changeEdge(myEdge, value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<double>(value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<double>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}

/****************************************************************************/
