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
/// @file    GNEMoveElementEdgeDouble.cpp
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

#include "GNEMoveElementEdgeDouble.h"

// ===========================================================================
// static members
// ===========================================================================

const double GNEMoveElementEdgeDouble::defaultSize = 10;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMoveElementEdgeDouble::GNEMoveElementEdgeDouble(GNEAttributeCarrier* element,
        GNEEdge* fromEdge, SumoXMLAttr startPosAttr, double& startPosValue,
        GNEEdge* toEdge, SumoXMLAttr endPosAttr, double& endPosValue) :
    GNEMoveElement(element),
    myStartPosAttr(startPosAttr),
    myStartPosValue(startPosValue),
    myEndPosAttr(endPosAttr),
    myEndPosPosValue(endPosValue) {
    // set parents
    element->getHierarchicalElement()->setParents<GNEEdge*>({fromEdge, toEdge});
}


GNEMoveElementEdgeDouble::~GNEMoveElementEdgeDouble() {}


GNEMoveOperation*
GNEMoveElementEdgeDouble::getMoveOperation() {
    const auto& parentEdges = myMovedElement->getHierarchicalElement()->getParentEdges();
    // get allow change edge
    const bool allowChangeEdge = myMovedElement->getNet()->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeEdge();
    // fist check if we're moving only extremes
    if (myMovedElement->drawMovingGeometryPoints()) {
        // get geometry points under cursor
        const auto geometryPoints = gViewObjectsHandler.getSelectedGeometryPoints(myMovedElement->getGUIGlObject());
        // continue depending of moved element
        if (geometryPoints.empty()) {
            return nullptr;
        } else if (geometryPoints.front() == 0) {
            // move start position
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentEdges.front(), myStartPosValue, parentEdges.front()->getEdgeShape().length2D() - POSITION_EPS,
                                        allowChangeEdge, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
        } else {
            // move end position
            return new GNEMoveOperation(myMovedElement->getMoveElement(), parentEdges.front(), 0, myEndPosPosValue,
                                        allowChangeEdge, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
        }
    } else if ((myStartPosValue != INVALID_DOUBLE) && (myEndPosPosValue != INVALID_DOUBLE)) {
        // move both start and end positions
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentEdges.front(), myStartPosValue, myEndPosPosValue,
                                    allowChangeEdge, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_BOTH);
    } else if (myStartPosValue != INVALID_DOUBLE) {
        // move only start position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentEdges.front(), myStartPosValue, parentEdges.front()->getEdgeShape().length2D() - POSITION_EPS,
                                    allowChangeEdge, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST);
    } else if (myEndPosPosValue != INVALID_DOUBLE) {
        // move only end position
        return new GNEMoveOperation(myMovedElement->getMoveElement(), parentEdges.front(), 0, myEndPosPosValue,
                                    allowChangeEdge, GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST);
    } else {
        // start and end positions undefined, then nothing to move
        return nullptr;
    }
}


void
GNEMoveElementEdgeDouble::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to do here
}


std::string
GNEMoveElementEdgeDouble::getMovingAttribute(const Parameterised* parameterised, SumoXMLAttr key) const {
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
GNEMoveElementEdgeDouble::getMovingAttributeDouble(SumoXMLAttr key) const {
    // position attributes
    if (key == myStartPosAttr) {
        return myStartPosValue;
    } else if (key == myEndPosAttr) {
        return myEndPosPosValue;
    } else {
        throw InvalidArgument(myMovedElement->getTagStr() + " doesn't have a moving attribute of type '" + toString(key) + "'");
    }
}


void
GNEMoveElementEdgeDouble::setMovingAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // position attributes
    if ((key == myStartPosAttr) || (key == myEndPosAttr)) {
        GNEChange_Attribute::changeAttribute(myMovedElement, key, value, undoList);
    } else {
        myMovedElement->setCommonAttribute(key, value, undoList);
    }
}


