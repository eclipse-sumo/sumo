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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementLaneDouble.h>
#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEOverheadWire.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEOverheadWire::GNEOverheadWire(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_OVERHEAD_WIRE_SECTION, ""),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_STARTPOS, myStartPosOverLane,
                            SUMO_ATTR_ENDPOS, myEndPosPosOverLane, myFriendlyPosition)) {
}


GNEOverheadWire::GNEOverheadWire(const std::string& id, GNENet* net, const std::string& filename, std::vector<GNELane*> lanes, GNEAdditional* substation,
                                 const double startPos, const double endPos, const bool friendlyPos, const std::vector<std::string>& forbiddenInnerLanes,
                                 const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_OVERHEAD_WIRE_SECTION, ""),
    Parameterised(parameters),
    myStartPosOverLane(startPos),
    myEndPosPosOverLane(endPos),
    myFriendlyPosition(friendlyPos),
    myForbiddenInnerLanes(forbiddenInnerLanes),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_STARTPOS, myStartPosOverLane,
                            SUMO_ATTR_ENDPOS, myEndPosPosOverLane, myFriendlyPosition)) {
    // set parents
    setParents<GNELane*>(lanes);
    setParent<GNEAdditional*>(substation);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEOverheadWire::~GNEOverheadWire() {
    delete myMoveElementLaneDouble;
}


GNEMoveElement*
GNEOverheadWire::getMoveElement() const {
    return myMoveElementLaneDouble;
}


Parameterised*
GNEOverheadWire::getParameters() {
    return this;
}


const Parameterised*
GNEOverheadWire::getParameters() const {
    return this;
}


void
GNEOverheadWire::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_OVERHEAD_WIRE_SECTION);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move atributes
    myMoveElementLaneDouble->writeMoveAttributes(device, false);
    // write specific attributes
    device.writeAttr(SUMO_ATTR_SUBSTATIONID, getParentAdditionals().front()->getID());
    if (!myForbiddenInnerLanes.empty()) {
        device.writeAttr(SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN, myForbiddenInnerLanes);
    }
    // write parameters
    writeParams(device);
    device.closeTag();
}


bool
GNEOverheadWire::isAdditionalValid() const {
    // only movement problems
    return myMoveElementLaneDouble->isMoveElementValid();
}


std::string
GNEOverheadWire::getAdditionalProblem() const {
    // only movement problems
    return myMoveElementLaneDouble->getMovingProblem();
}


void
GNEOverheadWire::fixAdditionalProblem() {
    // only movement problems
    return myMoveElementLaneDouble->fixMovingProblem();
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
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return toString(myForbiddenInnerLanes);
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return myMoveElementLaneDouble->getMovingAttribute(key);
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
            return myMoveElementLaneDouble->getMovingAttributeDouble(key);
    }
}


Position
GNEOverheadWire::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementLaneDouble->getMovingAttributePosition(key);
}


PositionVector
GNEOverheadWire::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEOverheadWire::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SUBSTATIONID:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementLaneDouble->setMovingAttribute(key, value, undoList);
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
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            return true;
        default:
            return myMoveElementLaneDouble->isMovingAttributeValid(key, value);
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
        case SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN:
            myForbiddenInnerLanes = parse<std::vector<std::string> >(value);
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            myMoveElementLaneDouble->setMovingAttribute(key, value);
            break;
    }
    // update geometry (except for template)
    if (getParentLanes().size() > 0) {
        updateGeometry();
    }
}

/****************************************************************************/
