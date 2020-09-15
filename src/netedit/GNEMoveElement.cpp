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
/// @file    GNEMove.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for move shape elements
/****************************************************************************/
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>

#include "GNEMoveElement.h"


void 
GNEMoveElement::moveElement(GNEMoveOperation* moveOperation, const Position &offset) {
    PositionVector newShape = moveOperation->geometryToMove.getShape();
    for (const auto &index : moveOperation->geometryPointsToMove) {
        newShape[index].add(offset);
    }
    GNEGeometry::Geometry newGeometry(newShape);

    moveOperation->moveElement->setMoveGeometry(newGeometry);
    //
}


void 
GNEMoveElement::commitMove(GNEMoveOperation* moveOperation, const Position &offset, GNEUndoList* undoList) {
    // first reset geometry
    moveOperation->moveElement->setMoveGeometry(moveOperation->originalGeometry);

    PositionVector newShape = moveOperation->geometryToMove.getShape();
    for (const auto &index : moveOperation->geometryPointsToMove) {
        newShape[index].add(offset);
    }
    GNEAttributeCarrier *AC = dynamic_cast<GNEAttributeCarrier*>(moveOperation->moveElement);
    if (AC){
        // separate start and end points
        const Position begin = newShape.front();
        const Position end = newShape.back();
        newShape.pop_front();
        newShape.pop_back();
        // commit new shape
        undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + AC->getTagStr());
        undoList->p_add(new GNEChange_Attribute(AC, GNE_ATTR_SHAPE_START, toString(begin)));
        undoList->p_add(new GNEChange_Attribute(AC, SUMO_ATTR_SHAPE, toString(newShape)));
        undoList->p_add(new GNEChange_Attribute(AC, GNE_ATTR_SHAPE_END, toString(end)));
        undoList->p_end();
    }


    //
}

/****************************************************************************/
