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
/// @file    GNEMoveElementLaneSingle.cpp
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
// static members
// ===========================================================================

const std::string GNEMoveElementLaneSingle::PositionType::SINGLE = "single";
const std::string GNEMoveElementLaneSingle::PositionType::STARPOS = TL("lane start");
const std::string GNEMoveElementLaneSingle::PositionType::ENDPOS = TL("lane end");

// ===========================================================================
// Method definitions
// ===========================================================================

GNEMoveElementLaneSingle::GNEMoveElementLaneSingle(GNEAttributeCarrier* element,
        SumoXMLAttr posAttr, double& position, bool& friendlyPos, const std::string& defaultBehavior) :
    GNEMoveElement(element),
    myPosAttr(posAttr),
    myPosOverLane(position),
    myFriendlyPos(friendlyPos),
    myPositionType(defaultBehavior) {
}


GNEMoveElementLaneSingle::~GNEMoveElementLaneSingle() {}


GNEMoveOperation*
GNEMoveElementLaneSingle::getMoveOperation() {
    // check if allow change lane is enabled
    const bool allowChangeLane = myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
    // continue depending if we're moving the start or the end position
    if (myPositionType == PositionType::ENDPOS) {
        return new GNEMoveOperation(this, myMovedElement->getHierarchicalElement()->getParentLanes().front(), INVALID_DOUBLE,
                                    myMovedElement->getHierarchicalElement()->getParentLanes().back(), myPosOverLane, false, allowChangeLane);
    } else {
        return new GNEMoveOperation(this, myMovedElement->getHierarchicalElement()->getParentLanes().front(), myPosOverLane,
                                    myMovedElement->getHierarchicalElement()->getParentLanes().back(), INVALID_DOUBLE, true, allowChangeLane);
    }
}


std::string
GNEMoveElementLaneSingle::getMovingAttribute(SumoXMLAttr key) const {
    if (key == myPosAttr) {
        if ((myPosOverLane == INVALID_DOUBLE) && (myPositionType != PositionType::SINGLE)) {
            return myPositionType;
        } else {
            return toString(myPosOverLane);
        }
    } else {
        switch (key) {
            case SUMO_ATTR_LANE:
                return myMovedElement->getHierarchicalElement()->getParentLanes().front()->getID();
            case SUMO_ATTR_FRIENDLY_POS:
                return toString(myFriendlyPos);
            default:
                return myMovedElement->getCommonAttribute(key);
        }
    }
}


double
GNEMoveElementLaneSingle::getMovingAttributeDouble(SumoXMLAttr key) const {
    if (key == myPosAttr) {
        return myPosOverLane;
    } else {
        return myMovedElement->getCommonAttributeDouble(key);
    }
}


Position
GNEMoveElementLaneSingle::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementLaneSingle::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementLaneSingle::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (key == myPosAttr) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else {
        switch (key) {
            case SUMO_ATTR_LANE:
            case SUMO_ATTR_FRIENDLY_POS:
                GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
                break;
            default:
                myMovedElement->setCommonAttribute(key, value, undoList);
                break;
        }
    }
}


bool
GNEMoveElementLaneSingle::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    if (key == myPosAttr) {
        if ((myPositionType != PositionType::SINGLE) && (value.empty() || (value == myPositionType))) {
            return true;
        } else {
            return GNEAttributeCarrier::canParse<double>(value);
        }
    } else {
        switch (key) {
            case SUMO_ATTR_LANE:
                if (myMovedElement->getNet()->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                    return true;
                } else {
                    return false;
                }
            case SUMO_ATTR_FRIENDLY_POS:
                return GNEAttributeCarrier::canParse<bool>(value);
            default:
                return myMovedElement->isCommonAttributeValid(key, value);
        }
    }
}


void
GNEMoveElementLaneSingle::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    if (key == myPosAttr) {
        if (value.empty()) {
            myPosOverLane = INVALID_DOUBLE;
        } else if ((value == PositionType::STARPOS) && (myPositionType == PositionType::STARPOS)) {
            myPosOverLane = INVALID_DOUBLE;
        } else if ((value == PositionType::ENDPOS) && (myPositionType == PositionType::ENDPOS)) {
            myPosOverLane = INVALID_DOUBLE;
        } else {
            myPosOverLane = GNEAttributeCarrier::parse<double>(value);
        }
    } else {
        switch (key) {
            case SUMO_ATTR_FRIENDLY_POS:
                myFriendlyPos = GNEAttributeCarrier::parse<bool>(value);
                break;
            default:
                myMovedElement->setCommonAttribute(key, value);
                break;
        }
    }
}


void
GNEMoveElementLaneSingle::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


bool
GNEMoveElementLaneSingle::isMoveElementValid() const {
    // obtain lane final length
    const double laneLenght = (myPositionType == PositionType::ENDPOS) ? myMovedElement->getHierarchicalElement()->getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength() :
                              myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // adjust position (negative means start counting from backward)
    const double adjustedPosition = (myPosOverLane == INVALID_DOUBLE) ? 0 : (myPosOverLane < 0) ? (myPosOverLane + laneLenght) : myPosOverLane;
    // check conditions
    if (myFriendlyPos) {
        return true;
    } else if (adjustedPosition < 0) {
        return false;
    } else if (adjustedPosition > laneLenght) {
        return false;
    } else if ((myPositionType == PositionType::STARPOS) && (adjustedPosition > (laneLenght - POSITION_EPS))) {
        return false;
    } else {
        return true;
    }
}


