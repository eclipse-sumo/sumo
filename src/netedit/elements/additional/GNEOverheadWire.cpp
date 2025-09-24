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
/// @file    GNEOverheadWire.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEOverheadWire.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverheadWire::GNEOverheadWire(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_OVERHEAD_WIRE_SECTION, ""),
    GNEMoveElementLaneDouble(this) {
}


GNEOverheadWire::GNEOverheadWire(const std::string& id, GNENet* net, const std::string& filename, std::vector<GNELane*> lanes, GNEAdditional* substation,
                                 const double startPos, const double endPos, const bool friendlyPos, const std::vector<std::string>& forbiddenInnerLanes,
                                 const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_OVERHEAD_WIRE_SECTION, ""),
    GNEMoveElementLaneDouble(this, lanes, startPos, endPos, friendlyPos),
    Parameterised(parameters),
    myForbiddenInnerLanes(forbiddenInnerLanes) {
    // set parents
    setParent<GNEAdditional*>(substation);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEOverheadWire::~GNEOverheadWire() {
}


GNEMoveElement*
GNEOverheadWire::getMoveElement() {
    return this;
}


void
GNEOverheadWire::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_OVERHEAD_WIRE_SECTION);
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_SUBSTATIONID, getParentAdditionals().front()->getID());
    device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
    device.writeAttr(SUMO_ATTR_STARTPOS, myStartPosOverLane);
    device.writeAttr(SUMO_ATTR_ENDPOS, myEndPosPosOverLane);
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, myFriendlyPosition);
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
            return (myStartPosOverLane >= 0) &&
                   (myEndPosPosOverLane >= 0) &&
                   ((myStartPosOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) &&
                   ((myEndPosPosOverLane) <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
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
    if (myStartPosOverLane < 0) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + " < 0");
    }
    if (myStartPosOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
        errorFirstLanePosition = (toString(SUMO_ATTR_STARTPOS) + TL(" > lanes's length"));
    }
    // check positions over last lane
    if (myEndPosPosOverLane < 0) {
        errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
    }
    if (myEndPosPosOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
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
    // first check if lanes are consecutives
    if (!areLaneConsecutives(getParentLanes())) {
        // build connections between all consecutive lanes
        bool foundConnection = true;
        int i = 0;
        // iterate over all lanes, and stop if myE2valid is false
        while (i < ((int)getParentLanes().size() - 1)) {
            // change foundConnection to false
            foundConnection = false;
            // if a connection between "from" lane and "to" lane of connection is found, change myE2valid to true again
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
    } else if (getParentLanes().size() == 1) {
        // make a copy of start and end positions over lane
        double startPos = myStartPosOverLane;
        double endPos = myEndPosPosOverLane;
        // fix start and end positions using fixLaneDoublePosition
        GNEAdditionalHandler::fixLaneDoublePosition(startPos, endPos, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
        // set new start and end positions
        setAttribute(SUMO_ATTR_STARTPOS, toString(startPos), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_ENDPOS, toString(endPos), myNet->getViewNet()->getUndoList());
    } else {
        // set fixed positions
        setAttribute(SUMO_ATTR_STARTPOS, toString(getStartFixedPositionOverLane()), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_ENDPOS, toString(getEndFixedPositionOverLane()), myNet->getViewNet()->getUndoList());
    }
}


bool
GNEOverheadWire::checkDrawMoveContour() const {
    return false;
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
    // nothing to update
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
    myNet->getNetworkPathManager()->calculateConsecutivePathLanes(this, getParentLanes());
}


void
GNEOverheadWire::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // calculate overheadWire width
    const double overheadWireWidth = s.addSize.getExaggeration(s, segment->getLane());
    // check if E2 can be drawn
    if (segment->getLane() && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // get detail level
        const auto d = s.getDetailLevel(overheadWireWidth);
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_STARTPOS);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GUIGeometry overheadWireGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            overheadWireGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                                geometryDepartPos,
                                                Position::INVALID,
                                                geometryEndPos,
                                                Position::INVALID);
        } else if (segment->isFirstSegment()) {
            overheadWireGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                                geometryDepartPos,
                                                Position::INVALID,
                                                -1,
                                                Position::INVALID);
        } else if (segment->isLastSegment()) {
            overheadWireGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                                -1,
                                                Position::INVALID,
                                                geometryEndPos,
                                                Position::INVALID);
        } else {
            overheadWireGeometry = segment->getLane()->getLaneGeometry();
        }
        // get both geometries
        auto overheadWireGeometryTop = overheadWireGeometry;
        auto overheadWireGeometryBot = overheadWireGeometry;
        // move to sides
        overheadWireGeometryTop.moveGeometryToSide(overheadWireWidth * 0.5);
        overheadWireGeometryBot.moveGeometryToSide(overheadWireWidth * -0.5);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // obtain color
            const RGBColor overheadWireColorTop = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorTop;
            const RGBColor overheadWireColorBot = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorBot;
            // push layer matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            glTranslated(0, 0, getType() + offsetFront);
            // Set top color
            GLHelper::setColor(overheadWireColorTop);
            // draw top geometry
            GUIGeometry::drawGeometry(d, overheadWireGeometryTop, 0.2);
            // Set bot color
            GLHelper::setColor(overheadWireColorBot);
            // draw bot geometry
            GUIGeometry::drawGeometry(d, overheadWireGeometryBot, 0.2);
            // draw geometry points
            if (segment->isFirstSegment() && segment->isLastSegment()) {
                drawLeftGeometryPoint(s, d, overheadWireGeometry.getShape().front(),  overheadWireGeometry.getShapeRotations().front(), overheadWireColorTop, true);
                drawRightGeometryPoint(s, d, overheadWireGeometry.getShape().back(), overheadWireGeometry.getShapeRotations().back(), overheadWireColorTop, true);
            } else if (segment->isFirstSegment()) {
                drawLeftGeometryPoint(s, d, overheadWireGeometry.getShape().front(), overheadWireGeometry.getShapeRotations().front(), overheadWireColorTop, true);
            } else if (segment->isLastSegment()) {
                drawRightGeometryPoint(s, d, overheadWireGeometry.getShape().back(), overheadWireGeometry.getShapeRotations().back(), overheadWireColorTop, true);
            }
            // Pop layer matrix
            GLHelper::popMatrix();
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? overheadWireGeometry.getShape() : segment->getLane()->getLaneShape();
        // calculate contour and draw dotted geometry
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, shape, getType(), overheadWireWidth, 1, true, true, 0, segment, segment->getLane()->getParentEdge());
    }
}


