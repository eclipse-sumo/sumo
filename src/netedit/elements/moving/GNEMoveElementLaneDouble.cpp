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
#include "GNEMoveElementLaneSingle.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNEMoveElementLaneDouble::defaultSize = 10;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementLaneDouble::GNEMoveElementLaneDouble(GNEAttributeCarrier* element,
        SumoXMLAttr startPosAttr, double& startPosValue, SumoXMLAttr endPosAttr,
        double& endPosValue, bool& friendlyPosition) :
    GNEMoveElement(element),
    myStartPos(new GNEMoveElementLaneSingle(element, startPosAttr, startPosValue, friendlyPosition,
                                            GNEMoveElementLaneSingle::PositionType::STARPOS)),
    myEndPos(new GNEMoveElementLaneSingle(element, endPosAttr, endPosValue, friendlyPosition,
                                          GNEMoveElementLaneSingle::PositionType::ENDPOS)) {
}


GNEMoveElementLaneDouble::~GNEMoveElementLaneDouble() {
    delete myStartPos;
    delete myEndPos;
}


GNEMoveOperation*
GNEMoveElementLaneDouble::getMoveOperation() {
    const auto& parentLanes = myMovedElement->getHierarchicalElement()->getParentLanes();
    // get allow change lane
    const bool allowChangeLane = myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane();
    // fist check if we're moving only extremes
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get geometry points under cursor
        const auto& geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(myMovedElement->getGUIGlObject());
        // continue depending of clicked geometry point
        if (geometryPoints.empty()) {
            return nullptr;
        } else {
            if (geometryPoints.front() == 0) {
                // move start position
                return myStartPos->getMoveOperation();
            } else {
                // move end position
                return myEndPos->getMoveOperation();
            }
        }
    } else if ((myStartPos->myPosOverLane != INVALID_DOUBLE) && (myEndPos->myPosOverLane != INVALID_DOUBLE)) {
        // move both start and end positions depending of number of lanes
        if (parentLanes.size() > 1) {
            if (gViewObjectsHandler.isObjectSelected(parentLanes.front())) {
                return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPos->myPosOverLane, parentLanes.back(), myEndPos->myPosOverLane, true, false);
            } else if (gViewObjectsHandler.isObjectSelected(parentLanes.back())) {
                return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPos->myPosOverLane, parentLanes.back(), myEndPos->myPosOverLane, false, false);
            } else {
                // temporal, in the future will be allow, clicking in the intermediate lanes
                return nullptr;
            }
        } else {
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentLanes.front(), myStartPos->myPosOverLane, myEndPos->myPosOverLane, allowChangeLane);
        }
    } else if (myStartPos->myPosOverLane != INVALID_DOUBLE) {
        // move only start position
        return myStartPos->getMoveOperation();
    } else if (myEndPos->myPosOverLane != INVALID_DOUBLE) {
        // move only end position
        return myEndPos->getMoveOperation();
    } else {
        // start and end positions undefined, then nothing to move
        return nullptr;
    }
}


std::string
GNEMoveElementLaneDouble::getMovingAttribute(SumoXMLAttr key) const {
    // position attributes
    if (key == myStartPos->myPosAttr) {
        return myStartPos->getMovingAttribute(key);
    } else if (key == myEndPos->myPosAttr) {
        return myEndPos->getMovingAttribute(key);
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
            case SUMO_ATTR_LANES:
                return GNEAttributeCarrier::parseIDs(myMovedElement->getHierarchicalElement()->getParentLanes());
            case SUMO_ATTR_FRIENDLY_POS:
                return myStartPos->getMovingAttribute(key);
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
                return myMovedElement->getCommonAttribute(key);
        }
    }
}


double
GNEMoveElementLaneDouble::getMovingAttributeDouble(SumoXMLAttr key) const {
    // position attributes
    if (key == myStartPos->myPosAttr) {
        return myStartPos->myPosOverLane;
    } else if (key == myEndPos->myPosAttr) {
        return myEndPos->myPosOverLane;
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_CENTER:
                return (getStartFixedPositionOverLane(false) + getEndFixedPositionOverLane(false)) * 0.5;
            case SUMO_ATTR_LENGTH:
            case GNE_ATTR_SIZE:
                if (myMovedElement->isTemplate()) {
                    return myTemplateSize;
                } else {
                    return (getEndFixedPositionOverLane(false) - getStartFixedPositionOverLane(false));
                }
            default:
                throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
        }
    }
}


Position
GNEMoveElementLaneDouble::getMovingAttributePosition(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePosition(key);
}


PositionVector
GNEMoveElementLaneDouble::getMovingAttributePositionVector(SumoXMLAttr key) const {
    return myMovedElement->getCommonAttributePositionVector(key);
}


