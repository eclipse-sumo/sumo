/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEOverheadWire.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEOverheadWire.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverheadWire::GNEOverheadWire(SumoXMLTag tag, GNENet* net) :
    GNEAdditional("", net, GLO_OVERHEAD_WIRE_SEGMENT, SUMO_TAG_OVERHEAD_WIRE_SECTION, "",
        {}, {}, {}, {}, {}, {}),
    myStartPos(0),
    myEndPos(0),
    myFriendlyPosition(false) {
    // reset default values
    resetDefaultValues();
}


GNEOverheadWire::GNEOverheadWire(const std::string& id, std::vector<GNELane*> lanes, GNEAdditional *substation, GNENet* net, 
        const double startPos, const double endPos, const bool friendlyPos, const std::vector<std::string>& forbiddenInnerLanes, 
        const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_OVERHEAD_WIRE_SEGMENT, SUMO_TAG_OVERHEAD_WIRE_SECTION, "",
        {}, {}, {}, {}, {}, {}),
    Parameterised(parameters),
    myStartPos(startPos),
    myEndPos(endPos),
    myFriendlyPosition(friendlyPos),
    myForbiddenInnerLanes(forbiddenInnerLanes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEOverheadWire::~GNEOverheadWire() {
}


GNEMoveOperation* 
GNEOverheadWire::getMoveOperation() {
    //
    return nullptr;
}


void
GNEOverheadWire::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_OVERHEAD_WIRE_SECTION);
    device.writeAttr(SUMO_ATTR_ID, getID());

    ///

    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    // write parameters
    writeParams(device);
    device.closeTag();
}


bool
GNEOverheadWire::isAdditionalValid() const {
    // first check if there is connection between all consecutive lanes
    if (areLaneConsecutives(getParentLanes())) {
        // with friendly position enabled position are "always fixed"
        if (myFriendlyPosition) {
            return true;
        } else {
            return (myStartPos >= 0) && ((myStartPos) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) &&
                    (myEndPos >= 0) && ((myEndPos) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
        }
    } else {
        return false;
    }
}


std::string
GNEOverheadWire::getAdditionalProblem() const {
    // declare variable for error position
    std::string errorFirstLanePosition, separator, errorLastLanePosition;
    // abort if lanes aren't consecutives
    if (!areLaneConsecutives(getParentLanes())) {
        return "lanes aren't consecutives";
    }
    // check positions over first lane
    if (myStartPos < 0) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    }
    if (myStartPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + " > lanes's length");
    }
    // check positions over last lane
    if (myEndPos < 0) {
        errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    }
    if (myEndPos > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
    }
    // check separator
    if ((errorFirstLanePosition.size() > 0) && (errorLastLanePosition.size() > 0)) {
        separator = " and ";
    }
    // return error message
    return errorFirstLanePosition + separator + errorLastLanePosition;
}


void
GNEOverheadWire::fixAdditionalProblem() {
    if (!areLaneConsecutives(getParentLanes())) {
        // build connections between all consecutive lanes
        bool foundConnection = true;
        int i = 0;
        // iterate over all lanes, and stop if myE2valid is false
        while (i < ((int)getParentLanes().size() - 1)) {
            // change foundConnection to false
            foundConnection = false;
            // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
            for (const auto& connection : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
                if ((connection->getLaneFrom() == getParentLanes().at(i)) && (connection->getLaneTo() == getParentLanes().at(i + 1))) {
                    foundConnection = true;
                }
            }
            // if connection wasn't found
            if (!foundConnection) {
                // create new connection manually
                NBEdge::Connection newCon(getParentLanes().at(i)->getIndex(), getParentLanes().at(i + 1)->getParentEdge()->getNBEdge(), getParentLanes().at(i + 1)->getIndex());
                // allow to undo creation of new lane
                myNet->getViewNet()->getUndoList()->add(new GNEChange_Connection(getParentLanes().at(i)->getParentEdge(), newCon, false, true), true);
            }
            // update lane iterator
            i++;
        }
    } else {
        // declare new positions
        double newPositionOverLane = myStartPos;
        double newEndPositionOverLane = myEndPos;
        // fix pos and length checkAndFixDetectorPosition
        GNEAdditionalHandler::fixE2MultiLanePosition(
            newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(),
            newEndPositionOverLane, getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position and endPosition
        setAttribute(SUMO_ATTR_STARTPOS, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myNet->getViewNet()->getUndoList());
    }
}


void
GNEOverheadWire::updateGeometry() {
    // check E2 detector
    if (myTagProperty.getTag() == GNE_TAG_E2DETECTOR_MULTILANE) {
        // compute path
        computePathElement();
    } else {
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
        // update centering boundary without updating grid
        updateCenteringBoundary(false);
    }
}


Position 
GNEOverheadWire::getPositionInView() const {
    //
    return Position(); 
}


void 
GNEOverheadWire::updateCenteringBoundary(const bool updateGrid) {
    //
}


void
GNEOverheadWire::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    //
}


void
GNEOverheadWire::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


std::string
GNEOverheadWire::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_STARTPOS:
            return toString(myStartPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEOverheadWire::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            return myStartPos;
        case SUMO_ATTR_LENGTH:
            return (myEndPos - myStartPos);
        case SUMO_ATTR_ENDPOS:
            return myEndPos;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map& 
GNEOverheadWire::getACParametersMap() const {
    return getParametersMap();
}


void
GNEOverheadWire::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEOverheadWire::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (isValidDetectorID(value)) {
                if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR) {
                    return (myNet->getAttributeCarriers()->retrieveAdditional(GNE_TAG_E2DETECTOR_MULTILANE, value, false) == nullptr);
                } else {
                    return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_E2DETECTOR, value, false) == nullptr);
                }
            } else {
                return false;
            }
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else if (canParse<std::vector<GNELane*> >(myNet, value, false)) {
                // check if lanes are consecutives
                return lanesConsecutives(parse<std::vector<GNELane*> >(myNet, value));
            } else {
                return false;
            }
        case SUMO_ATTR_STARTPOS:
            return canParse<double>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEOverheadWire::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEOverheadWire::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEOverheadWire::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEOverheadWire::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_STARTPOS:
            myStartPos = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPos = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEOverheadWire::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // change only start position
        myStartPos = moveResult.newFirstPos;
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // change only end position
        myEndPos = moveResult.newFirstPos;
    } else {
        // change both position
        myStartPos = moveResult.newFirstPos;
        myEndPos = moveResult.newSecondPos;
        // set lateral offset
        myMoveElementLateralOffset = moveResult.firstLaneOffset;
    }
    // update geometry
    updateGeometry();
}


void
GNEOverheadWire::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
    // set attributes depending of operation type
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // set only start position
        setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // set only end position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both
        setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos), undoList);
        // check if lane has to be changed
        if (moveResult.newFirstLane) {
            // set new lane
            setAttribute(SUMO_ATTR_LANE, moveResult.newFirstLane->getID(), undoList);
        }
    }
    // end change attribute
    undoList->end();
}


double
GNEOverheadWire::getStartGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // get startPosition
    double fixedPos = myStartPos;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < 0) {
        return 0;
    } else if (fixedPos > (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS)) {
        return (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS);
    } else {
        return fixedPos;
    }
}


double
GNEOverheadWire::getEndGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
    // get endPosition
    double fixedPos = myEndPos;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().back()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < POSITION_EPS) {
        return POSITION_EPS;
    } else if (fixedPos > getParentLanes().back()->getLaneShapeLength()) {
        return getParentLanes().back()->getLaneShapeLength();
    } else {
        return fixedPos;
    }
}

/****************************************************************************/
