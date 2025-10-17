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
        // check if allow change lane
        const bool allowChangeLane = myVehicle->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
        // obtain diameter
        const double diameter = myVehicle->getAttributeDouble(SUMO_ATTR_WIDTH) > myVehicle->getAttributeDouble(SUMO_ATTR_LENGTH) ? myVehicle->getAttributeDouble(SUMO_ATTR_WIDTH) : myVehicle->getAttributeDouble(SUMO_ATTR_LENGTH);
        // return move operation depending if we're editing departPos or arrivalPos
        if (myVehicle->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myVehicle->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS)) < (diameter * diameter)) {
            return new GNEMoveOperation(this, firstLane, startPosDouble, lastLane, INVALID_DOUBLE, true, allowChangeLane);
        } else if (myVehicle->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myVehicle->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS)) < (arrivalPositionDiameter * arrivalPositionDiameter)) {
            return new GNEMoveOperation(this, firstLane, INVALID_DOUBLE, lastLane, endPosDouble, false, allowChangeLane);
        }
    }
    // nothing to move
    return nullptr;
}


std::string
GNEMoveElementVehicle::getMovingAttribute(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttribute(key);
}


double
GNEMoveElementVehicle::getMovingAttributeDouble(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributeDouble(key);
}


Position
GNEMoveElementVehicle::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementVehicle::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementVehicle::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    myMovedElement->setCommonAttribute(key, value, undoList);
}


bool
GNEMoveElementVehicle::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    return myMovedElement->isCommonAttributeValid(key, value);
}


void
GNEMoveElementVehicle::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    myMovedElement->setCommonAttribute(key, value);
}


void
GNEMoveElementVehicle::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


void
GNEMoveElementVehicle::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // change depart
        myVehicle->departPosProcedure = DepartPosDefinition::GIVEN;
        myVehicle->parametersSet |= VEHPARS_DEPARTPOS_SET;
        myVehicle->departPos = moveResult.newFirstPos;
        // check if depart lane has to be changed
        if (moveResult.newFirstLane) {
            // set new depart lane
            std::string error = "";
            myVehicle->parseDepartLane(moveResult.newFirstLane->getID(), myVehicle->getTagStr(), myVehicle->getID(), myVehicle->departLane, myVehicle->departLaneProcedure, error);
            // mark parameter as set
            myVehicle->parametersSet |= VEHPARS_DEPARTLANE_SET;
        }
    } else if (moveResult.newLastPos != INVALID_DOUBLE) {
        // change arrival
        myVehicle->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
        myVehicle->parametersSet |= VEHPARS_ARRIVALPOS_SET;
        myVehicle->arrivalPos = moveResult.newFirstPos;
        // check if arrival lane has to be changed
        if (moveResult.newLastLane) {
            // set new arrival lane
            std::string error = "";
            myVehicle->parseArrivalLane(moveResult.newLastLane->getID(), myVehicle->getTagStr(), myVehicle->getID(), myVehicle->departLane, myVehicle->arrivalLaneProcedure, error);
            // mark parameter as set
            myVehicle->parametersSet |= VEHPARS_ARRIVALLANE_SET;
        }
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
        // begin change attribute
        undoList->begin(myVehicle, TLF("departPos of %", myVehicle->getTagStr()));
        // now set departPos
        myVehicle->setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos), undoList);
        // check if depart lane has to be changed
        if (moveResult.newFirstLane) {
            // set new depart lane
            myVehicle->setAttribute(SUMO_ATTR_DEPARTLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
        }
    } else if (moveResult.newLastPos != INVALID_DOUBLE) {
        // begin change attribute
        undoList->begin(myVehicle, TLF("arrivalPos of %", myVehicle->getTagStr()));
        // now set arrivalPos
        myVehicle->setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newLastPos), undoList);
        // check if arrival lane has to be changed
        if (moveResult.newLastLane) {
            // set new arrival lane
            myVehicle->setAttribute(SUMO_ATTR_ARRIVALLANE, toString(moveResult.newLastLane->getIndex()), undoList);
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