void
GNEMoveElementLaneDouble::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // position attributes
    if (key == myStartPos->myPosAttr) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else if (key == myEndPos->myPosAttr) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
            case SUMO_ATTR_LANES:
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
                    GNEChange_Attribute::changeAttribute(myMovedElement, myEndPos->myPosAttr, toString(getStartFixedPositionOverLane(true) + GNEAttributeCarrier::parse<double>(value)), undoList);
                }
                break;
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
    if (key == myStartPos->myPosAttr) {
        return myStartPos->isMovingAttributeValid(key, value);
    } else if (key == myEndPos->myPosAttr) {
        return myEndPos->isMovingAttributeValid(key, value);
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_LANE:
            case SUMO_ATTR_LANES:
                if (value.empty()) {
                    return false;
                } else {
                    return GNEAttributeCarrier::canParse<std::vector<GNELane*> >(myMovedElement->getNet(), value, true);
                }
            case SUMO_ATTR_FRIENDLY_POS:
                return myStartPos->isMovingAttributeValid(key, value);
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
                return myMovedElement->isCommonAttributeValid(key, value);
        }
    }
}


void
GNEMoveElementLaneDouble::setMovingAttribute(SumoXMLAttr key, const std::string& value) {
    // position attributes
    if (key == myStartPos->myPosAttr) {
        myStartPos->setMovingAttribute(key, value);
    } else if (key == myEndPos->myPosAttr) {
        myEndPos->setMovingAttribute(key, value);
    } else {
        // other attributes
        switch (key) {
            case SUMO_ATTR_FRIENDLY_POS:
                myStartPos->setMovingAttribute(key, value);
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
                return myMovedElement->setCommonAttribute(key, value);
        }
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
    } else if (!myStartPos->isMoveElementValid() || !myEndPos->isMoveElementValid()) {
        return false;
    } else if ((myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) &&
               (myStartPos->getFixedPositionOverLane(false) > (myEndPos->getFixedPositionOverLane(false) - POSITION_EPS))) {
        return false;
    } else {
        return true;
    }
}


std::string
GNEMoveElementLaneDouble::getMovingProblem() const {
    // first check if lanes are connected
    if (!GNEAdditional::areLaneConsecutives(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return TL("Lanes aren't consecutives");
    } else if (!GNEAdditional::areLaneConnected(myMovedElement->getHierarchicalElement()->getParentLanes())) {
        return TL("Lanes aren't connected");
    } else if (!myStartPos->isMoveElementValid()) {
        return myStartPos->getMovingProblem();
    } else if (!myEndPos->isMoveElementValid()) {
        return myEndPos->getMovingProblem();
    } else if ((myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) &&
               (myStartPos->getFixedPositionOverLane(false) > (myEndPos->getFixedPositionOverLane(false) - POSITION_EPS))) {
        return TL("starPos > (endPos - EPS)");
    } else {
        return "";
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
    // Fix both position
    myStartPos->fixMovingProblem();
    myEndPos->fixMovingProblem();
    if (myMovedElement->getHierarchicalElement()->getParentLanes().size() == 1) {
        // extra if starPos > endPos (endPos is dominant)
        const double finalLenght = myMovedElement->getHierarchicalElement()->getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
        const double maxStartPos = (myEndPos->myPosOverLane == INVALID_DOUBLE) ? finalLenght : (myEndPos->getFixedPositionOverLane(false) - POSITION_EPS);
        if (maxStartPos < POSITION_EPS) {
            myMovedElement->setAttribute(myStartPos->myPosAttr, "0", undolist);
            myMovedElement->setAttribute(myEndPos->myPosAttr, toString(POSITION_EPS), undolist);
        } else if (myStartPos->getFixedPositionOverLane(false) > maxStartPos) {
            // use truncate to avoid problem precission under certain conditions
            const double newStartPos = (std::trunc(maxStartPos * 1000) / 1000);
            myMovedElement->setAttribute(myStartPos->myPosAttr, toString(newStartPos), undolist);
        }
    }
}


void
GNEMoveElementLaneDouble::writeMoveAttributes(OutputDevice& device, const bool writeLength) const {
    // lane/s
    if (myMovedElement->getTagProperty()->hasAttribute(SUMO_ATTR_LANE)) {
        device.writeAttr(SUMO_ATTR_LANE, myMovedElement->getAttribute(SUMO_ATTR_LANE));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, myMovedElement->getAttribute(SUMO_ATTR_LANES));
    }
    // write start position
    if (myStartPos->myPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(myStartPos->myPosAttr)) {
        device.writeAttr(myStartPos->myPosAttr, myStartPos->myPosOverLane);
    }
    // write end position depending of lenght
    if (writeLength) {
        device.writeAttr(SUMO_ATTR_LENGTH, (myEndPos->myPosOverLane - myStartPos->myPosOverLane));
    } else if (myEndPos->myPosOverLane != myMovedElement->getTagProperty()->getDefaultDoubleValue(myEndPos->myPosAttr)) {
        device.writeAttr(myEndPos->myPosAttr, myEndPos->myPosOverLane);
    }
    // friendly position (only if true)
    if (myStartPos->myFriendlyPos) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myStartPos->myFriendlyPos);
    }
}


