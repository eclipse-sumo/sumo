/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <netedit/GNEViewNet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNERouteProbe.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbe::GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, const std::string& frequency, const std::string& name, const std::string& filename, SUMOTime begin) :
    GNEAdditional(id, viewNet, GLO_ROUTEPROBE, SUMO_TAG_ROUTEPROBE, name, false, {
    edge
}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
myFrequency(frequency),
myFilename(filename),
myBegin(begin),
myRelativePositionY(0) {
}


GNERouteProbe::~GNERouteProbe() {
}


void
GNERouteProbe::updateGeometry() {
    // Clear all containers
    myGeometry.clearGeometry();

    // obtain relative position of routeProbe in edge
    myRelativePositionY = 2 * getEdgeParents().front()->getRouteProbeRelativePosition(this);

    // get lanes of edge
    GNELane* firstLane = getEdgeParents().front()->getLanes().at(0);

    // Get shape of lane parent
    double offset = firstLane->getGeometry().shape.length() < 0.5 ? firstLane->getGeometry().shape.length() : 0.5;
    myGeometry.shape.push_back(firstLane->getGeometry().shape.positionAtOffset(offset));

    // Save rotation (angle) of the vector constructed by points f and s
    myGeometry.shapeRotations.push_back(firstLane->getGeometry().shape.rotationDegreeAtOffset(offset) * -1);

    // Set block icon position
    myBlockIcon.position = myGeometry.shape.getLineCenter();

    // Set offset of the block icon
    myBlockIcon.offset = Position(1.1, (-3.06) - myRelativePositionY);

    // Set block icon rotation, and using their rotation for logo
    myBlockIcon.setRotation(firstLane);
}


Position
GNERouteProbe::getPositionInView() const {
    if (getEdgeParents().front()->getLanes().front()->getGeometry().shape.length() < 0.5) {
        return getEdgeParents().front()->getLanes().front()->getGeometry().shape.front();
    } else {
        Position A = getEdgeParents().front()->getLanes().front()->getGeometry().shape.positionAtOffset(0.5);
        Position B = getEdgeParents().front()->getLanes().back()->getGeometry().shape.positionAtOffset(0.5);
        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


Boundary
GNERouteProbe::getCenteringBoundary() const {
    return myGeometry.shape.getBoxBoundary().grow(10);
}


void
GNERouteProbe::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNERouteProbe::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


std::string
GNERouteProbe::getParentName() const {
    return getEdgeParents().front()->getMicrosimID();
}


void
GNERouteProbe::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    const double width = (double) 2.0 * s.scale;
    const double exaggeration = s.addSize.getExaggeration(s, this);
    const int numberOfLanes = int(getEdgeParents().front()->getLanes().size());
    // start drawing
    glPushName(getGlID());
    glLineWidth(1.0);
    // set color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.colorSettings.routeProbe);
    }
    // draw shape
    glPushMatrix();
    glTranslated(0, 0, getType());
    glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), 0);
    glRotated(myGeometry.shapeRotations[0], 0, 0, 1);
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
    if ((width * exaggeration > 1) && !s.drawForSelecting) {
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectionColor);
        } else {
            GLHelper::setColor(RGBColor::WHITE);
        }
        glRotated(90, 0, 0, -1);
        glBegin(GL_LINES);
        glVertex2d(0, 0);
        glVertex2d(0, (numberOfLanes * 3.3));
        glEnd();
    }
    // Pop shape matrix
    glPopMatrix();
    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), getType());
    glRotated(myGeometry.shapeRotations[0], 0, 0, 1);
    glTranslated((-2.56) - myRelativePositionY, (-1.6), 0);
    // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
    if (!s.drawForSelecting && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
        glColor3d(1, 1, 1);
        glRotated(-90, 0, 0, 1);
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBESELECTED), 1);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBE), 1);
        }
    } else {
        GLHelper::setColor(s.colorSettings.routeProbe);
        GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
    }
    // Pop logo matrix
    glPopMatrix();
    // Show Lock icon depending of the Edit mode
    myBlockIcon.drawIcon(s, exaggeration, 0.4);
    // draw name
    drawName(getPositionInView(), s.scale, s.addName);
    // check if dotted contour has to be drawn
    if (myViewNet->getDottedAC() == this) {
        GLHelper::drawShapeDottedContourRectangle(s, getType(), myGeometry.shape[0], 2, 2, myGeometry.shapeRotations[0], (-2.56) - myRelativePositionY, -1.6);
    }
    // pop name
    glPopName();
}


std::string
GNERouteProbe::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_BEGIN:
            return time2string(myBegin);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_BEGIN:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERouteProbe::getPopUpID() const {
    return getTagStr();
}


std::string
GNERouteProbe::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN);
}

// ===========================================================================
// private
// ===========================================================================

bool
GNERouteProbe::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_FREQUENCY:
            if (value.empty()) {
                return true;
            } else {
                return canParse<SUMOTime>(value);
            }
        case SUMO_ATTR_BEGIN:
            return canParse<SUMOTime>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            changeEdgeParents(this, value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = value;
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<SUMOTime>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
