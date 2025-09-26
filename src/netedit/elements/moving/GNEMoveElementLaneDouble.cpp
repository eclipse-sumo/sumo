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
/// @file    GNEMoveElementLaneDouble.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Class used for elements that can be moved over a lane with two positions
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

#include "GNEMoveElementLaneDouble.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element) :
    GNEMoveElement(element) {
}


GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
        GNELane* lane, const double startPos, const double endPos, const bool friendlyPosition) :
    GNEMoveElement(element),
    myStartPosOverLane(startPos),
    myEndPosPosOverLane(endPos),
    myFriendlyPosition(friendlyPosition),
    myAttributesFormat(attributesFormat) {
    // set parents
    element->getHierarchicalElement()->setParent<GNELane*>(lane);
}


GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element, AttributesFormat attributesFormat,
        const std::vector<GNELane*>& lanes, const double startPos, const double endPos, const bool friendlyPosition) :
    GNEMoveElement(element),
    myStartPosOverLane(startPos),
    myEndPosPosOverLane(endPos),
    myFriendlyPosition(friendlyPosition),
    myAttributesFormat(attributesFormat) {
    // set parents
    element->getHierarchicalElement()->setParents<GNELane*>(lanes);
}


GNEMoveElementLaneDouble::~GNEMoveElementLaneDouble() {}


GNEMoveOperation*
GNEMoveElementLaneDouble::getMoveOperation() {
    const auto& parentLanes = myMovedElement->getHierarchicalElement()->getParentLanes();
    // get allow change lane
    const bool allowChangeLane = myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
    // fist check if we're moving only extremes
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get geometry points under cursor
        const auto geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(myMovedElement->getGUIGlObject());
        // continue depending of moved element
        if (geometryPoints.empty()) {
            return nullptr;
        } else if (geometryPoints.front() == 0) {
            // move start position
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosOverLane, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
        } else {
            // move end position
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), 0, myEndPosPosOverLane,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
        }
    } else if ((myStartPosOverLane != INVALID_DOUBLE) && (myEndPosPosOverLane != INVALID_DOUBLE)) {
        // move both start and end positions
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosOverLane, myEndPosPosOverLane,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_BOTH);
    } else if (myStartPosOverLane != INVALID_DOUBLE) {
        // move only start position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosOverLane, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
    } else if (myEndPosPosOverLane != INVALID_DOUBLE) {
        // move only end position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), 0, myEndPosPosOverLane,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
    } else {
        // start and end positions undefined, then nothing to move
        return nullptr;
    }
}


void
GNEMoveElementLaneDouble::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