double
GNEMoveElementLaneDouble::getStartFixedPositionOverLane(const bool adjustGeometryFactor) const {
    if (myStartPos->getFixedPositionOverLane(adjustGeometryFactor) < 0) {
        return 0;
    } else if (myStartPos->getFixedPositionOverLane(adjustGeometryFactor) > (myEndPos->getFixedPositionOverLane(adjustGeometryFactor) - POSITION_EPS)) {
        return (myEndPos->getFixedPositionOverLane(adjustGeometryFactor) - POSITION_EPS);
    } else {
        return myStartPos->getFixedPositionOverLane(adjustGeometryFactor);
    }
}


double
GNEMoveElementLaneDouble::getEndFixedPositionOverLane(const bool adjustGeometryFactor) const {
    if (myEndPos->getFixedPositionOverLane(adjustGeometryFactor) < POSITION_EPS) {
        return POSITION_EPS;
    } else if (myStartPos->getFixedPositionOverLane(adjustGeometryFactor) > (myEndPos->getFixedPositionOverLane(adjustGeometryFactor) - POSITION_EPS)) {
        return myEndPos->getFixedPositionOverLane(adjustGeometryFactor);
    } else {
        return myEndPos->getFixedPositionOverLane(adjustGeometryFactor);
    }
}


void
GNEMoveElementLaneDouble::setMoveShape(const GNEMoveResult& moveResult) {
    // check if we're moving both points
    if ((moveResult.newFirstPos != INVALID_DOUBLE) && (moveResult.newLastPos != INVALID_DOUBLE)) {
        // change both position
        myStartPos->setMoveShape(moveResult);
        myEndPos->setMoveShape(moveResult);
        // set lateral offset
        myMovingLateralOffset = moveResult.firstLaneOffset;
    } else if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // change only start position
        myStartPos->setMoveShape(moveResult);
    } else if (moveResult.newLastPos != INVALID_DOUBLE) {
        myEndPos->setMoveShape(moveResult);
    }
    // update geometry
    myMovedElement->updateGeometry();
}


void
GNEMoveElementLaneDouble::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myMovedElement, TLF("position of %", myMovedElement->getTagStr()));
    // check if we're moving both points
    if ((moveResult.newFirstPos != INVALID_DOUBLE) && (moveResult.newLastPos != INVALID_DOUBLE)) {
        // set both positions
        myStartPos->commitMoveShape(moveResult, undoList);
        myEndPos->commitMoveShape(moveResult, undoList);
    } else if (moveResult.newFirstPos != INVALID_DOUBLE) {
        // set only start position
        myStartPos->commitMoveShape(moveResult, undoList);
    } else if (moveResult.newLastPos != INVALID_DOUBLE) {
        // set only end position
        myEndPos->commitMoveShape(moveResult, undoList);
    }
    // end change attribute
    undoList->end();
}


void
GNEMoveElementLaneDouble::setSize(const std::string& value, GNEUndoList* undoList) {
    const auto laneLength = myMovedElement->getHierarchicalElement()->getParentLanes().front()->getLaneShapeLength();
    const double newSize = GNEAttributeCarrier::parse<double>(value);
    // continue depending of values of start und end position
    if ((myStartPos->myPosOverLane != INVALID_DOUBLE) && (myEndPos->myPosOverLane != INVALID_DOUBLE)) {
        // get middle lengths
        const double center = (getStartFixedPositionOverLane(false) + getEndFixedPositionOverLane(false)) * 0.5;
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
        GNEChange_Attribute::changeAttribute(myMovedElement, myStartPos->myPosAttr, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPos->myPosAttr, toString(newEndPos), undoList);
        undoList->end();
    } else if (myStartPos->myPosOverLane != INVALID_DOUBLE) {
        double newStartPos = laneLength - newSize;
        // adjust new StartPos
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myStartPos->myPosAttr, toString(newStartPos), undoList);
        undoList->end();
    } else if (myEndPos->myPosOverLane != INVALID_DOUBLE) {
        double newEndPos = newSize;
        // adjust endPos
        if (newEndPos > laneLength) {
            newEndPos = laneLength;
        }
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPos->myPosAttr, toString(newEndPos), undoList);
        undoList->end();
    }
}

/****************************************************************************/
