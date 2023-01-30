/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNEOverheadWire.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverheadWire::GNEOverheadWire(GNENet* net) :
    GNEAdditional("", net, GLO_OVERHEAD_WIRE_SEGMENT, SUMO_TAG_OVERHEAD_WIRE_SECTION,
                  GUIIconSubSys::getIcon(GUIIcon::OVERHEADWIRE), "", {}, {}, {}, {}, {}, {}),
                            myStartPos(0),
                            myEndPos(0),
myFriendlyPosition(false) {
    // reset default values
    resetDefaultValues();
}


GNEOverheadWire::GNEOverheadWire(const std::string& id, std::vector<GNELane*> lanes, GNEAdditional* substation, GNENet* net,
                                 const double startPos, const double endPos, const bool friendlyPos, const std::vector<std::string>& forbiddenInnerLanes,
                                 const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_OVERHEAD_WIRE_SEGMENT, SUMO_TAG_OVERHEAD_WIRE_SECTION,
                  GUIIconSubSys::getIcon(GUIIcon::OVERHEADWIRE), "", {}, {}, lanes, {substation}, {}, {}),
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
    // check modes and detector type
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return getMoveOperationMultiLane(myStartPos, myEndPos);
    } else {
        return nullptr;
    }
}


void
GNEOverheadWire::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_OVERHEAD_WIRE_SECTION);
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_SUBSTATIONID, getParentAdditionals().front()->getID());
    device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPos);
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    if (!myForbiddenInnerLanes.empty()) {
        device.writeAttr(SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN, myForbiddenInnerLanes);
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
            return (myStartPos >= 0) &&
                   (myEndPos >= 0) &&
                   ((myStartPos) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) &&
                   ((myEndPos) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
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
        return TL("lanes aren't consecutives");
    }
    // abort if lanes aren't connected
    if (!areLaneConnected(getParentLanes())) {
        return TL("lanes aren't connected");
    }
    // check positions over first lane
    if (myStartPos < 0) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    }
    if (myStartPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + TL(" > lanes's length"));
    }
    // check positions over last lane
    if (myEndPos < 0) {
        errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    }
    if (myEndPos > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + TL(" > lanes's length"));
    }
    // check separator
    if ((errorFirstLanePosition.size() > 0) && (errorLastLanePosition.size() > 0)) {
        separator = TL(" and ");
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
        GNEAdditionalHandler::fixMultiLanePosition(
            newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(),
            newEndPositionOverLane, getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position and endPosition
        setAttribute(SUMO_ATTR_STARTPOS, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myNet->getViewNet()->getUndoList());
    }
}


void
GNEOverheadWire::updateGeometry() {
    // compute path
    computePathElement();
}


Position
GNEOverheadWire::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNEOverheadWire::updateCenteringBoundary(const bool /* updateGrid */) {
    myAdditionalBoundary.reset();
    // add center
    myAdditionalBoundary.add(getPositionInView());
    // grow
    myAdditionalBoundary.grow(10);
}


void
GNEOverheadWire::splitEdgeGeometry(const double /* splitPosition */, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // obtain new list of lanes
    std::string newLanes = getNewListOfParents(originalElement, newElement);
    // update Lanes
    if (newLanes.size() > 0) {
        setAttribute(SUMO_ATTR_LANES, newLanes, undoList);
    }
}


void
GNEOverheadWire::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


void
GNEOverheadWire::computePathElement() {
    // calculate path
    myNet->getPathManager()->calculateConsecutivePathLanes(this, getParentLanes());
}