bool
GNEMoveElementLaneDouble::isMoveElementValid() const {
    // first check if lanes are connected
    if (!GNEAdditional::areLaneConsecutives(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return false;
    } else if (!GNEAdditional::areLaneConnected(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return false;
    } else {
        // obtain lane final length
        const double fromLength = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        const double toLength = myMovedElement->getHierarchicalElement()->getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        // check conditions
        if (myFriendlyPosition) {
            return true;
        } else if (myStartPosOverLane < 0) {
            return false;
        } else if (myStartPosOverLane > fromLength) {
            return false;
        } else if (myEndPosPosOverLane < 0) {
            return false;
        } else if (myEndPosPosOverLane > toLength) {
            return false;
        } else if (myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) {
            // only if we have only one lane
            if ((myStartPosOverLane + POSITION_EPS) >= myEndPosPosOverLane) {
                return false;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
}


std::string
GNEMoveElementLaneDouble::getMovingProblem() const {
    // first check if lanes are connected
    if (!GNEAdditional::areLaneConsecutives(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return TL("Lanes aren't consecutives");
    } else if (!GNEAdditional::areLaneConnected(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return TL("Lanes aren't connected");
    } else {
        // obtain lane final lengths
        const double fromLaneLength = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        const double toLaneLength = myMovedElement->getHierarchicalElement()->getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        // adjust positions (negative means start counting from backward)
        const double fixedStartPosition = (myStartPosOverLane == INVALID_DOUBLE) ? 0 : myStartPosOverLane < 0 ? (myStartPosOverLane + fromLaneLength) : myStartPosOverLane;
        const double fixedEndPosition = (myEndPosPosOverLane == INVALID_DOUBLE) ? 0 : myEndPosPosOverLane < 0 ? (myEndPosPosOverLane + toLaneLength) : myEndPosPosOverLane;
        // check conditions
        if (myFriendlyPosition) {
            return "";
        } else if (fixedStartPosition < 0) {
            return TLF("% < 0", toString(SUMO_ATTR_STARTPOS));
        } else if (fixedStartPosition > fromLaneLength) {
            return TLF("% > start lanes's length", toString(SUMO_ATTR_STARTPOS));
        } else if (fixedEndPosition < 0) {
            return TLF("% < 0", toString(SUMO_ATTR_ENDPOS));
        } else if (fixedEndPosition > toLaneLength) {
            return TLF("% > end lanes's length", toString(SUMO_ATTR_ENDPOS));
        } else if (myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) {
            // only if we have only one lane
            if ((fixedStartPosition + POSITION_EPS) >= fixedEndPosition) {
                return TLF("% >= %", toString(SUMO_ATTR_STARTPOS), toString(SUMO_ATTR_ENDPOS));
            } else {
                return "";
            }
        } else {
            return "";
        }
    }
}


void
GNEMoveElementLaneDouble::fixMovingProblem() {
    const auto undolist = myMovedElement->getNet()->getViewNet()->getUndoList();
    // iterate over all lanes and build connections
    for (int i = 1; i < (int)myMovedElement->getHierarchicalElement()->getParentLanes().size(); i++) {
        // get lanes
        const auto firstLane = myMovedElement->getHierarchicalElement()->getParentLanes().at(i - 1);
        const auto secondLane = myMovedElement->getHierarchicalElement()->getParentLanes().at(i);
        // search connection
        bool foundConnection = false;
        for (const auto& connection : firstLane->getParentEdge()->getGNEConnections()) {
            if ((connection->getLaneFrom() == firstLane) && (connection->getLaneTo() == secondLane)) {
                foundConnection = true;
                break;
            }
        }
        // check if connection exist
        if (!foundConnection) {
            // create new connection manually
            NBEdge::Connection newCon(firstLane->getIndex(), secondLane->getParentEdge()->getNBEdge(), secondLane->getIndex());
            // allow to undo creation of new lane
            undolist->add(new GNEChange_Connection(firstLane->getParentEdge(), newCon, false, true), true);
        }
    }
    // set fixed positions
    myMovedElement->setAttribute(SUMO_ATTR_STARTPOS, toString(getStartFixedPositionOverLane()), undolist);
    myMovedElement->setAttribute(SUMO_ATTR_ENDPOS, toString(getEndFixedPositionOverLane()), undolist);
}


void
GNEMoveElementLaneDouble::writeMoveAttributes(OutputDevice& device) const {
    // lane/s
    if (myMovedElement->getTagProperty()->hasAttribute(SUMO_ATTR_LANE)) {
        device.writeAttr(SUMO_ATTR_LANE, myMovedElement->getAttribute(SUMO_ATTR_LANE));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, myMovedElement->getAttribute(SUMO_ATTR_LANES));
    }
    // start and end positions
    if (myAttributesFormat == AttributesFormat::STARTPOS_ENDPOS) {
        if (myStartPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_STARTPOS)) {
            device.writeAttr(SUMO_ATTR_STARTPOS, myStartPosOverLane);
        }
        if (myEndPosPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_ENDPOS)) {
            device.writeAttr(SUMO_ATTR_ENDPOS, myEndPosPosOverLane);
        }
    }
    // pos and length
    if (myAttributesFormat == AttributesFormat::POS_LENGTH) {
        if (myStartPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_POSITION)) {
            device.writeAttr(SUMO_ATTR_POSITION, myStartPosOverLane);
        }
        if (myEndPosPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_LENGTH)) {
            device.writeAttr(SUMO_ATTR_LENGTH, (myEndPosPosOverLane - myStartPosOverLane));
        }
    }
    // start and end positions
    if (myAttributesFormat == AttributesFormat::POS_ENDPOS) {
        if (myStartPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_POSITION)) {
            device.writeAttr(SUMO_ATTR_POSITION, myStartPosOverLane);
        }
        if (myEndPosPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(SUMO_ATTR_ENDPOS)) {
            device.writeAttr(SUMO_ATTR_ENDPOS, myEndPosPosOverLane);
        }
    }
    // friendly position (only if true)
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
    }
}


