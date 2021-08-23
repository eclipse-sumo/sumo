/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEStoppingPlace.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNEStoppingPlace::myCircleWidth = 1.1;
const double GNEStoppingPlace::myCircleWidthSquared = 1.21;
const double GNEStoppingPlace::myCircleInWidth = 0.9;
const double GNEStoppingPlace::myCircleInText = 1.6;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                                   GNELane* lane, const double startPos, const double endPos, const std::string& name,
                                   bool friendlyPosition, const std::map<std::string, std::string>& parameters, bool blockMovement) :
    GNEAdditional(id, net, type, tag, name,
{}, {}, {lane}, {}, {}, {}, {}, {},
parameters, blockMovement),
            myStartPosition(startPos),
            myEndPosition(endPos),
myFriendlyPosition(friendlyPosition) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


GNEMoveOperation*
GNEStoppingPlace::getMoveOperation(const double /*shapeOffset*/) {
    // check conditions
    if ((myStartPosition == INVALID_DOUBLE) && (myEndPosition == INVALID_DOUBLE)) {
        // start and end positions undefined, then nothing to move
        return nullptr;
    } else if (myBlockMovement) {
        // element blocked, then nothing to move
        return nullptr;
    } else {
        // return move operation for additional placed over shape
        return new GNEMoveOperation(this, getParentLanes().front(), getAttributeDouble(SUMO_ATTR_STARTPOS), getAttributeDouble(SUMO_ATTR_ENDPOS),
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
    }
}


bool
GNEStoppingPlace::isAdditionalValid() const {
    return GNEAdditionalHandler::checkDoublePositionOverLane(getAttributeDouble(SUMO_ATTR_STARTPOS), getAttributeDouble(SUMO_ATTR_ENDPOS),
            getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), myFriendlyPosition);
}


std::string
GNEStoppingPlace::getAdditionalProblem() const {
    // obtain lane length
    double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength() * getParentLanes().front()->getLengthGeometryFactor();
    // calculate start and end positions
    double startPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
    double endPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
    // check if position has to be fixed
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    // declare variables
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if (startPos < 0) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    } else if (startPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
    }
    if (endPos < 0) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    } else if (endPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
    }
    // check separator
    if ((errorStart.size() > 0) && (errorEnd.size() > 0)) {
        separator = " and ";
    }
    return errorStart + separator + errorEnd;
}


void
GNEStoppingPlace::fixAdditionalProblem() {
    // calculate start and end positions
    double startPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
    double endPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
    // fix start and end positions using fixStoppingPlacePosition
    GNEAdditionalHandler::fixDoublePositionOverLane(startPos, endPos, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
    // set new start and end positions
    setAttribute(SUMO_ATTR_STARTPOS, toString(startPos), myNet->getViewNet()->getUndoList());
    setAttribute(SUMO_ATTR_ENDPOS, toString(endPos), myNet->getViewNet()->getUndoList());
}


Position
GNEStoppingPlace::getPositionInView() const {
    return myBoundary.getCenter();
}


void
GNEStoppingPlace::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid && myTagProperty.isPlacedInRTree()) {
        myNet->removeGLObjectFromGrid(this);
    }
    // update geometry
    updateGeometry();
    // add shape boundary
    myBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    // grow with "width"
    if (myTagProperty.hasAttribute(SUMO_ATTR_WIDTH)) {
        // we cannot use "getAttributeDouble(...)"
        myBoundary.growWidth(parse<double>(getAttribute(SUMO_ATTR_WIDTH)));
    }
    // grow
    myBoundary.grow(10);
    // add parking spaces
    for (const auto& parkingSpace : getChildAdditionals()) {
        if (parkingSpace->getTagProperty().getTag() == SUMO_TAG_PARKING_SPACE) {
            myBoundary.add(parkingSpace->getCenteringBoundary());
        }
    }
    // add additional into RTREE again
    if (updateGrid &&  myTagProperty.isPlacedInRTree()) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEStoppingPlace::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // first check tat both network elements are lanes and originalElement correspond to stoppingPlace lane
    if ((originalElement->getTagProperty().getTag() == SUMO_TAG_LANE) &&
            (newElement->getTagProperty().getTag() == SUMO_TAG_LANE) &&
            (getParentLanes().front() == originalElement)) {
        // check if we have to change additional lane depending of split position
        if ((myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE)) {
            // calculate middle position
            const double middlePosition = ((myEndPosition - myStartPosition) / 2.0) + myStartPosition;
            //  four cases:
            if (splitPosition < myStartPosition) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, toString(myStartPosition - splitPosition), undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
            } else if ((splitPosition > myStartPosition) && (splitPosition < middlePosition)) {
                // change lane
                setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
                // now adjust start and end position
                setAttribute(SUMO_ATTR_STARTPOS, "0", undoList);
                setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
            } else if ((splitPosition > middlePosition) && (splitPosition < myEndPosition)) {
                // only adjust end position
                setAttribute(SUMO_ATTR_ENDPOS, toString(splitPosition), undoList);
            } else if ((splitPosition > myEndPosition)) {
                // nothing to do
            }
        } else if ((myStartPosition != INVALID_DOUBLE) && (splitPosition < myStartPosition)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust start position
            setAttribute(SUMO_ATTR_STARTPOS, toString(myEndPosition - splitPosition), undoList);
        } else if ((myEndPosition != INVALID_DOUBLE) && (splitPosition < myEndPosition)) {
            // change lane
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
            // now adjust end position
            setAttribute(SUMO_ATTR_ENDPOS, toString(myEndPosition - splitPosition), undoList);
        }
    }
}


