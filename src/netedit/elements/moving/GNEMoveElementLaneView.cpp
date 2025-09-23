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
// Class used for elements that can be moved over view
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementLaneView.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementLaneView::GNEMoveElementLaneView(GNEAttributeCarrier* element) :
    myElement(element) {
}


GNEMoveElementLaneView::GNEMoveElementLaneView(GNEAttributeCarrier* element, const Position& position) :
    myElement(element),
    myPosition(position) {
}


GNEMoveElementLaneView::~GNEMoveElementLaneView() {}


GNEMoveOperation*
GNEMoveElementLaneView::getMoveOperation() {
    // return move operation for element placed in view
    return new GNEMoveOperation(this, myPosition);
}


void
GNEMoveElementLaneView::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    myElement->updateGeometry();
}


void
GNEMoveElementLaneView::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(myElement, TLF("position of %", myElement->getTagStr()));
    myElement->setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
