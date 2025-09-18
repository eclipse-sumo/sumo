/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNELaneMovableElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/xml/NamespaceIDs.h>
#include <utils/gui/globjects/GUIGlObject.h>

#include "GNEAttributeCarrier.h"
#include "GNELaneMovableElement.h"
#include "GNEMoveElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNELaneMovableElement::GNELaneMovableElement() {
}


GNELaneMovableElement::GNELaneMovableElement(GNEAttributeCarrier* element, GNELane* lane, const double startPos, const double endPos,
        const bool friendlyPosition) :
    myElement(element),
    myStartPosition(startPos),
    myEndPosition(endPos),
    myFriendlyPosition(friendlyPosition) {
    // set parents
    element->getHierarchicalElement()->setParent<GNELane*>(lane);
}


GNELaneMovableElement::~GNELaneMovableElement() {}


GNEMoveOperation*
GNELaneMovableElement::getMoveOperation() {
    const auto& parentLanes = myElement->getHierarchicalElement()->getParentLanes();
    // get allow change lane
    const bool allowChangeLane = myElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
    // fist check if we're moving only extremes
    if (TESTdrawMovingGeometryPoints(false)) {
        // get geometry points under cursor
        const auto geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(this);
        // continue depending of moved element
        if (geometryPoints.empty()) {
            return nullptr;
        } else if (geometryPoints.front() == 0) {
            // move start position
            return new GNEMoveOperation(myElement, parentLanes.front(), myStartPosition, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
        } else {
            // move end position
            return new GNEMoveOperation(myElement, parentLanes.front(), 0, myEndPosition,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
        }
    } else if ((myStartPosition != INVALID_DOUBLE) && (myEndPosition != INVALID_DOUBLE)) {
        // move both start and end positions
        return new GNEMoveOperation(myElement, parentLanes.front(), myStartPosition, myEndPosition,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_BOTH);
    } else if (myStartPosition != INVALID_DOUBLE) {
        // move only start position
        return new GNEMoveOperation(myElement, parentLanes.front(), myStartPosition, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
    } else if (myEndPosition != INVALID_DOUBLE) {
        // move only end position
        return new GNEMoveOperation(myElement, parentLanes.front(), 0, myEndPosition,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
    } else {
        // start and end positions undefined, then nothing to move
        return nullptr;
    }
}


bool
GNELaneMovableElement::TESTdrawMovingGeometryPoints(const bool ignoreShift) const {
    // get modes
    const auto& modes = myElement->getNet()->getViewNet()->getEditModes();
    // check conditions
    if (modes.isCurrentSupermodeNetwork() && (modes.networkEditMode == NetworkEditMode::NETWORK_MOVE) &&
            (ignoreShift || myElement->getNet()->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed())) {
        return true;
    } else {
        return false;
    }
}


double
GNELaneMovableElement::getStartGeometryPositionOverLane() const {
    const auto& parentLanes = myElement->getHierarchicalElement()->getParentLanes();
    // continue depending if we defined a end position
    if (myStartPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = parentLanes.front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get startPosition
        double fixedPos = myStartPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= parentLanes.front()->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < 0) {
            return 0;
        } else if (fixedPos > (parentLanes.front()->getLaneShapeLength() - POSITION_EPS)) {
            return (parentLanes.front()->getLaneShapeLength() - POSITION_EPS);
        } else {
            return fixedPos;
        }
    } else {
        return 0;
    }
}


double
GNELaneMovableElement::getEndGeometryPositionOverLane() const {
    const auto& parentLanes = myElement->getHierarchicalElement()->getParentLanes();
    // continue depending if we defined a end position
    if (myEndPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = parentLanes.front()->getParentEdge()->getNBEdge()->getFinalLength();
        // get endPosition
        double fixedPos = myEndPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= parentLanes.front()->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < POSITION_EPS) {
            return POSITION_EPS;
        } else if (fixedPos > parentLanes.front()->getLaneShapeLength()) {
            return parentLanes.front()->getLaneShapeLength();
        } else {
            return fixedPos;
        }
    } else {
        return parentLanes.back()->getLaneShapeLength();
    }
}


void
GNELaneMovableElement::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // change only start position
        myStartPosition = moveResult.newFirstPos;
        // adjust startPos
        if (myStartPosition > (myElement->getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS)) {
            myStartPosition = (myElement->getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS);
        }
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // change only end position
        myEndPosition = moveResult.newFirstPos;
        // adjust endPos
        if (myEndPosition < (myElement->getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS)) {
            myEndPosition = (myElement->getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS);
        }
    } else {
        // change both position
        myStartPosition = moveResult.newFirstPos;
        myEndPosition = moveResult.newLastPos;
        // set lateral offset
        myMoveElementLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    myElement->updateGeometry();
}


void
GNELaneMovableElement::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myElement, "position of " + myElement->getTagStr());
    // set attributes depending of operation type
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // set only start position
        myElement->setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // set only end position
        myElement->setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        myElement->setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        myElement->setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newLastPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            myElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}


void
GNELaneMovableElement::adjustLength(const double newLength, GNEUndoList* undoList) {
    const auto laneLength = myElement->getHierarchicalElement()->getParentLanes().front()->getLaneShapeLength();
    auto newStartPos = myStartPosition;
    auto newEndPos = myEndPosition;
    if ((newStartPos != INVALID_DOUBLE) && (newEndPos != INVALID_DOUBLE)) {
        // get middle lengths
        const auto middleLength = (newEndPos - newStartPos) * 0.5;
        const auto middleNewLength = newLength * 0.5;
        // set new start and end positions
        newStartPos -= (middleNewLength - middleLength);
        newEndPos += (middleNewLength - middleLength);
        // now adjust both
        if (newStartPos < 0) {
            newEndPos += (newStartPos * -1);
            newStartPos = 0;
            if ((newStartPos < 0) && (newEndPos > laneLength)) {
                newStartPos = 0;
                newEndPos = laneLength;
            }
        } else if (newEndPos > laneLength) {
            newStartPos -= (newEndPos - laneLength);
            newStartPos = laneLength;
            if ((newStartPos < 0) && (newEndPos > laneLength)) {
                newStartPos = 0;
                newEndPos = laneLength;
            }
        }
        // set new start and end positions
        undoList->begin(myElement, TLF(" %'s length", myElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myElement, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(myElement, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    } else if (newStartPos != INVALID_DOUBLE) {
        newStartPos = laneLength - newLength;
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        undoList->begin(myElement, TLF(" %'s length", myElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myElement, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        undoList->end();
    } else if (newEndPos != INVALID_DOUBLE) {
        newEndPos = newLength;
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        undoList->begin(myElement, TLF(" %'s length", myElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myElement, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    }
}

/****************************************************************************/