double
GNEMoveElementLaneDouble::getStartFixedPositionOverLane() const {
    const auto& firstLane = myMovedElement->getHierarchicalElement()->getParentLanes().front();
    // continue depending if we defined a end position
    if (myStartPosOverLane == INVALID_DOUBLE) {
        return 0;
    } else {
        // get lane length and fixed end pos
        const double laneLength = firstLane->getParentEdge()->getNBEdge()->getFinalLength();
        const double fixedEndPos = getEndFixedPositionOverLane();
        // fix position
        double fixedPos = myStartPosOverLane;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
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
GNEMoveElementLaneDouble::getEndFixedPositionOverLane() const {
    const auto& lastLane = myMovedElement->getHierarchicalElement()->getParentLanes().back();
    // continue depending if we defined a end position
    if (myEndPosPosOverLane == INVALID_DOUBLE) {
        return lastLane->getLaneShapeLength();
    } else {
        // get lane final and shape length
        const double laneLength = lastLane->getParentEdge()->getNBEdge()->getFinalLength();
        // fix position
        double fixedPos = myEndPosPosOverLane;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
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
GNEMoveElementLaneDouble::adjustLaneMovableLength(const double newLength, GNEUndoList* undoList) {
    const auto laneLength = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getLaneShapeLength();
    auto newStartPos = myStartPosOverLane;
    auto newEndPos = myEndPosPosOverLane;
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
        undoList->begin(myMovedElement, TLF(" %'s length", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    } else if (newStartPos != INVALID_DOUBLE) {
        newStartPos = laneLength - newLength;
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        undoList->begin(myMovedElement, TLF(" %'s length", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_STARTPOS, toString(newStartPos), undoList);
        undoList->end();
    } else if (newEndPos != INVALID_DOUBLE) {
        newEndPos = newLength;
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        undoList->begin(myMovedElement, TLF(" %'s length", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, SUMO_ATTR_ENDPOS, toString(newEndPos), undoList);
        undoList->end();
    }
}


void
GNEMoveElementLaneDouble::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // change only start position
        myStartPosOverLane = moveResult.newFirstPos;
        // adjust startPos
        if (myStartPosOverLane > (myMovedElement->getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS)) {
            myStartPosOverLane = (myMovedElement->getAttributeDouble(SUMO_ATTR_ENDPOS) - POSITION_EPS);
        }
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // change only end position
        myEndPosPosOverLane = moveResult.newFirstPos;
        // adjust endPos
        if (myEndPosPosOverLane < (myMovedElement->getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS)) {
            myEndPosPosOverLane = (myMovedElement->getAttributeDouble(SUMO_ATTR_STARTPOS) + POSITION_EPS);
        }
    } else {
        // change both position
        myStartPosOverLane = moveResult.newFirstPos;
        myEndPosPosOverLane = moveResult.newLastPos;
        // set lateral offset
        myMovingLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementLaneDouble::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myMovedElement, "position of " + myMovedElement->getTagStr());
    // set attributes depending of operation type
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // set only start position
        myMovedElement->setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // set only end position
        myMovedElement->setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        myMovedElement->setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        myMovedElement->setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newLastPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            myMovedElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