std::string
GNEStoppingPlace::getParentName() const {
    return getParentLanes().front()->getID();
}


void
GNEStoppingPlace::setStoppingPlaceGeometry(double movingToSide) {
    // Get value of option "lefthand"
    const double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // obtain laneShape
    PositionVector laneShape = getParentLanes().front()->getLaneShape();

    // Move shape to side
    laneShape.move2side(movingToSide * offsetSign);

    // Cut shape using as delimitators fixed start position and fixed end position
    myAdditionalGeometry.updateGeometry(laneShape, getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
}


double
GNEStoppingPlace::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            if (myStartPosition != INVALID_DOUBLE) {
                return myStartPosition;
            } else {
                return 0;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosition != INVALID_DOUBLE) {
                return myEndPosition;
            } else {
                return getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
            }
        case GNE_ATTR_CENTER:
            return ((getAttributeDouble(SUMO_ATTR_ENDPOS) - getAttributeDouble(SUMO_ATTR_STARTPOS)) * 0.5) + getAttributeDouble(SUMO_ATTR_STARTPOS);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStoppingPlace::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    // all stopping place attributes are always enabled
    return true;
}


std::string
GNEStoppingPlace::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEStoppingPlace::getHierarchyName() const {
    return getTagStr();
}


void
GNEStoppingPlace::drawLines(const GUIVisualizationSettings& s, const std::vector<std::string>& lines, const RGBColor& color) const {
    if (!s.drawForPositionSelection) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() <= 1) ? 0 : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // Iterate over every line
        for (int i = 0; i < (int)lines.size(); ++i) {
            // push a new matrix for every line
            GLHelper::pushMatrix();
            // translate
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            // rotate over lane
            GNEGeometry::rotateOverLane(rot);
            // draw line with a color depending of the selection status
            if (drawUsingSelectColor()) {
                GLHelper::drawText(lines[i].c_str(), Position(1.2, (double)i), .1, 1.f, color, 0, FONS_ALIGN_LEFT);
            } else {
                GLHelper::drawText(lines[i].c_str(), Position(1.2, (double)i), .1, 1.f, color, 0, FONS_ALIGN_LEFT);
            }
            // pop matrix for every line
            GLHelper::popMatrix();
        }
    }
}


void
GNEStoppingPlace::drawSign(const GUIVisualizationSettings& s, const double exaggeration,
                           const RGBColor& baseColor, const RGBColor& signColor, const std::string& word) const {
    // calculate middle point
    const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
    // calculate rotation
    const double rot = (myAdditionalGeometry.getShape().size() <= 1) ? 0 : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
    if (s.drawForPositionSelection) {
        // only draw circle depending of distance between sign and mouse cursor
        if (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(mySignPos) <= (myCircleWidthSquared + 2)) {
            // push matrix
            GLHelper::pushMatrix();
            // Start drawing sign traslating matrix to signal position
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            // rotate over lane
            GNEGeometry::rotateOverLane(rot);
            // scale matrix depending of the exaggeration
            glScaled(exaggeration, exaggeration, 1);
            // set color
            GLHelper::setColor(baseColor);
            // Draw circle
            GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
            // pop draw matrix
            GLHelper::popMatrix();
        }
    } else {
        // push matrix
        GLHelper::pushMatrix();
        // Start drawing sign traslating matrix to signal position
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        // rotate over lane
        GNEGeometry::rotateOverLane(rot);
        // scale matrix depending of the exaggeration
        glScaled(exaggeration, exaggeration, 1);
        // set color
        GLHelper::setColor(baseColor);
        // Draw circle
        GLHelper::drawFilledCircle(myCircleWidth, s.getCircleResolution());
        // continue depending of rectangle selection
        if (!s.drawForRectangleSelection) {
            // Traslate to front
            glTranslated(0, 0, .1);
            // set color
            GLHelper::setColor(signColor);
            // draw another circle in the same position, but a little bit more small
            GLHelper::drawFilledCircle(myCircleInWidth, s.getCircleResolution());
            // draw H depending of detailSettings
            GLHelper::drawText(word, Position(), .1, myCircleInText, baseColor);
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


double
GNEStoppingPlace::getStartGeometryPositionOverLane() const {
    if (myStartPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get startPosition
        double fixedPos = myStartPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < 0) {
            return 0;
        } else if (fixedPos > (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS)) {
            return (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS);
        } else {
            return fixedPos;
        }
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndGeometryPositionOverLane() const {
    if (myEndPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get endPosition
        double fixedPos = myEndPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < POSITION_EPS) {
            return POSITION_EPS;
        } else if (fixedPos > getParentLanes().front()->getLaneShapeLength()) {
            return getParentLanes().front()->getLaneShapeLength();
        } else {
            return fixedPos;
        }
    } else {
        return getParentLanes().front()->getLaneShapeLength();
    }
}


void
GNEStoppingPlace::setMoveShape(const GNEMoveResult& moveResult) {
    // change both position
    myStartPosition = moveResult.newFirstPos;
    myEndPosition = moveResult.newSecondPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    updateGeometry();
}


void
GNEStoppingPlace::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // only commit geometry moving if at leats start or end positions is defined
    if ((myStartPosition != INVALID_DOUBLE) || (myEndPosition != INVALID_DOUBLE)) {
        // begin change attribute
        undoList->p_begin("position of " + getTagStr());
        // set startPos
        if (myStartPosition != INVALID_DOUBLE) {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos)));
        }
        // set endPos
        if (myEndPosition != INVALID_DOUBLE) {
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos)));
        }
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
        // end change attribute
        undoList->p_end();
    }
}

/****************************************************************************/
