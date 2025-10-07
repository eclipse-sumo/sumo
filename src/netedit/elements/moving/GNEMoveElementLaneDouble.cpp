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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEMoveElementLaneDouble.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNEMoveElementLaneDouble::defaultSize = 10;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element, GNELane* lane,
        SumoXMLAttr startPosAttr, double& startPosValue, SumoXMLAttr endPosAttr, double& endPosValue,
        bool& friendlyPosition) :
    GNEMoveElement(element),
    myStartPosAttr(startPosAttr),
    myStartPosValue(startPosValue),
    myEndPosAttr(endPosAttr),
    myEndPosPosValue(endPosValue),
    myFriendlyPosition(friendlyPosition) {
    // set parents
    if (lane) {
        element->getHierarchicalElement()->setParent<GNELane*>(lane);
    }
}


GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element, const std::vector<GNELane*>& lanes,
        SumoXMLAttr startPosAttr, double& startPosValue, SumoXMLAttr endPosAttr, double& endPosValue,
        bool& friendlyPosition) :
    GNEMoveElement(element),
    myStartPosAttr(startPosAttr),
    myStartPosValue(startPosValue),
    myEndPosAttr(endPosAttr),
    myEndPosPosValue(endPosValue),
    myFriendlyPosition(friendlyPosition) {
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
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosValue, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
        } else {
            // move end position
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), 0, myEndPosPosValue,
                                        allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
        }
    } else if ((myStartPosValue != INVALID_DOUBLE) && (myEndPosPosValue != INVALID_DOUBLE)) {
        // move both start and end positions
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosValue, myEndPosPosValue,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_BOTH);
    } else if (myStartPosValue != INVALID_DOUBLE) {
        // move only start position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPosValue, parentLanes.front()->getLaneShape().length2D() - POSITION_EPS,
                                    allowChangeLane, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
    } else if (myEndPosPosValue != INVALID_DOUBLE) {
        // move only end position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), 0, myEndPosPosValue,
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


std::string
GNEMoveElementLaneDouble::getMovingAttribute(const Parameterised* parameterised, SumoXMLAttr key) const {
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
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
                return myMovedElement->getHierarchicalElement()->getParentLanes().front()->getID();
            case SUMO_ATTR_FRIENDLY_POS:
                return toString(myFriendlyPosition);
            case GNE_ATTR_SHIFTLANEINDEX:
                return "";
            case SUMO_ATTR_LENGTH:
            case GNE_ATTR_SIZE:
                if (myMovedElement->isTemplate()) {
                    return toString(myTemplateSize);
                } else {
                    return toString(getMovingAttributeDouble(GNE_ATTR_SIZE));
                }
            case GNE_ATTR_FORCESIZE:
                return toString(myTemplateForceSize);
            case GNE_ATTR_REFERENCE:
                return SUMOXMLDefinitions::ReferencePositions.getString(myReferencePosition);
            default:
                return myMovedElement->getCommonAttribute(parameterised, key);
        }
    }
}


double
GNEMoveElementLaneDouble::getMovingAttributeDouble(SumoXMLAttr key) const {
    // position attributes
    if (key == myStartPosAttr) {
        return myStartPosValue;
    } else if (key == myEndPosAttr) {
        return myEndPosPosValue;
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_CENTER:
                return (getStartFixedPositionOverLane() + getEndFixedPositionOverLane()) * 0.5;
            case SUMO_ATTR_LENGTH:
            case GNE_ATTR_SIZE:
                if (myMovedElement->isTemplate()) {
                    return myTemplateSize;
                } else {
                    return (getEndFixedPositionOverLane() - getStartFixedPositionOverLane());
                }
            default:
                throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
        }
    }
}


void
GNEMoveElementLaneDouble::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // position attributes
    if ((key == myStartPosAttr) || (key == myEndPosAttr)) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
            case SUMO_ATTR_FRIENDLY_POS:
            case GNE_ATTR_SHIFTLANEINDEX:
            case GNE_ATTR_REFERENCE:
            case GNE_ATTR_FORCESIZE:
                GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
                break;
            case SUMO_ATTR_LENGTH:
                if (myMovedElement->isTemplate()) {
                    // use size value
                    GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
                } else {
                    // change end position
                    GNEChange_Attribute::changeAttribute(myMovedElement, myEndPosAttr, toString(getStartFixedPositionOverLane() + GNEAttributeCarrier::parse<double>(value)), undoList);
                }
            case GNE_ATTR_SIZE:
                if (myMovedElement->isTemplate()) {
                    GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
                } else {
                    setSize(value, undoList);
                }
                break;
            default:
                myMovedElement->setCommonAttribute(key, value, undoList);
                break;
        }
    }
}


