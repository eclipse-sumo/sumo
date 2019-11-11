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
#include <utils/vehicle/SUMORouteHandler.h>

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

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, GNELane* lane, double startPos, double endPos,
        int parametersSet, const std::string& name, bool friendlyPosition, bool blockMovement) :
    GNEAdditional(id, viewNet, type, tag, name, blockMovement, {}, {lane}, {}, {}, {}, {}, {}, {}, {}, {}),
    myStartPosition(startPos),
    myEndPosition(endPos),
    myParametersSet(parametersSet),
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
        double startPos = (myParametersSet & STOPPINGPLACE_STARTPOS_SET) ? myStartPosition : 0;
        double endPos = (myParametersSet & STOPPINGPLACE_ENDPOS_SET) ? myEndPosition : laneLenght;
        // check if position has to be fixed
        if (startPos < 0) {
            startPos += laneLenght;
        }
        if (endPos < 0) {
            endPos += laneLenght;
        }
        // check values
        if (myParametersSet == 0) {
            return true;
        } else if ((myParametersSet & STOPPINGPLACE_STARTPOS_SET) == 0) {
            return (endPos <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
        } else if ((myParametersSet & STOPPINGPLACE_ENDPOS_SET) == 0) {
            return (startPos >= 0);
        } else {
            return ((startPos >= 0) && (endPos <= getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength()) && ((endPos - startPos) >= POSITION_EPS));
        }
    }
}


std::string
GNEStoppingPlace::getAdditionalProblem() const {
    // calculate start and end positions
    double startPos = (myParametersSet & STOPPINGPLACE_STARTPOS_SET) ? myStartPosition : 0;
    double endPos = (myParametersSet & STOPPINGPLACE_ENDPOS_SET) ? myEndPosition : getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
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
    double newStartPos = myStartPosition;
    double newEndPos = myEndPosition;
    // fix start and end positions using fixStoppingPlacePosition
    SUMORouteHandler::checkStopPos(newStartPos, newEndPos, getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength(), POSITION_EPS, true);
    // set new start and end positions
    setAttribute(SUMO_ATTR_STARTPOS, toString(newStartPos), myViewNet->getUndoList());
    setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPos), myViewNet->getUndoList());
}


Position
GNEStoppingPlace::getPositionInView() const {
    // calculate start and end positions as absolute values
    double startPos = fabs((myParametersSet & STOPPINGPLACE_STARTPOS_SET) ? myStartPosition : 0);
    double endPos = fabs((myParametersSet & STOPPINGPLACE_ENDPOS_SET) ? myEndPosition : getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength());
    // obtain position in view depending if both positions are defined
    if (myParametersSet == 0) {
        return getLaneParents().front()->getLaneShape().positionAtOffset(getLaneParents().front()->getLaneShape().length() / 2);
    } else if ((myParametersSet & STOPPINGPLACE_STARTPOS_SET) == 0) {
        return getLaneParents().front()->getLaneShape().positionAtOffset(endPos);
    } else if ((myParametersSet & STOPPINGPLACE_ENDPOS_SET) == 0) {
        return getLaneParents().front()->getLaneShape().positionAtOffset(startPos);
    } else {
        return getLaneParents().front()->getLaneShape().positionAtOffset((startPos + endPos) / 2.0);
    }
}


void
GNEStoppingPlace::moveGeometry(const Position& offset) {
    // only move if at leats start or end positions is defined
    if (myParametersSet > 0) {
        // Calculate new position using old position
        Position newPosition = myMove.originalViewPosition;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myViewNet->snapToActiveGrid(newPosition);
        double offsetLane = getLaneParents().front()->getLaneShape().nearest_offset_to_point2D(newPosition, false) - getLaneParents().front()->getLaneShape().nearest_offset_to_point2D(myMove.originalViewPosition, false);
        // check if both position has to be moved
        if ((myParametersSet & STOPPINGPLACE_STARTPOS_SET) && (myParametersSet & STOPPINGPLACE_ENDPOS_SET)) {
            // calculate stoppingPlace lenght and lane lenght (After apply geometry factor)
            double stoppingPlaceLenght = fabs(parse<double>(myMove.secondOriginalPosition) - parse<double>(myMove.firstOriginalLanePosition));
            double laneLengt = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength() * getLaneParents().front()->getLengthGeometryFactor();
            // avoid changing stopping place's lenght
            if ((parse<double>(myMove.firstOriginalLanePosition) + offsetLane) < 0) {
                myStartPosition = 0;
                myEndPosition = stoppingPlaceLenght;
            } else if ((parse<double>(myMove.secondOriginalPosition) + offsetLane) > laneLengt) {
                myStartPosition = laneLengt - stoppingPlaceLenght;
                myEndPosition = laneLengt;
            } else {
                myStartPosition = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
                myEndPosition = parse<double>(myMove.secondOriginalPosition) + offsetLane;
            }
        } else {
            // check if start position must be moved
            if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
                myStartPosition = parse<double>(myMove.firstOriginalLanePosition) + offsetLane;
            }
            // check if start position must be moved
            if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
                myEndPosition = parse<double>(myMove.secondOriginalPosition) + offsetLane;
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
    if (myParametersSet > 0) {
        undoList->p_begin("position of " + getTagStr());
        if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_STARTPOS, toString(myStartPosition), true, myMove.firstOriginalLanePosition));
        }
        if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_ENDPOS, toString(myEndPosition), true, myMove.secondOriginalPosition));
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
    if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
        return myStartPosition;
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndPosition() const {
    if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
        return myEndPosition;
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
    myGeometry.shape = getLaneParents().front()->getLaneShape();

    // Move shape to side
    myGeometry.shape.move2side(movingToSide * offsetSign);

    // Cut shape using as delimitators fixed start position and fixed end position
    myGeometry.shape = myGeometry.shape.getSubpart(getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane());

    // Get calculate lenghts and rotations
    myGeometry.calculateShapeRotationsAndLengths();
}


double
GNEStoppingPlace::getStartGeometryPositionOverLane() const {
    if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
        double fixedPos = myStartPosition;
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    } else {
        return 0;
    }
}


double
GNEStoppingPlace::getEndGeometryPositionOverLane() const {
    if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
        double fixedPos = myEndPosition;
        const double len = getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
        if (fixedPos < 0) {
            fixedPos += len;
        }
        return fixedPos * getLaneParents().front()->getLengthGeometryFactor();
    } else {
        return getLaneParents().front()->getParentEdge().getNBEdge()->getFinalLength();
    }
}


double 
GNEStoppingPlace::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            if (myParametersSet & STOPPINGPLACE_STARTPOS_SET) {
                return myStartPosition;
            } else {
                return -1;
            }
        case SUMO_ATTR_ENDPOS:
            if (myParametersSet & STOPPINGPLACE_ENDPOS_SET) {
                return myEndPosition;
            } else {
                return -1;
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
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