bool
GNEMoveElementEdgeDouble::isMovingAttributeValid(SumoXMLAttr key, const std::string& value) const {
    // position attributes
    if (key == myStartPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_START)) {
            return true;
        } else if (GNEAttributeCarrier::canParse<double>(value)) {
            return SUMORouteHandler::isStopPosValid(GNEAttributeCarrier::parse<double>(value), getMovingAttributeDouble(key), myMovedElement->getHierarchicalElement()->getParentEdges().front()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
        } else {
            return false;
        }
    } else if (key == myEndPosAttr) {
        if (value.empty() || (value == GNEAttributeCarrier::LANE_END)) {
            return true;
        } else if (GNEAttributeCarrier::canParse<double>(value)) {
            return SUMORouteHandler::isStopPosValid(getMovingAttributeDouble(SUMO_ATTR_STARTPOS), GNEAttributeCarrier::parse<double>(value), myMovedElement->getHierarchicalElement()->getParentEdges().front()->getNBEdge()->getFinalLength(), POSITION_EPS, myFriendlyPosition);
        } else {
            return false;
        }
    } else {
        return myMovedElement->isCommonValid(key, value);
    }
}


void
GNEMoveElementEdgeDouble::setMovingAttribute(Parameterised* parameterised, SumoXMLAttr key, const std::string& value) {
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
        return myMovedElement->setCommonAttribute(parameterised, key, value);
    }
}

