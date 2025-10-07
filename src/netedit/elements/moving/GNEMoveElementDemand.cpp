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
/// @file    GNEMoveElementDemand.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2025
///
// Class used for elements that can be moved over a edge with two positions
/****************************************************************************/
#include <config.h>

#include <foreign/fontstash/fontstash.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
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

#include "GNEMoveElementDemand.h"

// ===========================================================================
// static definitions
// ===========================================================================

const double GNEMoveElementDemand::arrivalPositionDiameter = SUMO_const_halfLaneWidth * 0.5;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementDemand::GNEMoveElementDemand(GNEDemandElement* demandElement,
        GNEEdge* fromEdge, SumoXMLAttr startPosAttr, double& startPosValue,
        GNEEdge* toEdge, SumoXMLAttr endPosAttr, double& endPosValue) :
    GNEMoveElement(demandElement),
    myDemandElement(demandElement),
    myStartPosAttr(startPosAttr),
    myStartPosValue(startPosValue),
    myEndPosAttr(endPosAttr),
    myEndPosPosValue(endPosValue) {
    // set parents
    demandElement->getHierarchicalElement()->setParents<GNEEdge*>({fromEdge, toEdge});
}


GNEMoveElementDemand::~GNEMoveElementDemand() {}


GNEMoveOperation*
GNEMoveElementDemand::getMoveOperation() {
    // get first and last lanes
    const GNELane* firstLane = myDemandElement->getFirstPathLane();
    const GNELane* lastLane = myDemandElement->getLastPathLane();
    // check both lanes
    if (firstLane && lastLane) {
        // get depart and arrival positions (doubles)
        const double startPosDouble = myDemandElement->getAttributeDouble(myStartPosAttr);
        const double endPosDouble = (myDemandElement->getAttributeDouble(myEndPosAttr) < 0) ? lastLane->getLaneShape().length2D() : myDemandElement->getAttributeDouble(myEndPosAttr);
        // obtain diameter
        const double diameter = myDemandElement->getAttributeDouble(SUMO_ATTR_WIDTH) > myDemandElement->getAttributeDouble(SUMO_ATTR_LENGTH) ? myDemandElement->getAttributeDouble(SUMO_ATTR_WIDTH) : myDemandElement->getAttributeDouble(SUMO_ATTR_LENGTH);
        // return move operation depending if we're editing departPos or arrivalPos
        if (myDemandElement->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myDemandElement->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_STARTPOS)) < (diameter * diameter)) {
            return new GNEMoveOperation(this, firstLane, startPosDouble, lastLane, INVALID_DOUBLE,
                                        myDemandElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane(),
                                        GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST);
        } else if (myDemandElement->getNet()->getViewNet()->getPositionInformation().distanceSquaredTo2D(myDemandElement->getAttributePosition(GNE_ATTR_PLAN_GEOMETRY_ENDPOS)) < (arrivalPositionDiameter * arrivalPositionDiameter)) {
            return new GNEMoveOperation(this, firstLane, INVALID_DOUBLE, lastLane, endPosDouble,
                                        myDemandElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane(),
                                        GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST);
        }
    }
    // nothing to move
    return nullptr;
}


void
GNEMoveElementDemand::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


std::string
GNEMoveElementDemand::getMovingAttribute(const Parameterised* parameterised, SumoXMLAttr key) const {
    // position attributes
    if (key == myStartPosAttr) {
        if (myStartPosValue == INVALID_DOUBLE) {
            return GNEAttributeCarrier::LANE_START;
        } else {
            return toString(myStartPosValue);
        }
    } else if (key == myEndPosAttr) {
        if (myEndPosPosValue == INVALID_DOUBLE) {
            return GNEAttributeCarrier::LANE_END;
        } else {
            return toString(myEndPosPosValue);
        }
    } else {
        return myMovedElement->getCommonAttribute(parameterised, key);
    }
}


double
GNEMoveElementDemand::getMovingAttributeDouble(SumoXMLAttr /*key*/) const {
    // nothing to do
    return 0;
}


void
GNEMoveElementDemand::setMovingAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