bool
GNEMoveElementLaneDouble::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    // position attributes
    if (key == myStartPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_START)) {
            return true;
        } else if (GNEAttributeCarrier::canParse<double>(value)) {
            return SUMORouteHandler::isStopPosValid(GNEAttributeCarrier::parse<double>(value), getMovingAttributeDouble(key), myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
        } else {
            return false;
        }
    } else if (key == myEndPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_END)) {
            return true;
        } else if (GNEAttributeCarrier::canParse<double>(value)) {
            return SUMORouteHandler::isStopPosValid(getMovingAttributeDouble(SUMO_ATTR_STARTPOS), GNEAttributeCarrier::parse<double>(value), myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
        } else {
            return false;
        }
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
                if (myMovedElement->getNet()->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                    return true;
                } else {
                    return false;
                }
            case SUMO_ATTR_FRIENDLY_POS:
                return GNEAttributeCarrier::canParse<bool>(value);
            case GNE_ATTR_SHIFTLANEINDEX:
                return true;
            case SUMO_ATTR_LENGTH:
            case GNE_ATTR_SIZE:
                if (value.empty()) {
                    return false;
                } else {
                    return GNEAttributeCarrier::canParse<double>(value) && GNEAttributeCarrier::parse<double>(value) >= POSITION_EPS;
                }
            case GNE_ATTR_FORCESIZE:
                return GNEAttributeCarrier::canParse<bool>(value);
            case GNE_ATTR_REFERENCE:
                return SUMOXMLDefinitions::ReferencePositions.hasString(value);
            default:
                return myMovedElement->isCommonValid(key, value);
        }
    }
}


