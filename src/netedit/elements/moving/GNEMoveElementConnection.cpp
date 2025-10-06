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
/// @file    GNEMoveElementConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for moving connection shapes
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNEMoveElementConnection.h"

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementConnection::GNEMoveElementConnection(GNEConnection* connection) :
    GNEMoveElement(connection),
    myConnection(connection) {
}


GNEMoveElementConnection::~GNEMoveElementConnection() {}


GNEMoveOperation*
GNEMoveElementConnection::getMoveOperation() {
    // edit depending if shape is being edited
    if (myConnection->isShapeEdited()) {
        // calculate move shape operation
        return getEditShapeOperation(myConnection, myConnection->getConnectionShape(), false);
    } else {
        return nullptr;
    }
}


void
GNEMoveElementConnection::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (myConnection->isShapeEdited()) {
        // get original shape
        PositionVector shape = myConnection->getConnectionShape();
        // check shape size
        if (shape.size() > 2) {
            // obtain index
            int index = shape.indexOfClosest(clickedPosition);
            // get snap radius
            const double snap_radius = myConnection->getNet()->getViewNet()->getVisualisationSettings().neteditSizeSettings.connectionGeometryPointRadius;
            // check if we have to create a new index
            if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
                // remove geometry point
                shape.erase(shape.begin() + index);
                // commit new shape
                undoList->begin(myConnection, TLF("remove geometry point of %", myConnection->getTagStr()));
                GNEChange_Attribute::changeAttribute(myConnection, SUMO_ATTR_CUSTOMSHAPE, toString(shape), undoList, true);
                undoList->end();
            }
        }
    }
}


void
GNEMoveElementConnection::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
    myConnection->getNBConnection()->customShape = moveResult.shapeToUpdate;
    // update geometry
    myConnection->updateGeometry();
}


void
GNEMoveElementConnection::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(myConnection, TLF("moving % of %", toString(SUMO_ATTR_CUSTOMSHAPE), myConnection->getTagStr()));
    GNEChange_Attribute::changeAttribute(myConnection, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate), undoList, true);
    undoList->end();
}

/****************************************************************************/