bool
GNEMoveElementDemand::isMovingAttributeValid(SumoXMLAttr /*key*/, const std::string& /*value*/) const {
    // nothing to do
    return false;
}


void
GNEMoveElementDemand::setMovingAttribute(Parameterised* /*parameterised*/, SumoXMLAttr /*key*/, const std::string& /*value*/) {
    // nothing to do
}

bool
GNEMoveElementDemand::isMoveElementValid() const {
    // nothing to do
    return true;
}


std::string
GNEMoveElementDemand::getMovingProblem() const {
    // nothing to do
    return "";
}


void
GNEMoveElementDemand::fixMovingProblem() {
    // nothing to do
}


double
GNEMoveElementDemand::getStartFixedPositionOverEdge() const {
    const auto& firstLane = myDemandElement->getFirstPathLane();
    // continue depending if we defined a end position
    if (firstLane == nullptr) {
        return 0;
    } else if (myStartPosValue == INVALID_DOUBLE) {
        return 0;
    } else {
        // get edge length and fixed end pos
        const double edgeLength = firstLane->getParentEdge()->getNBEdge()->getFinalLength();
        const double fixedEndPos = getEndFixedPositionOverEdge();
        // fix position
        double fixedPos = myStartPosValue;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += edgeLength;
        }
        // set length geometry factor
        fixedPos *= firstLane->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < 0) {
            return 0;
        } else if (fixedPos > (fixedEndPos - POSITION_EPS)) {
            return (fixedEndPos - POSITION_EPS);
        } else {
            return fixedPos;
        }
    }
}


double
GNEMoveElementDemand::getEndFixedPositionOverEdge() const {
    const auto lastLane = myDemandElement->getLastPathLane();
    // continue depending if we defined a end position
    if (lastLane == nullptr) {
        return 0;
    } else if (myEndPosPosValue == INVALID_DOUBLE) {
        return lastLane->getLaneShapeLength();
    } else {
        // get edge final and shape length
        const double edgeLength = lastLane->getParentEdge()->getNBEdge()->getFinalLength();
        // fix position
        double fixedPos = myEndPosPosValue;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += edgeLength;
        }
        // set length geometry factor
        fixedPos *= lastLane->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < POSITION_EPS) {
            return POSITION_EPS;
        } else if (fixedPos > lastLane->getLaneShapeLength()) {
            return lastLane->getLaneShapeLength();
        } else {
            return fixedPos;
        }
    }
}


void
GNEMoveElementDemand::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.newFirstPos != INVALID_DOUBLE) &&
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST)) {
        // change depart
        myDemandElement->setAttribute(SUMO_ATTR_DEPARTPOS, toString(moveResult.newFirstPos));
    }
    if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST)) {
        myDemandElement->setAttribute(SUMO_ATTR_ARRIVALPOS, toString(moveResult.newFirstPos));
    }
    // set lateral offset
    myMovingLateralOffset = moveResult.firstLaneOffset;
    // update geometry
    myDemandElement->updateGeometry();
}


void
GNEMoveElementDemand::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMovingLateralOffset = 0;
    // check value
    if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // continue depending if we're moving first or last position
        if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST) {
            // begin change attribute
            undoList->begin(myDemandElement, TLF("departPos of %", myDemandElement->getTagStr()));
            // now set departPos
            myDemandElement->setAttribute(myStartPosAttr, toString(moveResult.newFirstPos), undoList);
            // check if depart lane has to be changed
            if (moveResult.newFirstLane) {
                // set new depart lane
                myDemandElement->setAttribute(SUMO_ATTR_DEPARTLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
            }
        } else {
            // begin change attribute
            undoList->begin(myDemandElement, TLF("arrivalPos of %", myDemandElement->getTagStr()));
            // now set arrivalPos
            myDemandElement->setAttribute(myEndPosAttr, toString(moveResult.newFirstPos), undoList);
            // check if arrival lane has to be changed
            if (moveResult.newFirstLane) {
                // set new arrival lane
                myDemandElement->setAttribute(SUMO_ATTR_ARRIVALLANE, toString(moveResult.newFirstLane->getIndex()), undoList);
            }
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