void
GNEOverheadWire::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // calculate overheadWire width
    const double overheadWireWidth = s.addSize.getExaggeration(s, lane);
    // check if E2 can be drawn
    if (s.drawAdditionals(overheadWireWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GUIGeometry overheadWireGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            overheadWireGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                                geometryDepartPos, geometryEndPos,      // extrem positions
                                                Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isFirstSegment()) {
            overheadWireGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                                geometryDepartPos, -1,                  // extrem positions
                                                Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isLastSegment()) {
            overheadWireGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                                -1, geometryEndPos,                     // extrem positions
                                                Position::INVALID, Position::INVALID);  // extra positions
        } else {
            overheadWireGeometry = lane->getLaneGeometry();
        }
        // get both geometries
        auto overheadWireGeometryTop = overheadWireGeometry;
        auto overheadWireGeometryBot = overheadWireGeometry;
        // move to sides
        overheadWireGeometryTop.moveGeometryToSide(overheadWireWidth * 0.5);
        overheadWireGeometryBot.moveGeometryToSide(overheadWireWidth * -0.5);
        // obtain color
        const RGBColor overheadWireColorTop = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorTop;
        const RGBColor overheadWireColorBot = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorBot;
        // avoid draw invisible elements
        if (overheadWireColorTop.alpha() != 0) {
            // Start drawing adding an gl identificator
            GLHelper::pushName(getGlID());
            // push layer matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            glTranslated(0, 0, getType() + offsetFront);
            // Set top color
            GLHelper::setColor(overheadWireColorTop);
            // draw top geometry
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), overheadWireGeometryTop, 0.2);
            // Set bot color
            GLHelper::setColor(overheadWireColorBot);
            // draw bot geometry
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), overheadWireGeometryBot, 0.2);
            // draw geometry points
            if (segment->isFirstSegment() && segment->isLastSegment()) {
                drawLeftGeometryPoint(myNet->getViewNet(), overheadWireGeometry.getShape().front(),  overheadWireGeometry.getShapeRotations().front(), overheadWireColorTop, true);
                drawRightGeometryPoint(myNet->getViewNet(), overheadWireGeometry.getShape().back(), overheadWireGeometry.getShapeRotations().back(), overheadWireColorTop, true);
            } else if (segment->isFirstSegment()) {
                drawLeftGeometryPoint(myNet->getViewNet(), overheadWireGeometry.getShape().front(), overheadWireGeometry.getShapeRotations().front(), overheadWireColorTop, true);
            } else if (segment->isLastSegment()) {
                drawRightGeometryPoint(myNet->getViewNet(), overheadWireGeometry.getShape().back(), overheadWireGeometry.getShapeRotations().back(), overheadWireColorTop, true);
            }
            // Pop layer matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
        }
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? overheadWireGeometry.getShape() : lane->getLaneShape();
        // check if mouse is over element
        mouseWithinGeometry(shape, overheadWireWidth);
        // inspect contour
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, overheadWireWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // front contour
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, overheadWireWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // delete contour
        if (myNet->getViewNet()->drawDeleteContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, overheadWireWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // select contour
        if (myNet->getViewNet()->drawSelectContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, overheadWireWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
    }
}


void
GNEOverheadWire::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // calculate overheadWire width
    const double overheadWireWidth = s.addSize.getExaggeration(s, fromLane);
    // check if E2 can be drawn
    if (s.drawAdditionals(overheadWireWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // obtain color
        const RGBColor overheadWireColorTop = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorTop;
        const RGBColor overheadWireColorBot = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorBot;
        // avoid draw invisible elements
        if (overheadWireColorTop.alpha() != 0) {
            // declare geometry
            GUIGeometry overheadWireGeometry({fromLane->getLaneShape().back(), toLane->getLaneShape().front()});
            // check if exist connection
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                overheadWireGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
            }
            // get both geometries
            auto overheadWireGeometryTop = overheadWireGeometry;
            auto overheadWireGeometryBot = overheadWireGeometry;
            // move to sides
            overheadWireGeometryTop.moveGeometryToSide(overheadWireWidth * 0.5);
            overheadWireGeometryBot.moveGeometryToSide(overheadWireWidth * -0.5);
            // Start drawing adding an gl identificator
            GLHelper::pushName(getGlID());
            // Add a draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            glTranslated(0, 0, getType() + offsetFront);
            // Set top color
            GLHelper::setColor(overheadWireColorTop);
            // draw top geometry
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), overheadWireGeometryTop, 0.2);
            // Set bot color
            GLHelper::setColor(overheadWireColorBot);
            // draw bot geometry
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), overheadWireGeometryBot, 0.2);
            // Pop last matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
        }
        // draw contours
        if (fromLane->getLane2laneConnections().exist(toLane)) {
            // check if mouse is over element
            mouseWithinGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(), overheadWireWidth);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        overheadWireWidth, 1, false, false);
            }
            // front contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        overheadWireWidth, 1, false, false);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        overheadWireWidth, 1, false, false);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        overheadWireWidth, 1, false, false);
            }
        }
    }
}


std::string
GNEOverheadWire::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_SUBSTATIONID:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_STARTPOS:
            return toString(myStartPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return toString(myForbiddenInnerLanes);
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
            if (myStartPos < 0) {
                return 0;
            } else if (myStartPos > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
                return getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
            } else {
                return myStartPos;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPos < 0) {
                return 0;
            } else if (myEndPos > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
                return getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
            } else {
                return myEndPos;
            }
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
        case SUMO_ATTR_SUBSTATIONID:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
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
            if (value == getID()) {
                return true;
            } else if (isValidAdditionalID(value)) {
                return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_OVERHEAD_WIRE_SECTION, value, false) == nullptr);
            } else {
                return false;
            }
        case SUMO_ATTR_SUBSTATIONID:
            if (value.empty()) {
                return false;
            } else {
                return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRACTION_SUBSTATION, value, false) != nullptr);
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
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return true;
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
        case SUMO_ATTR_SUBSTATIONID:
            replaceAdditionalParent(SUMO_TAG_TRACTION_SUBSTATION, value, 0);
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
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            myForbiddenInnerLanes = parse<std::vector<std::string> >(value);
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
        // set both positions
        setAttribute(SUMO_ATTR_STARTPOS, toString(moveResult.newFirstPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos), undoList);
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