void
GNEOverheadWire::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // calculate overheadWire width
    const double overheadWireWidth = s.addSize.getExaggeration(s, segment->getPreviousLane());
    // check if overhead wire can be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() && segment->getPreviousLane() && segment->getNextLane()) {
        // obtain color
        const RGBColor overheadWireColorTop = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorTop;
        const RGBColor overheadWireColorBot = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.overheadWireColorBot;
        // declare geometry
        GUIGeometry overheadWireGeometry({segment->getPreviousLane()->getLaneShape().back(), segment->getNextLane()->getLaneShape().front()});
        // get detail level
        const auto d = s.getDetailLevel(1);
        // check if exist connection
        if (segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane())) {
            overheadWireGeometry = segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane());
        }
        // get both geometries
        auto overheadWireGeometryTop = overheadWireGeometry;
        auto overheadWireGeometryBot = overheadWireGeometry;
        // move to sides
        overheadWireGeometryTop.moveGeometryToSide(overheadWireWidth * 0.5);
        overheadWireGeometryBot.moveGeometryToSide(overheadWireWidth * -0.5);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // Add a draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            glTranslated(0, 0, getType() + offsetFront);
            // Set top color
            GLHelper::setColor(overheadWireColorTop);
            // draw top geometry
            GUIGeometry::drawGeometry(d, overheadWireGeometryTop, 0.2);
            // Set bot color
            GLHelper::setColor(overheadWireColorBot);
            // draw bot geometry
            GUIGeometry::drawGeometry(d, overheadWireGeometryBot, 0.2);
            // Pop last matrix
            GLHelper::popMatrix();
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // draw contours
        if (segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane())) {
            // get shape
            const auto& shape = segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane()).getShape();
            // calculate contour and draw dotted geometry
            myAdditionalContour.calculateContourExtrudedShape(s, d, this, shape, getType(), overheadWireWidth, 1, true, true, 0, segment, segment->getJunction());
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
            return toString(myStartPosOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPosPosOverLane);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return toString(myForbiddenInnerLanes);
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNEOverheadWire::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_STARTPOS:
            if (myStartPosOverLane < 0) {
                return 0;
            } else if (myStartPosOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
                return getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
            } else {
                return myStartPosOverLane;
            }
        case SUMO_ATTR_ENDPOS:
            if (myEndPosPosOverLane < 0) {
                return 0;
            } else if (myEndPosPosOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
                return getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
            } else {
                return myEndPosPosOverLane;
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEOverheadWire::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_SUBSTATIONID:
            if (value.empty()) {
                return false;
            } else {
                return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TRACTION_SUBSTATION, value, false) != nullptr);
            }
        case SUMO_ATTR_STARTPOS:
            if (value.empty() || (value == LANE_START)) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_ENDPOS:
            if (value.empty() || (value == LANE_END)) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return true;
        default:
            return isCommonValid(key, value);
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
            setAdditionalID(value);
            break;
        case SUMO_ATTR_SUBSTATIONID:
            replaceAdditionalParent(SUMO_TAG_TRACTION_SUBSTATION, value, 0);
            break;
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_STARTPOS:
            if (value.empty() || (value == LANE_START)) {
                myStartPosOverLane = INVALID_DOUBLE;
            } else {
                myStartPosOverLane = parse<double>(value);
            }
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ENDPOS:
            if (value.empty() || (value == LANE_END)) {
                myEndPosPosOverLane = INVALID_DOUBLE;
            } else {
                myEndPosPosOverLane = parse<double>(value);
            }
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
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}

/****************************************************************************/
