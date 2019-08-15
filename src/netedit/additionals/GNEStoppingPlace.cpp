/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/options/OptionsCont.h>

#include "GNEStoppingPlace.h"

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

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane, const std::string& startPos, const std::string& endPos,
                                   const std::string& name, bool friendlyPosition, bool blockMovement) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement, {}, {lane}, {}, {}, {}, {}, {}, {}, {}, {}),
              myStartPosition(startPos),
              myEndPosition(endPos),
myFriendlyPosition(friendlyPosition) {
}


GNEStoppingPlace::~GNEStoppingPlace() {}


bool
GNEStoppingPlace::isAdditionalValid() const {
    // with friendly position enabled position are "always fixed"
    if (myFriendlyPosition) {
        return true;
    } else {
        // obtain lane length
        double laneLenght = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
        // calculate start and end positions
        double startPos = canParse<double>(myStartPosition) ? parse<double>(myStartPosition) : 0;
        double endPos = canParse<double>(myEndPosition) ? parse<double>(myEndPosition) : laneLenght;
        // check if position has to be fixed
        if (startPos < 0) {
            startPos += laneLenght;
        }
        if (endPos < 0) {
            endPos += laneLenght;
        }
        // check values
        if (myStartPosition.empty() && myEndPosition.empty()) {
            return true;
        } else if (myStartPosition.empty()) {
            return (endPos <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
        } else if (myEndPosition.empty()) {
            return (startPos >= 0);
        } else {
            return ((startPos >= 0) && (endPos <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) && ((endPos - startPos) >= POSITION_EPS));
        }
    }
}


std::string
GNEStoppingPlace::getAdditionalProblem() const {
    // calculate start and end positions
    double startPos = canParse<double>(myStartPosition) ? parse<double>(myStartPosition) : 0;
    double endPos = canParse<double>(myEndPosition) ? parse<double>(myEndPosition) : getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    // obtain lane lenght
    double laneLenght = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    // check if position has to be fixed
    if (startPos < 0) {
        startPos += laneLenght;
    }
    if (endPos < 0) {
        endPos += laneLenght;
    }
    // declare variables
    std::string errorStart, separator, errorEnd;
    // check positions over lane
    if (startPos < 0) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    } else if (startPos > getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) {
        errorStart = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
    }
    if (endPos < 0) {
        errorEnd = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    } else if (endPos > getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) {
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
    // declare new start and end position
    std::string newStartPos = myStartPosition;
    std::string newEndPos = myEndPosition;
    // fix start and end positions using fixStoppinPlacePosition
    fixStoppinPlacePosition(newStartPos, newEndPos, getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), true);
    // set new start and end positions
    setAttribute(SUMO_ATTR_STARTPOS, newStartPos, myViewNet->getUndoList());
    setAttribute(SUMO_ATTR_ENDPOS, newEndPos, myViewNet->getUndoList());
}


bool
GNEStoppingPlace::checkStoppinPlacePosition(const std::string& startPosStr, const std::string& endPosStr, const double laneLength, const bool friendlyPos) {
    // obtain start and end position in double format, depending if it can be parsed to double
    double startPos = GNEAttributeCarrier::canParse<double>(startPosStr) ? GNEAttributeCarrier::parse<double>(startPosStr) : 0;
    double endPos = GNEAttributeCarrier::canParse<double>(endPosStr) ? GNEAttributeCarrier::parse<double>(endPosStr) : laneLength;
    // return check stop pos (note: this is the same function of SUMORouteHandler::checkStopPos)
    if (POSITION_EPS > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < POSITION_EPS || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
    }
    if (startPos < 0 || startPos > endPos - POSITION_EPS) {
        if (!friendlyPos) {
            return false;
        }
    }
    return true;
}


