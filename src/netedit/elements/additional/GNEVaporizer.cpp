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
/// @file    GNEVaporizer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEVaporizer.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVaporizer::GNEVaporizer(GNEViewNet* viewNet, GNEEdge* edge, SUMOTime begin, SUMOTime end, const std::string& name) :
    GNEAdditional(edge->getID(), viewNet, GLO_VAPORIZER, SUMO_TAG_VAPORIZER, name, false, 
        {edge}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myBegin(begin),
    myEnd(end) {
}


GNEVaporizer::~GNEVaporizer() {
}


void
GNEVaporizer::updateGeometry() {
    // get lanes of edge
    GNELane* firstLane = getParentEdges().front()->getLanes().at(0);

    // Get shape of parent lane
    const double offset = firstLane->getLaneShape().length() < 2.5 ? firstLane->getLaneShape().length() : 2.5;

    // update geometry
    myAdditionalGeometry.updateGeometry(firstLane, offset);

    // Set block icon position
    myBlockIcon.position = myAdditionalGeometry.getShape().getLineCenter();

    // Set offset of the block icon
    myBlockIcon.offset = Position(1.1, (-3.06));

    // Set block icon rotation, and using their rotation for logo
    myBlockIcon.setRotation(firstLane);

    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


void
GNEVaporizer::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(),
                                          myAdditionalGeometry.getPosition(),
                                          myAdditionalGeometry.getRotation(),
                                          myViewNet->getVisualisationSettings().additionalSettings.vaporizerSize,
                                          myViewNet->getVisualisationSettings().additionalSettings.vaporizerSize);
}


Position
GNEVaporizer::getPositionInView() const {
    if (getParentEdges().front()->getLanes().front()->getLaneShape().length() < 2.5) {
        return getParentEdges().front()->getLanes().front()->getLaneShape().front();
    } else {
        Position A = getParentEdges().front()->getLanes().front()->getLaneShape().positionAtOffset(2.5);
        Position B = getParentEdges().front()->getLanes().back()->getLaneShape().positionAtOffset(2.5);

        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


Boundary
GNEVaporizer::getCenteringBoundary() const {
    return myAdditionalGeometry.getShape().getBoxBoundary().grow(10);
}


void
GNEVaporizer::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEVaporizer::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNEVaporizer::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


std::string
GNEVaporizer::getParentName() const {
    return getParentEdges().front()->getID();
}


void
GNEVaporizer::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double vaporizerExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (s.drawAdditionals(vaporizerExaggeration) && myViewNet->getDataViewOptions().showAdditionals()) {
        // get values
        const int numberOfLanes = int(getParentEdges().front()->getLanes().size());
        const double width = (double) 2.0 * s.scale;
        // begin draw
        glPushName(getGlID());
        glLineWidth(1.0);
        // set color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.additionalSettings.vaporizerColor);
        }
        // draw shape
        glPushMatrix();
        glTranslated(0, 0, getType());
        glTranslated(myAdditionalGeometry.getPosition().x(), myAdditionalGeometry.getPosition().y(), 0);
        glRotated(myAdditionalGeometry.getRotation(), 0, 0, 1);
        glScaled(vaporizerExaggeration, vaporizerExaggeration, 1);
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
        // draw position indicator (White) if isn't being drawn for selecting
        if ((width * vaporizerExaggeration > 1) && !s.drawForRectangleSelection) {
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
        glTranslated((-2.56), (-1.6), 0);
        // Draw icon depending of Vaporizer is selected and if isn't being drawn for selecting
        if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, vaporizerExaggeration)) {
            glColor3d(1, 1, 1);
            glRotated(-90, 0, 0, 1);
            if (drawUsingSelectColor()) {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_VAPORIZERSELECTED), s.additionalSettings.vaporizerSize);
            } else {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_VAPORIZER), s.additionalSettings.vaporizerSize);
            }
        } else {
            GLHelper::setColor(s.additionalSettings.vaporizerColor);
            GLHelper::drawBoxLine(Position(0, s.additionalSettings.vaporizerSize), 0, 2 * s.additionalSettings.vaporizerSize, s.additionalSettings.vaporizerSize);
        }
        // Pop logo matrix
        glPopMatrix();
        // Show Lock icon
        myBlockIcon.drawIcon(s, vaporizerExaggeration, 0.4);
        // draw name
        drawName(getPositionInView(), s.scale, s.addName);
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GNEGeometry::drawShapeDottedContour(s, getType(), vaporizerExaggeration, myDottedGeometry);
        }
        // pop name
        glPopName();
    }
}


std::string
GNEVaporizer::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            return getID();
        case SUMO_ATTR_BEGIN:
            return time2string(myBegin);
        case SUMO_ATTR_END:
            return time2string(myEnd);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEVaporizer::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(myBegin);
        case SUMO_ATTR_END:
            return STEPS2TIME(myEnd);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NAME:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVaporizer::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != nullptr) {
                return isValidAdditionalID(value);
            } else {
                return false;
            }
        case SUMO_ATTR_BEGIN:
            if (canParse<SUMOTime>(value)) {
                return (parse<SUMOTime>(value) <= myEnd);
            } else {
                return false;
            }
        case SUMO_ATTR_END:
            if (canParse<SUMOTime>(value)) {
                return (myBegin <= parse<SUMOTime>(value));
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVaporizer::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEVaporizer::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVaporizer::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVaporizer::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            replaceParentEdges(this, value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
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