bool
GNEMoveElementEdgeDouble::isMoveElementValid() const {
    // obtain edge final length
    const double fromLength = myMovedElement->getHierarchicalElement()->getParentEdges().front()->getNBEdge()->getFinalLength();
    const double toLength = myMovedElement->getHierarchicalElement()->getParentEdges().back()->getNBEdge()->getFinalLength();
    // adjust positions
    const double adjustedStartPosition = (myStartPosValue == INVALID_DOUBLE) ? 0 : (myStartPosValue < 0) ? (myStartPosValue + fromLength) : myStartPosValue;
    const double adjustedEndPosition = (myEndPosPosValue == INVALID_DOUBLE) ? toLength : (myEndPosPosValue < 0) ? (myEndPosPosValue + toLength) : myEndPosPosValue;
    // check conditions
    if (adjustedStartPosition < 0) {
        return false;
    } else if (adjustedStartPosition > fromLength) {
        return false;
    } else if (adjustedEndPosition < 0) {
        return false;
    } else if (adjustedEndPosition > toLength) {
        return false;
    } else if (myMovedElement->getHierarchicalElement()->getParentEdges().size() == 1) {
        // only if we have only one edge
        if ((adjustedStartPosition + POSITION_EPS) >= adjustedEndPosition) {
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}


std::string
GNEMoveElementEdgeDouble::getMovingProblem() const {
    // obtain edge final lengths
    const double fromEdgeLength = myMovedElement->getHierarchicalElement()->getParentEdges().front()->getNBEdge()->getFinalLength();
    const double toEdgeLength = myMovedElement->getHierarchicalElement()->getParentEdges().back()->getNBEdge()->getFinalLength();
    // adjust positions
    const double adjustedStartPosition = (myStartPosValue == INVALID_DOUBLE) ? 0 : (myStartPosValue < 0) ? (myStartPosValue + fromEdgeLength) : myStartPosValue;
    const double adjustedEndPosition = (myEndPosPosValue == INVALID_DOUBLE) ? toEdgeLength : (myEndPosPosValue < 0) ? (myEndPosPosValue + toEdgeLength) : myEndPosPosValue;
    // check conditions
    if (adjustedStartPosition < 0) {
        return TLF("% < 0", toString(myStartPosAttr));
    } else if (adjustedStartPosition > fromEdgeLength) {
        return TLF("% > start edges's length", toString(myStartPosAttr));
    } else if (adjustedEndPosition < 0) {
        return TLF("% < 0", toString(myEndPosAttr));
    } else if (adjustedEndPosition > toEdgeLength) {
        return TLF("% > end edges's length", toString(myEndPosAttr));
    } else if (myMovedElement->getHierarchicalElement()->getParentEdges().size() == 1) {
        // only if we have only one edge
        if ((adjustedStartPosition + POSITION_EPS) >= adjustedEndPosition) {
            return TLF("% >= %", toString(myStartPosAttr), toString(myEndPosAttr));
        } else {
            return "";
        }
    } else {
        return "";
    }
}


void
GNEMoveElementEdgeDouble::fixMovingProblem() {
    const auto undolist = myMovedElement->getNet()->getViewNet()->getUndoList();
    // set fixed positions
    myMovedElement->setAttribute(myStartPosAttr, toString(getStartFixedPositionOverEdge()), undolist);
    myMovedElement->setAttribute(myEndPosAttr, toString(getEndFixedPositionOverEdge()), undolist);
}


double
GNEMoveElementEdgeDouble::getStartFixedPositionOverEdge() const {
    const auto& firstEdge = myMovedElement->getHierarchicalElement()->getParentEdges().front();
    // continue depending if we defined a end position
    if (myStartPosValue == INVALID_DOUBLE) {
        return 0;
    } else {
        // get edge length and fixed end pos
        const double edgeLength = firstEdge->getNBEdge()->getFinalLength();
        const double fixedEndPos = getEndFixedPositionOverEdge();
        // fix position
        double fixedPos = myStartPosValue;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += edgeLength;
        }
        // set length geometry factor
        fixedPos *= firstEdge->getLengthGeometryFactor();
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
GNEMoveElementEdgeDouble::getEndFixedPositionOverEdge() const {
    const auto& lastEdge = myMovedElement->getHierarchicalElement()->getParentEdges().back();
    // continue depending if we defined a end position
    if (myEndPosPosValue == INVALID_DOUBLE) {
        return lastEdge->getEdgeShapeLength();
    } else {
        // get edge final and shape length
        const double edgeLength = lastEdge->getNBEdge()->getFinalLength();
        // fix position
        double fixedPos = myEndPosPosValue;
        // adjust fixedPos
        if (fixedPos < 0) {
            fixedPos += edgeLength;
        }
        // set length geometry factor
        fixedPos *= lastEdge->getLengthGeometryFactor();
        // return depending of fixedPos
        if (fixedPos < POSITION_EPS) {
            return POSITION_EPS;
        } else if (fixedPos > lastEdge->getEdgeShapeLength()) {
            return lastEdge->getEdgeShapeLength();
        } else {
            return fixedPos;
        }
    }
}


void
GNEMoveElementEdgeDouble::setMoveShape(const GNEMoveResult& moveResult) {
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
GNEMoveElementEdgeDouble::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
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
    }
    // end change attribute
    undoList->end();
}


void
GNEMoveElementEdgeDouble::setSize(const std::string& value, GNEUndoList* undoList) {
    const auto edgeLength = myMovedElement->getHierarchicalElement()->getParentEdges().front()->getEdgeShapeLength();
    const double newSize = GNEAttributeCarrier::parse<double>(value);
    // continue depending of values of start und end position
    if ((myStartPosValue != INVALID_DOUBLE) && (myEndPosPosValue != INVALID_DOUBLE)) {
        // get middle lengths
        const double center = (getStartFixedPositionOverEdge() + getEndFixedPositionOverEdge()) * 0.5;
        // calculate new lenghts
        double newStartPos = center - (newSize * 0.5);
        double newEndPos = center + (newSize * 0.5);
        // adjust positions
        if (newStartPos < 0) {
            newStartPos = 0;
        }
        if (newEndPos > edgeLength) {
            newEndPos = edgeLength;
        }
        // set new start und end positions
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myStartPosAttr, toString(newStartPos), undoList);
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPosAttr, toString(newEndPos), undoList);
        undoList->end();
    } else if (myStartPosValue != INVALID_DOUBLE) {
        double newStartPos = edgeLength - newSize;
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
        if (newEndPos > edgeLength) {
            newEndPos = edgeLength;
        }
        undoList->begin(myMovedElement, TLF(" %'s size", myMovedElement->getTagStr()));
        GNEChange_Attribute::changeAttribute(myMovedElement, myEndPosAttr, toString(newEndPos), undoList);
        undoList->end();
    }
}

/****************************************************************************/
