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
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

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
        // get connection
        const auto& connection = myConnection->getNBEdgeConnection();
        // calculate move shape operation
        return getEditShapeOperation(myConnection, connection.customShape.size() > 0 ? connection.customShape : myConnection->myConnectionGeometry.getShape(), false);
    } else {
        return nullptr;
    }
}


std::string
GNEMoveElementConnection::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementConnection::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementConnection::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementConnection::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementConnection::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementConnection::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementConnection::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementConnection::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // edit depending if shape is being edited
    if (myConnection->isShapeEdited()) {
        // get connection
        const auto& connection = myConnection->getNBEdgeConnection();
        // get original shape
        PositionVector shape = connection.customShape.size() > 0 ? connection.customShape : connection.shape;
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
                GNEChange_Attribute::changeAttribute(myConnection, SUMO_ATTR_CUSTOMSHAPE, toString(shape), undoList);
                undoList->end();
            }
        }
    }
}


void
GNEMoveElementConnection::setMoveShape(const GNEMoveResult& moveResult) {
    // set custom shape
    myConnection->getNBEdgeConnection().customShape = moveResult.shapeToUpdate;
    // mark junction as deprecated
    myConnection->myShapeDeprecated = true;
    // update geometry
    myConnection->updateGeometry();
}


void
GNEMoveElementConnection::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->begin(myConnection, TLF("moving custom shape of %", myConnection->getTagStr()));
    GNEChange_Attribute::changeAttribute(myConnection, SUMO_ATTR_CUSTOMSHAPE, toString(moveResult.shapeToUpdate), undoList);
    undoList->end();
}

/****************************************************************************/