bool
GNEStoppingPlace::fixStoppinPlacePosition(std::string& startPosStr, std::string& endPosStr, const double laneLength, const bool friendlyPos) {
    // obtain start and end position in double format
    double startPos = fabs(canParse<double>(startPosStr) ? parse<double>(startPosStr) : 0);
    double endPos = fabs(parse<double>(endPosStr) ? parse<double>(endPosStr) : laneLength);
    double minLength = POSITION_EPS + 0.01;
    // return check stop pos (note: this is the same function of SUMORouteHandler::checkStopPos)
    if (minLength > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < minLength || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if (startPos < 0 || startPos > endPos - minLength) {
        if (!friendlyPos) {
            return false;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > endPos - minLength) {
            startPos = endPos - minLength;
        }
    }
    startPosStr = toString(startPos);
    endPosStr = toString(endPos);
    return true;
}


Position
GNEStoppingPlace::getPositionInView() const {
    // calculate start and end positions as absolute values
    double startPos = fabs(canParse<double>(myStartPosition) ? parse<double>(myStartPosition) : 0);
    double endPos = fabs(canParse<double>(myEndPosition) ? parse<double>(myEndPosition) : getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
    // obtain position in view depending if both positions are defined
    if (myStartPosition.empty() && myEndPosition.empty()) {
        return getLaneParents().front()->getGeometry().shape.positionAtOffset(getLaneParents().front()->getGeometry().shape.length() / 2);
    } else if (myStartPosition.empty()) {
        return getLaneParents().front()->getGeometry().shape.positionAtOffset(endPos);
    } else if (myEndPosition.empty()) {
        return getLaneParents().front()->getGeometry().shape.positionAtOffset(startPos);
    } else {
        return getLaneParents().front()->getGeometry().shape.positionAtOffset((startPos + endPos) / 2.0);
    }
}


void
GNEStoppingPlace::moveGeometry(const Position& offset) {
    // only move if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        // Calculate new position using old position
        Position newPosition = myMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        double offsetLane = getLaneParents().front()->getGeometry().shape.nearest_offset_to_point2D(newPosition, false) - getLaneParents().front()->getGeometry().shape.nearest_offset_to_point2D(myMove.originalViewPosition, false);
        // check if both position has to be moved
        if (!myStartPosition.empty() && !myEndPosition.empty()) {
            // calculate stoppingPlace lenght and lane lenght (After apply geometry factor)
            double stoppingPlaceLenght = fabs(parse<double>(myMove.secondOriginalPosition) - parse<double>(myMove.firstOriginalLanePosition));
            double laneLengt = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
            // avoid changing stopping place's lenght
            if ((parse<double>(myMove.firstOriginalLanePosition) + offsetLane) < 0) {
                myStartPosition = "0";
                myEndPosition = toString(stoppingPlaceLenght);
            } else if ((parse<double>(myMove.secondOriginalPosition) + offsetLane) > laneLengt) {
                myStartPosition = toString(laneLengt - stoppingPlaceLenght);
                myEndPosition = toString(laneLengt);
            } else {
                myStartPosition = toString(parse<double>(myMove.firstOriginalLanePosition) + offsetLane);
                myEndPosition = toString(parse<double>(myMove.secondOriginalPosition) + offsetLane);
            }
        } else {
            // check if start position must be moved
            if (!myStartPosition.empty()) {
                myStartPosition = toString(parse<double>(myMove.firstOriginalLanePosition) + offsetLane);
            }
            // check if start position must be moved
            if (!myEndPosition.empty()) {
                myEndPosition = toString(parse<double>(myMove.secondOriginalPosition) + offsetLane);
            }
        }
        // update demand element children
        for (const auto& i : getDemandElementChildren()) {
            // if child is a person plan, update geometry of their person parent
            if (i->getTagProperty().isPersonPlan()) {
                i->getDemandElementParents().front()->markSegmentGeometryDeprecated();
                i->getDemandElementParents().front()->updateGeometry();
            } else {
                i->markSegmentGeometryDeprecated();
                i->updateGeometry();
            }
        }
        // Update geometry
        updateGeometry();
    }
}


void
GNEStoppingPlace::commitGeometryMoving(GNEUndoList* undoList) {
    // only commit geometry moving if at leats start or end positions is defined
    if (!myStartPosition.empty() || !myEndPosition.empty()) {
        undoList->p_begin("position of " + getTagStr());
        if (!myStartPosition.empty()) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_STARTPOS, myStartPosition, true, myMove.firstOriginalLanePosition));
        }
        if (!myEndPosition.empty()) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ENDPOS, myEndPosition, true, myMove.secondOriginalPosition));
        }
        undoList->p_end();
        // update demand element children
        for (const auto& i : getDemandElementChildren()) {
            // if child is a person plan, update geometry of their person parent
            if (i->getTagProperty().isPersonPlan()) {
                i->getDemandElementParents().front()->markSegmentGeometryDeprecated();
                i->getDemandElementParents().front()->updateGeometry();
            } else {
                i->markSegmentGeometryDeprecated();
                i->updateGeometry();
            }
        }
    }
}


double
GNEStoppingPlace::getStartPosition() const {
    if (canParse<double>(myStartPosition)) {
        return parse<double>(myStartPosition);
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndPosition() const {
    if (canParse<double>(myEndPosition)) {
        return parse<double>(myEndPosition);
    } else {
        return getLaneParents().front()->getLaneShapeLength();
    }
}


std::string
GNEStoppingPlace::getParentName() const {
    return getLaneParents().front()->getMicrosimID();
}


void
GNEStoppingPlace::setStoppingPlaceGeometry(double movingToSide) {
    // Clear all containers
    myGeometry.clearGeometry();

    // Get value of option "lefthand"
    double offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myGeometry.shape = getLaneParents().front()->getGeometry().shape;

    // Move shape to side
    myGeometry.shape.move2side(movingToSide * offsetSign);

    // Cut shape using as delimitators fixed start position and fixed end position
    myGeometry.shape = myGeometry.shape.getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());

    // Get calculate lenghts and rotations
    myGeometry.calculateShapeRotationsAndLengths();
}


double
GNEStoppingPlace::getStartGeometryPositionOverLane() const {
    if (myStartPosition.empty()) {
        return 0;
    } else {
        double fixedPos = parse<double>(myStartPosition);
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    }
}


double
GNEStoppingPlace::getEndGeometryPositionOverLane() const {
    if (myEndPosition.empty()) {
        return getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    } else {
        double fixedPos = parse<double>(myEndPosition);
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    }
}


std::string
GNEStoppingPlace::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEStoppingPlace::getHierarchyName() const {
    return getTagStr();
}

/****************************************************************************/
