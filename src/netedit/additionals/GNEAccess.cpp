/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAccess.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2018
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEAccess.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAccess::GNEAccess(GNEAdditional* busStop, GNELane* lane, GNEViewNet* viewNet, const std::string& pos, const std::string& length, bool friendlyPos, bool blockMovement) :
    GNEAdditional(busStop, viewNet, GLO_ACCESS, SUMO_TAG_ACCESS, "", blockMovement,
{}, {lane}, {}, {busStop}, {}, {}, {}, {}, {}, {}),
myPositionOverLane(pos),
myLength(length),
myFriendlyPosition(friendlyPos) {
}


GNEAccess::~GNEAccess() {
}


void
GNEAccess::moveGeometry(const Position& offset) {
    // Calculate new position using old position
    Position newPosition = myMove.originalViewPosition;
    newPosition.add(offset);
    // filtern position using snap to active grid
    newPosition = myViewNet->snapToActiveGrid(newPosition);
    myPositionOverLane = toString(getLaneParents().front()->getGeometry().shape.nearest_offset_to_point2D(newPosition, false));
    // Update geometry
    updateGeometry();
}


void
GNEAccess::commitGeometryMoving(GNEUndoList* undoList) {
    if (!myBlockMovement) {
        // commit new position allowing undo/redo
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_POSITION, myPositionOverLane, true, myMove.firstOriginalLanePosition));
        undoList->p_end();
    }
}


void
GNEAccess::updateGeometry() {
    // Clear all containers
    myGeometry.clearGeometry();

    // Get shape of lane parent
    myGeometry.shape = getLaneParents().front()->getGeometry().shape;

    // set start position
    double fixedPositionOverLane;
    if (!canParse<double>(myPositionOverLane)) {
        fixedPositionOverLane = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    } else if (parse<double>(myPositionOverLane) < 0) {
        fixedPositionOverLane = 0;
    } else if (parse<double>(myPositionOverLane) > getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) {
        fixedPositionOverLane = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        fixedPositionOverLane = parse<double>(myPositionOverLane);
    }
    // obtain position
    myGeometry.shape[0] = getLaneParents().front()->getGeometry().shape.positionAtOffset(fixedPositionOverLane * getLaneParents().front()->getLengthGeometryFactor());

    // Save rotation (angle) of the vector constructed by points f and s
    myGeometry.shapeRotations.push_back(getLaneParents().front()->getGeometry().shape.rotationDegreeAtOffset(fixedPositionOverLane) * -1);

    // Set block icon position
    myBlockIcon.position = myGeometry.shape.getLineCenter();

    // Set offset of the block icon
    myBlockIcon.offset = Position(-1, 0);

    // Set block icon rotation, and using their rotation for logo
    myBlockIcon.setRotation(getLaneParents().front());
}


Position
GNEAccess::getPositionInView() const {
    if (!canParse<double>(myPositionOverLane)) {
        return getLaneParents().front()->getGeometry().shape.front();
    } else {
        double posOverLane = parse<double>(myPositionOverLane);
        if (posOverLane < 0) {
            return getLaneParents().front()->getGeometry().shape.front();
        } else if (posOverLane > getLaneParents().front()->getGeometry().shape.length()) {
            return getLaneParents().front()->getGeometry().shape.back();
        } else {
            return getLaneParents().front()->getGeometry().shape.positionAtOffset(posOverLane);
        }
    }
}


Boundary
GNEAccess::getCenteringBoundary() const {
    return myGeometry.shape.getBoxBoundary().grow(10);
}


bool
GNEAccess::isAccessPositionFixed() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        if (canParse<double>(myPositionOverLane)) {
            return (parse<double>(myPositionOverLane) >= 0) && ((parse<double>(myPositionOverLane)) <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
        } else {
            return false;
        }
    }
}


GNEEdge&
GNEAccess::getEdge() const {
    return getLaneParents().front()->getParentEdge();
}


std::string
GNEAccess::getParentName() const {
    return getAdditionalParents().at(0)->getID();
}


void
GNEAccess::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // Start drawing adding an gl identificator
    glPushName(getGlID());
    // push matrix
    glPushMatrix();
    // set color depending of selection
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.colorSettings.busStop);
    }
    glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), GLO_ACCESS);
    // draw circle
    if (s.drawForSelecting) {
        GLHelper::drawFilledCircle((double) 0.5 * exaggeration, 8);
    } else {
        std::vector<Position> vertices = GLHelper::drawFilledCircleReturnVertices((double) 0.5 * exaggeration, 16);
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GLHelper::drawShapeDottedContourAroundClosedShape(s, getType(), vertices);
        }
    }
    // pop matrix
    glPopMatrix();
    // pop gl identficador
    glPopName();
}


std::string
GNEAccess::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_LANE:
            return getLaneParents().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_PARENT:
            return getAdditionalParents().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEAccess::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_LANE: {
            GNELane* lane = myViewNet->getNet()->retrieveLane(value, false);
            if (lane != nullptr) {
                if (getLaneParents().front()->getParentEdge().getID() != lane->getParentEdge().getID()) {
                    return GNEAdditionalHandler::accessCanBeCreated(getAdditionalParents().at(0), lane->getParentEdge());
                } else {
                    return true;
                }
            } else {
                return false;
            }
        }
        case SUMO_ATTR_POSITION:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_LENGTH:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEAccess::getPopUpID() const {
    return getTagStr();
}


std::string
GNEAccess::getHierarchyName() const {
    return getTagStr() + ": " + getLaneParents().front()->getParentEdge().getID();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEAccess::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_LANE:
            changeLaneParents(this, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = value;
            break;
        case SUMO_ATTR_LENGTH:
            myLength = value;
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
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
