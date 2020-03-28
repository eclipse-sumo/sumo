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
/// @file    GNEDetectorE1Instant.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
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
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorE1Instant.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE1Instant::GNEDetectorE1Instant(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos, const std::string& filename, const std::string& vehicleTypes, const std::string& name, bool friendlyPos, bool blockMovement) :
    GNEDetector(id, viewNet, GLO_E1DETECTOR_INSTANT, SUMO_TAG_INSTANT_INDUCTION_LOOP, pos, 0, filename, vehicleTypes, name, friendlyPos, blockMovement, {lane}) {
}


GNEDetectorE1Instant::~GNEDetectorE1Instant() {
}


bool
GNEDetectorE1Instant::isAdditionalValid() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        return fabs(myPositionOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEDetectorE1Instant::getAdditionalProblem() const {
    // declare variable for error position
    std::string errorPosition;
    const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // check positions over lane
    if (myPositionOverLane < -len) {
        errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
    }
    if (myPositionOverLane > len) {
        errorPosition = (toString(SUMO_ATTR_POSITION) + " > lanes's length");
    }
    return errorPosition;
}


void
GNEDetectorE1Instant::fixAdditionalProblem() {
    // declare new position
    double newPositionOverLane = myPositionOverLane;
    // fix pos and length  checkAndFixDetectorPosition
    GNEAdditionalHandler::checkAndFixDetectorPosition(newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), true);
    // set new position
    setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myViewNet->getUndoList());
}


void
GNEDetectorE1Instant::moveGeometry(const Position& offset) {
    // Calculate new position using old position
    Position newPosition = myMove.originalViewPosition;
    newPosition.add(offset);
    // filtern position using snap to active grid
    newPosition = myViewNet->snapToActiveGrid(newPosition);
    const bool storeNegative = myPositionOverLane < 0;
    myPositionOverLane = getParentLanes().front()->getLaneShape().nearest_offset_to_point2D(newPosition, false);
    if (storeNegative) {
        myPositionOverLane -= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
    // Update geometry
    updateGeometry();
}


void
GNEDetectorE1Instant::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_POSITION, toString(myPositionOverLane), true, myMove.firstOriginalLanePosition));
    undoList->p_end();
}


void
GNEDetectorE1Instant::updateGeometry() {
    // update geometry
    myAdditionalGeometry.updateGeometry(getParentLanes().front(), getGeometryPositionOverLane());

    // Set block icon position
    myBlockIcon.position = myAdditionalGeometry.getShape().getLineCenter();

    // Set offset of the block icon
    myBlockIcon.offset = Position(-1, 0);

    // Set block icon rotation, and using their rotation for logo
    myBlockIcon.setRotation(getParentLanes().front());

    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


void GNEDetectorE1Instant::updateDottedContour() {
    myDottedGeometry.updateDottedGeometry(myViewNet->getVisualisationSettings(),
                                          myAdditionalGeometry.getPosition(),
                                          myAdditionalGeometry.getRotation(),
                                          myViewNet->getVisualisationSettings().detectorSettings.E1InstantWidth,
                                          myViewNet->getVisualisationSettings().detectorSettings.E1InstantHeight);
}


void
GNEDetectorE1Instant::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double E1InstantExaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (s.drawAdditionals(E1InstantExaggeration) && myViewNet->getDataViewOptions().showAdditionals()) {
        // obtain scaledSize
        const double scaledWidth = s.detectorSettings.E1InstantWidth * 0.5 * s.scale;
        // start drawing
        glPushName(getGlID());
        glLineWidth(1.0);
        // set color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.detectorSettings.E1InstantColor);
        }
        // draw shape
        glPushMatrix();
        glTranslated(0, 0, getType());
        glTranslated(myAdditionalGeometry.getPosition().x(), myAdditionalGeometry.getPosition().y(), 0);
        glRotated(myAdditionalGeometry.getRotation(), 0, 0, 1);
        glScaled(E1InstantExaggeration, E1InstantExaggeration, 1);
        glBegin(GL_QUADS);
        glVertex2d(-1.0,  2);
        glVertex2d(-1.0, -2);
        glVertex2d(1.0, -2);
        glVertex2d(1.0,  2);
        glEnd();
        glTranslated(0, 0, .01);
        glBegin(GL_LINES);
        glVertex2d(0, 2 - .1);
        glVertex2d(0, -2 + .1);
        glEnd();
        // outline if isn't being drawn for selecting
        if ((scaledWidth * E1InstantExaggeration > 1) && !s.drawForRectangleSelection) {
            // set color
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectionColor);
            } else {
                GLHelper::setColor(RGBColor::WHITE);
            }
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_QUADS);
            glVertex2f(-1.0,  2);
            glVertex2f(-1.0, -2);
            glVertex2f(1.0, -2);
            glVertex2f(1.0,  2);
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        // position indicator if isn't being drawn for selecting
        if ((scaledWidth * E1InstantExaggeration > 1) && !s.drawForRectangleSelection) {
            // set color
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectionColor);
            } else {
                GLHelper::setColor(RGBColor::WHITE);
            }
            glRotated(90, 0, 0, -1);
            glBegin(GL_LINES);
            glVertex2d(0, 1.7);
            glVertex2d(0, -1.7);
            glEnd();
        }
        // Pop shape matrix
        glPopMatrix();
        // Check if the distance is enought to draw details and isn't being drawn for selecting
        if ((s.drawDetail(s.detailSettings.detectorDetails, E1InstantExaggeration)) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
            // Push matrix
            glPushMatrix();
            // Traslate to center of detector
            glTranslated(myAdditionalGeometry.getShape().getLineCenter().x(), myAdditionalGeometry.getShape().getLineCenter().y(), getType() + 0.1);
            // Rotate depending of myBlockIcon.rotation
            glRotated(myBlockIcon.rotation, 0, 0, -1);
            //move to logo position
            glTranslated(-1, 0, 0);
            // scale text
            glScaled(E1InstantExaggeration, E1InstantExaggeration, 1);
            // draw E1 logo
            if (drawUsingSelectColor()) {
                GLHelper::drawText("E1", Position(), .1, 1.5, s.colorSettings.selectionColor);
            } else {
                GLHelper::drawText("E1", Position(), .1, 1.5, RGBColor::BLACK);
            }
            // pop matrix
            glPopMatrix();
            // Show Lock icon depending of the Edit mode
            myBlockIcon.drawIcon(s, E1InstantExaggeration);
        }
        // Finish draw if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getPositionInView(), s.scale, s.addName);
        }
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GNEGeometry::drawShapeDottedContour(s, getType(), E1InstantExaggeration, myDottedGeometry);
        }
        glPopName();
    }
}


std::string
GNEDetectorE1Instant::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return myVehicleTypes;
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE1Instant::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNEDetectorE1Instant::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value) && fabs(parse<double>(value)) < getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE1Instant::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE1Instant::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_LANE:
            replaceParentLanes(this, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = value;
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
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