void
GNEMoveElementLaneDouble::setMovingAttribute(Parameterised* parameterised, SumoXMLAttr key, const std::string& value) {
    // position attributes
    if (key == myStartPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_START)) {
            myStartPosValue = INVALID_DOUBLE;
        } else {
            myStartPosValue = GNEAttributeCarrier::parse<double>(value);
        }
    } else if (key == myEndPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_END)) {
            myEndPosPosValue = INVALID_DOUBLE;
        } else {
            myEndPosPosValue = GNEAttributeCarrier::parse<double>(value);
        }
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_FRIENDLY_POS:
                myFriendlyPosition = GNEAttributeCarrier::parse<bool>(value);
                break;
            case SUMO_ATTR_LENGTH:
            case GNE_ATTR_SIZE:
                if (value.empty()) {
                    myTemplateSize = defaultSize;
                } else {
                    myTemplateSize = GNEAttributeCarrier::parse<double>(value);
                }
                break;
            case GNE_ATTR_FORCESIZE:
                myTemplateForceSize = GNEAttributeCarrier::parse<bool>(value);
                break;
            case GNE_ATTR_REFERENCE:
                myReferencePosition = SUMOXMLDefinitions::ReferencePositions.get(value);
                break;
            default:
                return myMovedElement->setCommonAttribute(parameterised, key, value);
        }
    }
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
        // adjust positions
        const double adjustedStartPosition = (myStartPosValue == INVALID_DOUBLE) ? 0 : (myStartPosValue < 0) ? (myStartPosValue + fromLength) : myStartPosValue;
        const double adjustedEndPosition = (myEndPosPosValue == INVALID_DOUBLE) ? toLength : (myEndPosPosValue < 0) ? (myEndPosPosValue + toLength) : myEndPosPosValue;
        // check conditions
        if (myFriendlyPosition) {
            return true;
        } else if (adjustedStartPosition < 0) {
            return false;
        } else if (adjustedStartPosition > fromLength) {
            return false;
        } else if (adjustedEndPosition < 0) {
            return false;
        } else if (adjustedEndPosition > toLength) {
            return false;
        } else if (myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) {
            // only if we have only one lane
            if ((adjustedStartPosition + POSITION_EPS) >= adjustedEndPosition) {
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
        // adjust positions
        const double adjustedStartPosition = (myStartPosValue == INVALID_DOUBLE) ? 0 : (myStartPosValue < 0) ? (myStartPosValue + fromLaneLength) : myStartPosValue;
        const double adjustedEndPosition = (myEndPosPosValue == INVALID_DOUBLE) ? toLaneLength : (myEndPosPosValue < 0) ? (myEndPosPosValue + toLaneLength) : myEndPosPosValue;
        // check conditions
        if (myFriendlyPosition) {
            return "";
        } else if (adjustedStartPosition < 0) {
            return TLF("% < 0", toString(myStartPosAttr));
        } else if (adjustedStartPosition > fromLaneLength) {
            return TLF("% > start lanes's length", toString(myStartPosAttr));
        } else if (adjustedEndPosition < 0) {
            return TLF("% < 0", toString(myEndPosAttr));
        } else if (adjustedEndPosition > toLaneLength) {
            return TLF("% > end lanes's length", toString(myEndPosAttr));
        } else if (myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) {
            // only if we have only one lane
            if ((adjustedStartPosition + POSITION_EPS) >= adjustedEndPosition) {
                return TLF("% >= %", toString(myStartPosAttr), toString(myEndPosAttr));
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
    myMovedElement->setAttribute(myStartPosAttr, toString(getStartFixedPositionOverLane()), undolist);
    myMovedElement->setAttribute(myEndPosAttr, toString(getEndFixedPositionOverLane()), undolist);
}


void
GNEMoveElementLaneDouble::writeMoveAttributes(OutputDevice& device) const {
    // lane/s
    if (myMovedElement->getTagProperty()->hasAttribute(SUMO_ATTR_LANE)) {
        device.writeAttr(SUMO_ATTR_LANE, myMovedElement->getAttribute(SUMO_ATTR_LANE));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, myMovedElement->getAttribute(SUMO_ATTR_LANES));
    }
    // write start position
    if (myStartPosValue != myMovedElement->getTagProperty()->getDefaultDoubleValue(myStartPosAttr)) {
        device.writeAttr(myStartPosAttr, myStartPosValue);
    }
    // write end position depending of lenght
    if (myMovedElement->hasAttribute(SUMO_ATTR_LENGTH)) {
        device.writeAttr(SUMO_ATTR_LENGTH, (myEndPosPosValue - myStartPosValue));
    } else if (myEndPosPosValue != myMovedElement->getTagProperty()->getDefaultDoubleValue(myEndPosAttr)) {
        device.writeAttr(myEndPosAttr, myEndPosPosValue);
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
    if (myStartPosValue == INVALID_DOUBLE) {
        return 0;
    } else {
        // get lane length and fixed end pos
        const double laneLength = firstLane->getParentEdge()->getNBEdge()->getFinalLength();
        const double fixedEndPos = getEndFixedPositionOverLane();
        // fix position
        double fixedPos = myStartPosValue;
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
    if (myEndPosPosValue == INVALID_DOUBLE) {
        return lastLane->getLaneShapeLength();
    } else {
        // get lane final and shape length
        const double laneLength = lastLane->getParentEdge()->getNBEdge()->getFinalLength();
        // fix position
        double fixedPos = myEndPosPosValue;
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
GNEMoveElementLaneDouble::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) {
        // change only start position
        myStartPosValue = moveResult.newFirstPos;
        // adjust startPos
        if (myStartPosValue > (myMovedElement->getAttributeDouble(myEndPosAttr) - POSITION_EPS)) {
            myStartPosValue = (myMovedElement->getAttributeDouble(myEndPosAttr) - POSITION_EPS);
        }
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // change only end position
        myEndPosPosValue = moveResult.newFirstPos;
        // adjust endPos
        if (myEndPosPosValue < (myMovedElement->getAttributeDouble(myStartPosAttr) + POSITION_EPS)) {
            myEndPosPosValue = (myMovedElement->getAttributeDouble(myStartPosAttr) + POSITION_EPS);
        }
    } else {
        // change both position
        myStartPosValue = moveResult.newFirstPos;
        myEndPosPosValue = moveResult.newLastPos;
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
        myMovedElement->setAttribute(myStartPosAttr, toString(moveResult.newFirstPos), undoList);
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) {
        // set only end position
        myMovedElement->setAttribute(myEndPosAttr, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        myMovedElement->setAttribute(myStartPosAttr, toString(moveResult.newFirstPos), undoList);
        myMovedElement->setAttribute(myEndPosAttr, toString(moveResult.newLastPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            myMovedElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}


void
GNEMoveElementLaneDouble::setSize(const std::string& value, GNEUndoList* undoList) {
    const auto laneLength = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getLaneShapeLength();
    const double newSize = GNEAttributeCarrier::parse<double>(value);
    // continue depending of values of start und end position
    if ((myStartPosValue != INVALID_DOUBLE) && (myEndPosPosValue != INVALID_DOUBLE)) {
        // get middle lengths
        const double center = (getStartFixedPositionOverLane() + getEndFixedPositionOverLane()) * 0.5;
        // calculate new lenghts
        double newStartPos = center - (newSize * 0.5);
        double newEndPos = center + (newSize * 0.5);
        // adjust positions
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        // set new start und end positions
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myStartPosAttr, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPosAttr, toString(newEndPos), undoList);
        undoList->end();
    } else if (myStartPosValue != INVALID_DOUBLE) {
        double newStartPos = laneLength - newSize;
        // adjust new StartPos
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myStartPosAttr, toString(newStartPos), undoList);
        undoList->end();
    } else if (myEndPosPosValue != INVALID_DOUBLE) {
        double newEndPos = newSize;
        // adjust endPos
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPosAttr, toString(newEndPos), undoList);
        undoList->end();
    }
}

/****************************************************************************/
