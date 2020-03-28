/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNERouteProbe.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
///
//
/****************************************************************************/
#include <config.h>

#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNERouteProbe.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERouteProbe::GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, const std::string& frequency, 
        const std::string& name, const std::string& filename, SUMOTime begin) :
    GNEAdditional(id, viewNet, GLO_ROUTEPROBE, SUMO_TAG_ROUTEPROBE, name, false, 
        {edge}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myFrequency(frequency),
    myFilename(filename),
    myBegin(begin),
    myRelativePositionY(0) {
}


GNERouteProbe::~GNERouteProbe() {
}


void
GNERouteProbe::updateGeometry() {
    // obtain relative position of routeProbe in edge
    myRelativePositionY = 2 * getParentEdges().front()->getRouteProbeRelativePosition(this);

    // get lanes of edge
    GNELane* firstLane = getParentEdges().front()->getLanes().at(0);

    // Get shape of parent lane
    const double offset = firstLane->getLaneShape().length() < 0.5 ? firstLane->getLaneShape().length() : 0.5;

    // update geometry
    myAdditionalGeometry.updateGeometry(firstLane, offset);

    // Set block icon position
    myBlockIcon.position = myAdditionalGeometry.getShape().getLineCenter();

    // Set offset of the block icon
    myBlockIcon.offset = Position(1.1, (-3.06) - myRelativePositionY);

    // Set block icon rotation, and using their rotation for logo
    myBlockIcon.setRotation(firstLane);

    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


void
GNERouteProbe::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(),
                                          myAdditionalGeometry.getPosition(),
                                          myAdditionalGeometry.getRotation(),
                                          myViewNet->getVisualisationSettings().additionalSettings.routeProbeSize,
                                          myViewNet->getVisualisationSettings().additionalSettings.routeProbeSize);
}


Position
GNERouteProbe::getPositionInView() const {
    if (getParentEdges().front()->getLanes().front()->getLaneShape().length() < 0.5) {
        return getParentEdges().front()->getLanes().front()->getLaneShape().front();
    } else {
        Position A = getParentEdges().front()->getLanes().front()->getLaneShape().positionAtOffset(0.5);
        Position B = getParentEdges().front()->getLanes().back()->getLaneShape().positionAtOffset(0.5);
        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


Boundary
GNERouteProbe::getCenteringBoundary() const {
    return myAdditionalGeometry.getShape().getBoxBoundary().grow(10);
}


void
GNERouteProbe::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
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
    return getParentEdges().front()->getID();
}


void
GNERouteProbe::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double routeProbeExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (s.drawAdditionals(routeProbeExaggeration) && myViewNet->getDataViewOptions().showAdditionals()) {
        // get values
        const double width = (double) 2.0 * s.scale;
        const int numberOfLanes = int(getParentEdges().front()->getLanes().size());
        // start drawing
        glPushName(getGlID());
        glLineWidth(1.0);
        // set color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.additionalSettings.routeProbeColor);
        }
        // draw shape
        glPushMatrix();
        glTranslated(0, 0, getType());
        glTranslated(myAdditionalGeometry.getPosition().x(), myAdditionalGeometry.getPosition().y(), 0);
        glRotated(myAdditionalGeometry.getRotation(), 0, 0, 1);
        glScaled(routeProbeExaggeration, routeProbeExaggeration, 1);
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
        if ((width * routeProbeExaggeration > 1) && !s.drawForRectangleSelection) {
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
        glTranslated(myAdditionalGeometry.getPosition().x(), myAdditionalGeometry.getPosition().y(), getType());
        glRotated(myAdditionalGeometry.getRotation(), 0, 0, 1);
        glTranslated((-2.56) - myRelativePositionY, (-1.6), 0);
        // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
        if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, routeProbeExaggeration)) {
            glColor3d(1, 1, 1);
            glRotated(-90, 0, 0, 1);
            if (drawUsingSelectColor()) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBESELECTED), s.additionalSettings.routeProbeSize);
            } else {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_ROUTEPROBE), s.additionalSettings.routeProbeSize);
            }
        } else {
            GLHelper::setColor(s.additionalSettings.routeProbeColor);
            GLHelper::drawBoxLine(Position(0, s.additionalSettings.routeProbeSize), 0, 2 * s.additionalSettings.routeProbeSize, s.additionalSettings.routeProbeSize);
        }
        // Pop logo matrix
        glPopMatrix();
        // Show Lock icon depending of the Edit mode
        myBlockIcon.drawIcon(s, routeProbeExaggeration, 0.4);
        // draw name
        drawName(getPositionInView(), s.scale, s.addName);
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GNEGeometry::drawShapeDottedContour(s, getType(), routeProbeExaggeration, myDottedGeometry);
        }
        // pop name
        glPopName();
    }
}


std::string
GNERouteProbe::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
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
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERouteProbe::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(myBegin);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERouteProbe::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_EDGE:
            replaceParentEdges(this, value);
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
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
