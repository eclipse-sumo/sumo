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
/// @file    GNEMoveElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over a lane with only one position
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementLaneSingle.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementLaneSingle::GNEMoveElementLaneSingle(GNEAttributeCarrier* element) :
    GNEMoveElement(element) {
}


GNEMoveElementLaneSingle::GNEMoveElementLaneSingle(GNEAttributeCarrier* element, GNELane* lane,
        const double position, const bool friendlyPos) :
    GNEMoveElement(element),
    myPosition(position),
    myFriendlyPos(friendlyPos) {
    // set parents
    element->getHierarchicalElement()->setParent<GNELane*>(lane);
}


GNEMoveElementLaneSingle::~GNEMoveElementLaneSingle() {}


GNEMoveOperation*
GNEMoveElementLaneSingle::getMoveOperation() {
    // return move operation over a single position
    return new GNEMoveOperation(this, myMovedElement->getHierarchicalElement()->getParentLanes().front(), myPosition,
                                myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane());
}


void
GNEMoveElementLaneSingle::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


double
GNEMoveElementLaneSingle::getOffsetPositionOverLane() const {
    const auto& lane = myMovedElement->getHierarchicalElement()->getParentLanes().front();
    // continue depending if we defined a end position
    if (myPosition != INVALID_DOUBLE) {
        // get lane final and shape length
        const double laneLength = lane->getParentEdge()->getNBEdge()->getFinalLength();
        // get startPosition
        double fixedPos = myPosition;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        fixedPos *= lane->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < 0) {
            return 0;
        } else if (fixedPos > (lane->getLaneShapeLength() - POSITION_EPS)) {
            return (lane->getLaneShapeLength() - POSITION_EPS);
        } else {
            return fixedPos;
        }
    } else {
        return 0;
    }
}


void
GNEMoveElementLaneSingle::setMoveShape(const GNEMoveResult& moveResult) {
    // change position
    myPosition = moveResult.newFirstPos;
    // set lateral offset
    myMovingLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementLaneSingle::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMovingLateralOffset = 0;
    // begin change attribute
    undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
    // set position
    myMovedElement->setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    // check if lane has to be changed
    if (moveResult.newFirstLane) {
        // set new lane
        myMovedElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
