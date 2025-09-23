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
    myElement(element) {
}


GNEMoveElementLaneSingle::GNEMoveElementLaneSingle(GNEAttributeCarrier* element, GNELane* lane,
        const double position, const bool friendlyPos) :
    myElement(element),
    myPosition(position),
    myFriendlyPos(friendlyPos) {
    // set parents
    element->getHierarchicalElement()->setParent<GNELane*>(lane);
}


GNEMoveElementLaneSingle::~GNEMoveElementLaneSingle() {}


GNEMoveOperation*
GNEMoveElementLaneSingle::getMoveOperation() {
    // return move operation over a single position
    return new GNEMoveOperation(this, myElement->getHierarchicalElement()->getParentLanes().front(), myPosition,
                                myElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane());
}


void
GNEMoveElementLaneSingle::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementLaneSingle::setMoveShape(const GNEMoveResult& moveResult) {
    // change position
    myPosition = moveResult.newFirstPos;
    // set lateral offset
    myMoveElementLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    myElement->updateGeometry();
}


void
GNEMoveElementLaneSingle::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMoveElementLateralOffset = 0;
    // begin change attribute
    undoList->begin(myElement, TLF("position of %", myElement->getTagStr()));
    // set position
    myElement->setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    // check if lane has to be changed
    if (moveResult.newFirstLane) {
        // set new lane
        myElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