std::string
GNEMoveElementLaneSingle::getMovingProblem() const {
    // obtain lane final length
    const double laneLenght = (myPositionType == PositionType::ENDPOS) ? myMovedElement->getHierarchicalElement()->getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength() :
                              myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // adjust position (negative means start counting from backward)
    const double adjustedPosition = (myPosOverLane == INVALID_DOUBLE) ? 0 : (myPosOverLane < 0) ? (myPosOverLane + laneLenght) : myPosOverLane;
    // check conditions
    if (myFriendlyPos) {
        return "";
    } else if (adjustedPosition < 0) {
        return TLF("% < 0", toString(myPosAttr));
    } else if (adjustedPosition > laneLenght) {
        return TLF("% > length of lane", toString(myPosAttr));
    } else if ((myPositionType == PositionType::STARPOS) && (adjustedPosition > (laneLenght - POSITION_EPS))) {
        return TLF("% > (length of lane - EPS)", toString(myPosAttr));
    } else {
        return "";
    }
}


void
GNEMoveElementLaneSingle::fixMovingProblem() {
    // obtain lane final length
    const double laneLenght = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // adjust position (negative means start counting from backward)
    const double adjustedPosition = (myPosOverLane == INVALID_DOUBLE) ? 0 : (myPosOverLane < 0) ? (myPosOverLane + laneLenght) : myPosOverLane;
    // check conditions
    if (adjustedPosition < 0) {
        myMovedElement->setAttribute(myPosAttr, "0", myMovedElement->getNet()->getViewNet()->getUndoList());
    } else if (adjustedPosition > laneLenght) {
        myMovedElement->setAttribute(myPosAttr, toString(laneLenght), myMovedElement->getNet()->getViewNet()->getUndoList());
    } else if ((myPositionType == PositionType::STARPOS) && (adjustedPosition > (laneLenght - POSITION_EPS))) {
        myMovedElement->setAttribute(myPosAttr, toString(laneLenght - POSITION_EPS), myMovedElement->getNet()->getViewNet()->getUndoList());
    }
}


void
GNEMoveElementLaneSingle::writeMoveAttributes(OutputDevice& device) const {
    // lane
    device.writeAttr(SUMO_ATTR_LANE, myMovedElement->getAttribute(SUMO_ATTR_LANE));
    // position (don't write if is an invalid double, except in no default)
    if ((myPositionType == PositionType::SINGLE) || (myPosOverLane != INVALID_DOUBLE)) {
        device.writeAttr(myPosAttr, myPosOverLane);
    }
    // friendly position (only if true)
    if (myFriendlyPos) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPos);
    }
}


double
GNEMoveElementLaneSingle::getFixedPositionOverLane(const bool adjustGeometryFactor) const {
    // get lane depending of type
    const auto& lane = (myPositionType == PositionType::ENDPOS) ? myMovedElement->getHierarchicalElement()->getParentLanes().back() : myMovedElement->getHierarchicalElement()->getParentLanes().front();
    const double laneLength = lane->getParentEdge()->getNBEdge()->getFinalLength();
    // continue depending if we defined a end position
    if (myPosOverLane == INVALID_DOUBLE) {
        if (myPositionType == PositionType::ENDPOS) {
            return adjustGeometryFactor ? (laneLength * lane->getLengthGeometryFactor()) : laneLength;
        } else {
            return 0;
        }
    } else {
        // fix position
        double fixedPos = myPosOverLane;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += laneLength;
        }
        // set length geometry factor
        if (adjustGeometryFactor) {
            // adjust geometry factor
            fixedPos *= lane->getLengthGeometryFactor();
            // return depending of fixedPos
            if (fixedPos < 0) {
                return 0;
            } else if (fixedPos > lane->getLaneShapeLength()) {
                return lane->getLaneShapeLength();
            } else {
                return fixedPos;
            }
        } else {
            // return depending of fixedPos
            if (fixedPos < 0) {
                return 0;
            } else if (fixedPos > laneLength) {
                return laneLength;
            } else {
                return fixedPos;
            }
        }
    }
}


void
GNEMoveElementLaneSingle::setMoveShape(const GNEMoveResult& moveResult) {
    if (myPositionType == PositionType::ENDPOS) {
        // change position
        myPosOverLane = moveResult.newLastPos;
        // set lateral offset
        myMovingLateralOffset = moveResult.lastLaneOffset;
    } else {
        // change position
        myPosOverLane = moveResult.newFirstPos;
        // set lateral offset
        myMovingLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementLaneSingle::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // reset lateral offset
    myMovingLateralOffset = 0;
    // begin change attribute
    undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
    // set position
    if (myPositionType == PositionType::ENDPOS) {
        myMovedElement->setAttribute(myPosAttr, toString(moveResult.newLastPos), undoList);
        // check if lane has to be changed
        if (moveResult.newLastLane && (moveResult.newLastLane != myMovedElement->getHierarchicalElement()->getParentLanes().back())) {
            // set new lane
            myMovedElement->setAttribute(SUMO_ATTR_LANE, moveResult.newLastLane->getID(), undoList);
        }
    } else {
        myMovedElement->setAttribute(myPosAttr, toString(moveResult.newFirstPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane && (moveResult.newFirstLane != myMovedElement->getHierarchicalElement()->getParentLanes().front())) {
            // set new lane
            myMovedElement->setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}

/****************************************************************************/
