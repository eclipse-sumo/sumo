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
/// @file    GNEMoveElementVehicle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/demand/GNEVehicle.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/moving/GNEMoveElement.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEMoveElementVehicle.h"

// ===========================================================================
// static definitions
// ===========================================================================

const double GNEMoveElementVehicle::arrivalPositionDiameter = SUMO_const_halfLaneWidth * 0.5;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementVehicle::GNEMoveElementVehicle(GNEVehicle* vehicle, GNEEdge* fromEdge,
        double& departPos, GNEEdge* toEdge, double& arrivalPos) :
    GNEMoveElement(vehicle),
    myVehicle(vehicle),
    myDepartPos(departPos),
    myArrivalPos(arrivalPos) {
    // set parents
    vehicle->getHierarchicalElement()->setParents<GNEEdge*>({fromEdge, toEdge});
}


GNEMoveElementVehicle::~GNEMoveElementVehicle() {}


GNEMoveOperation*
GNEMoveElementVehicle::getMoveOperation() {
    // get first and last lanes
    const GNELane* firstLane = myVehicle->getFirstPathLane();
    const GNELane* lastLane = myVehicle->getLastPathLane();
    // check both lanes
    if (firstLane && lastLane) {
        // get depart and arrival positions (doubles)
        const double startPosDouble = myVehicle->getAttributeDouble(SUMO_ATTR_DEPARTPOS);
        const double endPosDouble = (myVehicle->getAttributeDouble(SUMO_ATTR_ARRIVALPOS) < 0) ? lastLane->getLaneShape().length2D() : myVehicle->getAttributeDouble(SUMO_ATTR_ARRIVALPOS);
        // obtain diameter
        const double diameter = myVehicle->getAttributeDouble(SUMO_ATTR_WIDTH) > myVehicle->getAttributeDouble(SUMO_ATTR_LENGTH) ? myVehicle->getAttributeDouble(SUMO_ATTR_WIDTH) : myVehicle->getAttributeDouble(SUMO_ATTR_LENGTH);
        // return move operation depending if we're editing departPos or arrivalPos
        if (myVehicle->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myVehicle->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS)) < (diameter * diameter)) {
            return new GNEMoveOperation(this, firstLane, startPosDouble, lastLane, INVALID_DOUBLE,
                                        myVehicle->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane(),
                                        GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST);
        } else if (myVehicle->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myVehicle->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS)) < (arrivalPositionDiameter * arrivalPositionDiameter)) {
            return new GNEMoveOperation(this, firstLane, INVALID_DOUBLE, lastLane, endPosDouble,
                                        myVehicle->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane(),
                                        GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST);
        }
    }
    // nothing to move
    return nullptr;
}


void
GNEMoveElementVehicle::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementVehicle::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.newFirstPos != INVALID_DOUBLE) &&
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST)) {
        // change depart
        myVehicle->departPosProcedure = DepartPosDefinition::GIVEN;
        myVehicle->parametersSet |= VEHPARS_DEPARTPOS_SET;
        myVehicle->departPos = moveResult.newFirstPos;
    }
    if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST)) {
        // change arrival
        myVehicle->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
        myVehicle->parametersSet |= VEHPARS_ARRIVALPOS_SET;
        myVehicle->arrivalPos = moveResult.newFirstPos;
    }
    // set lateral offset
    myMovingLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    myVehicle->updateGeometry();
}


void
GNEMoveElementVehicle::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMovingLateralOffset = 0;
    // check value
    if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // continue depending if we're moving first or last position
        if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST) {
            // begin change attribute
            undoList->begin(myVehicle, TLF("departPos of %", myVehicle->getTagStr()));
            // now set departPos
            myVehicle->setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
            // check if depart lane has to be changed
            if (moveResult.newFirstLane) {
                // set new depart lane
                myVehicle->setAttribute(SUMO_ATTR_DEPARTLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
            }
        } else {
            // begin change attribute
            undoList->begin(myVehicle, TLF("arrivalPos of %", myVehicle->getTagStr()));
            // now set arrivalPos
            myVehicle->setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos), undoList);
            // check if arrival lane has to be changed
            if (moveResult.newFirstLane) {
                // set new arrival lane
                myVehicle->setAttribute(SUMO_ATTR_ARRIVALLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
            }
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
