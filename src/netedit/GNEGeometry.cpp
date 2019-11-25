/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEGeometry.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
/// @version $Id$
///
// File for geometry classes and functions
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNEAdditional.h>
#include <utils/common/StringTokenizer.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>

#include "GNEGeometry.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGeometry::Geometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::Geometry::Geometry() :
    myPosition(Position::INVALID),
    myRotation(0) {
}


void 
GNEGeometry::Geometry::updateGeometryShape(const PositionVector &shape, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // set new shape
    myShape = shape;
    // check that split positions are correct
    if ((startPos <= 0) || (startPos >= shape.length())) {
        startPos = -1;
    }
    if ((endPos <= 0) || (endPos >= shape.length())) {
        endPos = -1;
    }
    // split shape depending of startPos and endPos
    if ((startPos != -1) && (endPos != -1)) {
        // split lane
        myShape = myShape.getSubpart(startPos, endPos);
    } else if (startPos != -1) {
        // split lane
        myShape = myShape.splitAt(startPos).second;
    } else if (endPos != -1) {
        // split lane
        myShape = myShape.splitAt(endPos).first;
    }
    // check if we have to add an extra first position
    if (extraFirstPosition != Position::INVALID) {
        myShape.push_front(extraFirstPosition);
    }
    // check if we have to add an extra last position
    if (extraLastPosition != Position::INVALID) {
        myShape.push_back(extraLastPosition);
    }
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
}


void 
GNEGeometry::Geometry::updateGeometryPosition(const GNELane *lane, const double posOverLane) {
    // get lane length
    const double laneLength = lane->getLaneShape().length();
    // calculate position and rotation
    if (posOverLane < 0) {
        myPosition = lane->getLaneShape().positionAtOffset(0);
        myRotation = (lane->getLaneShape().rotationDegreeAtOffset(0) * -1);
    } else if (posOverLane > laneLength) {
        myPosition = lane->getLaneShape().positionAtOffset(laneLength);
        myRotation = (lane->getLaneShape().rotationDegreeAtOffset(laneLength) * -1);
    } else {
        myPosition = lane->getLaneShape().positionAtOffset(posOverLane);
        myRotation = (lane->getLaneShape().rotationDegreeAtOffset(posOverLane) * -1);
    }
}


void 
GNEGeometry::Geometry::updateGeometry(const GNEAdditional *additional) {
    // copy geometry of additional
    myShape = additional->getAdditionalGeometry().getShape();
    myShapeLengths = additional->getAdditionalGeometry().getShapeLengths();
    myShapeRotations = additional->getAdditionalGeometry().getShapeRotations();
}


const Position&
GNEGeometry::Geometry::getPosition() const {
    if (myPosition == Position::INVALID) {
        throw ProcessError("invalid single position");
    } else {
        return myPosition;
    }
}


double
GNEGeometry::Geometry::getRotation() const {
    if (myPosition == Position::INVALID) {
        throw ProcessError("invalid single rotation");
    } else {
        return myRotation;
    }
}


const PositionVector&
GNEGeometry::Geometry::getShape() const{
    return myShape;
}


const std::vector<double>&
GNEGeometry::Geometry::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<double>&
GNEGeometry::Geometry::getShapeLengths() const {
    return myShapeLengths;
}


void
GNEGeometry::Geometry::calculateShapeRotationsAndLengths() {
    // clear rotations and lengths
    myShapeRotations.clear();
    myShapeLengths.clear();
    // Get number of parts of the shape
    int numberOfSegments = (int)myShape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        myShapeRotations.reserve(numberOfSegments);
        myShapeLengths.reserve(numberOfSegments);
        // Calculate lengths and rotations for every shape
        for (int i = 0; i < numberOfSegments; i++) {
            myShapeRotations.push_back(calculateRotation(myShape[i], myShape[i + 1]));
            myShapeLengths.push_back(calculateLength(myShape[i], myShape[i + 1]));
        }
    }
}

// ---------------------------------------------------------------------------
// GNEGeometry::SegmentGeometry::Segment - methods
// ---------------------------------------------------------------------------

GNEGeometry::SegmentGeometry::Segment::Segment(const GNEAttributeCarrier* _AC, const GNELane* _lane, const bool _valid) :
    AC(_AC),
    edge(&_lane->getParentEdge()),
    lane(_lane),
    junction(nullptr),
    valid(_valid),
    myUseLaneShape(true),
    myUseLane2LaneShape(false) {
}


GNEGeometry::SegmentGeometry::Segment::Segment(const GNEAttributeCarrier* _AC, const GNELane* _lane,
        const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths, const bool _valid) :
    AC(_AC),
    edge(&_lane->getParentEdge()),
    lane(_lane),
    junction(nullptr),
    valid(_valid),
    myUseLaneShape(false),
    myUseLane2LaneShape(false),
    mySegmentShape(shape),
    mySegmentRotations(shapeRotations),
    mySegmentLengths(shapeLengths) {
}


GNEGeometry::SegmentGeometry::Segment::Segment(const GNEAttributeCarrier* _AC, const GNELane* currentLane, const GNELane* nextLane, const bool _valid) :
    AC(_AC),
    edge(nullptr),
    lane(nextLane),
    junction(currentLane->getParentEdge().getGNEJunctionDestiny()),
    valid(_valid),
    myUseLaneShape(false),
    myUseLane2LaneShape(true),
    mySegmentShape(currentLane->getLane2laneConnections().connectionsMap.at(nextLane).getShape()),
    mySegmentRotations(currentLane->getLane2laneConnections().connectionsMap.at(nextLane).getShapeRotations()),
    mySegmentLengths(currentLane->getLane2laneConnections().connectionsMap.at(nextLane).getShapeLengths()) {
}


void 
GNEGeometry::SegmentGeometry::Segment::update(const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths) {
    mySegmentShape = shape;
    mySegmentRotations = shapeRotations;
    mySegmentLengths = shapeLengths;
}


GNEGeometry::SegmentGeometry::SegmentToUpdate::SegmentToUpdate(const int _index, const GNELane* _lane, const GNELane* _nextLane) :
    index(_index),
    lane(_lane),
    nextLane(_nextLane) {
}


const PositionVector&
GNEGeometry::SegmentGeometry::Segment::getShape() const {
    if (myUseLaneShape) {
        return lane->getLaneShape();
    } else {
        return mySegmentShape;
    }
}


const std::vector<double>&
GNEGeometry::SegmentGeometry::Segment::getShapeRotations() const {
    if (myUseLaneShape) {
        return lane->getShapeRotations();
    } else {
        return mySegmentRotations;
    }
}
            

const std::vector<double>&
GNEGeometry::SegmentGeometry::Segment::getShapeLengths() const {
    if (myUseLaneShape) {
        return lane->getShapeLengths();
    } else {
        return mySegmentLengths;
    }
}

// ---------------------------------------------------------------------------
// GNEGeometry::Geometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::SegmentGeometry::SegmentGeometry() {}


void 
GNEGeometry::SegmentGeometry::insertLaneSegment(const GNEAttributeCarrier* AC, const GNELane* lane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(AC, lane, valid));
}


void 
GNEGeometry::SegmentGeometry::insertCustomSegment(const GNEAttributeCarrier* AC, const GNELane* lane,
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(AC, lane, laneShape, laneShapeRotations, laneShapeLengths, valid));
}


void
GNEGeometry::SegmentGeometry::insertLane2LaneSegment(const GNEAttributeCarrier* AC, const GNELane* currentLane, const GNELane* nextLane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(AC, currentLane, nextLane, valid));
}


void 
GNEGeometry::SegmentGeometry::updateCustomSegment(const int segmentIndex, const PositionVector& newLaneShape, const std::vector<double> &newLaneShapeRotations, const std::vector<double> &newLaneShapeLengths) {
    myShapeSegments.at(segmentIndex).update(newLaneShape, newLaneShapeRotations, newLaneShapeLengths);
}


void 
GNEGeometry::SegmentGeometry::updateLane2LaneSegment(const int segmentIndex, const GNELane* lane, const GNELane* nextLane) {
    myShapeSegments.at(segmentIndex+1).update(
        lane->getLane2laneConnections().connectionsMap.at(nextLane).getShape(), 
        lane->getLane2laneConnections().connectionsMap.at(nextLane).getShapeRotations(), 
        lane->getLane2laneConnections().connectionsMap.at(nextLane).getShapeLengths());
}


void
GNEGeometry::SegmentGeometry::clearSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


const Position &
GNEGeometry::SegmentGeometry::getFirstPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().getShape().size() > 0)) {
        return myShapeSegments.front().getShape().front();
    } else {
        return Position::INVALID;
    }
}

const Position &
GNEGeometry::SegmentGeometry::getLastPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.back().getShape().size() > 0)) {
        return myShapeSegments.back().getShape().back();
    } else {
        return Position::INVALID;
    }
}


double 
GNEGeometry::SegmentGeometry::getFirstRotation() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().getShapeRotations().size() > 0)) {
        return myShapeSegments.front().getShapeRotations().front();
    } else {
        return 0;
    }
}


Boundary 
GNEGeometry::SegmentGeometry::getBoxBoundary() const {
    Boundary b;
    for (const auto &i : myShapeSegments) {
        b.add(i.getShape().getBoxBoundary());
    }
    return b;
}


std::vector<GNEGeometry::SegmentGeometry::Segment>::const_iterator
GNEGeometry::SegmentGeometry::begin() const {
    return myShapeSegments.cbegin();
}


std::vector<GNEGeometry::SegmentGeometry::Segment>::const_iterator
GNEGeometry::SegmentGeometry::end() const {
    return myShapeSegments.cend();
}


const GNEGeometry::SegmentGeometry::Segment&
GNEGeometry::SegmentGeometry::front() const {
    return myShapeSegments.front();
}


const GNEGeometry::SegmentGeometry::Segment&
GNEGeometry::SegmentGeometry::back() const {
    return myShapeSegments.back();
}


int 
GNEGeometry::SegmentGeometry::size() const {
    return (int)myShapeSegments.size();
}

// ---------------------------------------------------------------------------
// GNEGeometry::Lane2laneConnection - methods
// ---------------------------------------------------------------------------

GNEGeometry::Lane2laneConnection::Lane2laneConnection(const GNELane* originLane) :
    myOriginLane(originLane) {
}


void 
GNEGeometry::Lane2laneConnection::updateLane2laneConnection() {
    // clear connectionsMap
    connectionsMap.clear();
    // iterate over outgoingEdge's lanes
    for (const auto &outgoingEdge : myOriginLane->getParentEdge().getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
        for (const auto &outgoingLane : outgoingEdge->getLanes()) {
            // get NBEdges from and to
            const NBEdge* NBEdgeFrom = myOriginLane->getParentEdge().getNBEdge();
            const NBEdge* NBEdgeTo = outgoingLane->getParentEdge().getNBEdge();
            if (NBEdgeFrom->getToNode()->getShape().area() > 4) {
                // Calculate smooth shape
                connectionsMap[outgoingLane].updateGeometryShape(NBEdgeFrom->getToNode()->computeSmoothShape(
                    NBEdgeFrom->getLaneShape(myOriginLane->getIndex()),
                    NBEdgeTo->getLaneShape(outgoingLane->getIndex()),
                    5, NBEdgeFrom->getTurnDestination() == NBEdgeTo,
                    (double) 5. * (double) NBEdgeFrom->getNumLanes(),
                    (double) 5. * (double) NBEdgeTo->getNumLanes()));
            } else {
                // create a shape using shape extremes
                connectionsMap[outgoingLane].updateGeometryShape({NBEdgeFrom->getLaneShape(myOriginLane->getIndex()).back(), NBEdgeTo->getLaneShape(outgoingLane->getIndex()).front()});
            }
            
        }
    }
}

// ---------------------------------------------------------------------------
// GNEGeometry - methods
// ---------------------------------------------------------------------------

double 
GNEGeometry::calculateRotation(const Position& first, const Position& second) {
    // return rotation (angle) of the vector constructed by points first and second
    return ((double)atan2((second.x() - first.x()), (first.y() - second.y())) * (double) 180.0 / (double)M_PI);
}


double 
GNEGeometry::calculateLength(const Position& first, const Position& second) {
    // return 2D distance between two points
    return first.distanceTo2D(second);
}


void 
GNEGeometry::adjustStartPosGeometricPath(double &startPos, const GNELane* startLane, double &endPos, const GNELane* endLane) {
    // adjust both, if start and end lane are the same
    if (startLane && endLane && (startLane == endLane) && (startPos != -1) && (endPos != -1)) {
        if (startPos >= endPos) {
            endPos = (startPos + POSITION_EPS);
        }
    }
    // adjust startPos
    if ((startPos != -1) && startLane) {
        if (startPos <= POSITION_EPS) {
            startPos = -1;
        } 
        if (startPos >= (startLane->getLaneShape().length() - POSITION_EPS)) {
            startPos = -1;
        }
    }
    // adjust endPos
    if ((endPos != -1) && endLane) {
        if (endPos <= POSITION_EPS) {
            endPos = -1;
        }
        if (endPos >= (endLane->getLaneShape().length() - POSITION_EPS)) {
            endPos = -1;
        }
    }
}


void 
GNEGeometry::calculateEdgeGeometricPath(const GNEAttributeCarrier* AC, GNEGeometry::SegmentGeometry &segmentGeometry, 
    const std::vector<GNEEdge*> &edges, const SUMOVehicleClass vClass, GNELane *fromLane, GNELane *toLane, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // clear geometry
    segmentGeometry.clearSegmentGeometry();
    // first check that there is edge parents
    if (edges.size() > 0) {
        // calculate depending if both from and to edges are the same
        if (fromLane == toLane) {
            // if obtained lane is null, then force to use first lane
            if (fromLane == nullptr) {
                calculateLaneGeometricPath(AC, segmentGeometry, {edges.front()->getLanes().front()}, startPos, endPos, extraFirstPosition, extraLastPosition);
            } else {
                calculateLaneGeometricPath(AC, segmentGeometry, {fromLane}, startPos, endPos, extraFirstPosition, extraLastPosition);
            }
        } else {
            // declare vector of lanes
            std::vector<GNELane*> lanes;
            // reserve space
            lanes.reserve(edges.size());
            // obtain lanes by VClass
            for (auto edgeParent = edges.begin(); edgeParent != edges.end(); edgeParent++) {
                GNELane* allowedLane = nullptr;
                if (edgeParent == edges.begin()) {
                    allowedLane = fromLane;
                } else if (edgeParent == (edges.end() - 1)) {
                    allowedLane = toLane;
                } else if (AC->getTagProperty().isRide()) {
                    // obtain first disallowed lane (special case for rides)
                    allowedLane = (*edgeParent)->getLaneByDisallowedVClass(vClass);
                } else {
                    // obtain first allowed lane
                    allowedLane = (*edgeParent)->getLaneByAllowedVClass(vClass);
                }
                // if there isn't allowed lane, then use first lane
                if (allowedLane == nullptr) {
                    allowedLane = (*edgeParent)->getLanes().front();
                }
                // add it to lanes
                lanes.push_back(allowedLane);
            }
            // calculate geometric path
            calculateLaneGeometricPath(AC, segmentGeometry, lanes, startPos, endPos, extraFirstPosition, extraLastPosition);
        }
    }
}


void 
GNEGeometry::calculateLaneGeometricPath(const GNEAttributeCarrier* AC, GNEGeometry::SegmentGeometry &segmentGeometry, const std::vector<GNELane*> &lanes, 
    double startPos, double endPos, const Position &extraFirstPosition, const Position &extraLastPosition) {
    // clear geometry
    segmentGeometry.clearSegmentGeometry();
    // first check that there is edge parents
    if (lanes.size() > 0) {
        // calculate depending if both from and to edges are the same
        if (lanes.size() == 1) {
            // filter start and end pos
            adjustStartPosGeometricPath(startPos, lanes.front(), endPos, lanes.front());
            // check if we have to define a new custom Segment, or we can use the commonLane shape
            if ((startPos != -1) || (endPos != -1) || (extraFirstPosition != Position::INVALID) || (extraLastPosition != Position::INVALID)) {
                // declare a lane to be trimmed
                Geometry trimmedLane;
                // update geometry
                trimmedLane.updateGeometryShape(lanes.front()->getLaneShape(), startPos, endPos, extraFirstPosition, extraLastPosition);
                // add sublane geometry
                segmentGeometry.insertCustomSegment(AC, lanes.front(),
                    trimmedLane.getShape(), 
                    trimmedLane.getShapeRotations(), 
                    trimmedLane.getShapeLengths(), true);
            } else {
                // add entire lane geometry geometry
                segmentGeometry.insertLaneSegment(AC, lanes.front(), true);
            }
        } else {
            // iterate over lanes
            for (int i = 0; i < (int)lanes.size(); i++) {
                // get lane (only for code readability)
                const GNELane* lane = lanes.at(i);
                // first check that lane shape isn't empty
                if (lane->getLaneShape().size() > 0) {
                    // check if first or last lane must be splitted
                    if ((lanes.at(i) == lanes.front()) && (startPos != -1)) {
                        // filter start position
                        adjustStartPosGeometricPath(startPos, lanes.at(i), endPos, nullptr);
                        // declare a lane to be trimmed
                        Geometry frontTrimmedLane;
                        // update geometry
                        frontTrimmedLane.updateGeometryShape(lanes.at(i)->getLaneShape(), startPos, -1, extraFirstPosition, Position::INVALID);
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(AC, lane,
                            frontTrimmedLane.getShape(), 
                            frontTrimmedLane.getShapeRotations(), 
                            frontTrimmedLane.getShapeLengths(), true);
                    } else if ((lane == lanes.back()) && (endPos != -1)) {
                        // filter end position
                        adjustStartPosGeometricPath(startPos, nullptr, endPos, lane);
                        // declare a lane to be trimmed
                        Geometry backTrimmedLane;
                        // update geometry
                        backTrimmedLane.updateGeometryShape(lanes.at(i)->getLaneShape(), -1, endPos, Position::INVALID, extraLastPosition);
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(AC, lane,
                            backTrimmedLane.getShape(), 
                            backTrimmedLane.getShapeRotations(), 
                            backTrimmedLane.getShapeLengths(), true);
                    } else {
                        // add entire lane geometry
                        segmentGeometry.insertLaneSegment(AC, lanes.at(i), true);
                    }
                }
                // now continue with connection
                if ((i+1) < (int)lanes.size()) {
                    // obtain next lane
                    const GNELane* nextLane = lanes.at(i+1);
                    // check that next lane exist
                    if (lane->getLane2laneConnections().connectionsMap.count(nextLane) > 0) {
                        // add lane2laneConnection segment geometry
                        segmentGeometry.insertLane2LaneSegment(AC, lane, nextLane, true);
                    }
                }
            }
        }
    }
}


void 
GNEGeometry::updateGeometricPath(GNEGeometry::SegmentGeometry &segmentGeometry, const GNEEdge* edge, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // calculate depending if both from and to edges are the same
    if ((segmentGeometry.size() == 1) && (segmentGeometry.front().edge == edge)) {
        // filter start and end pos
        adjustStartPosGeometricPath(startPos, segmentGeometry.front().lane, endPos, segmentGeometry.front().lane);
        // check if we have to define a new custom Segment, or we can use the commonLane shape
        if ((startPos != -1) || (endPos != -1) || (extraFirstPosition != Position::INVALID) || (extraLastPosition != Position::INVALID)) {
            // declare a lane to be trimmed
            Geometry trimmedLane;
            // update geometry
            trimmedLane.updateGeometryShape(segmentGeometry.front().lane->getLaneShape(), startPos, endPos, extraFirstPosition, extraLastPosition);
            // add sublane geometry
            segmentGeometry.updateCustomSegment(0,
                trimmedLane.getShape(), 
                trimmedLane.getShapeRotations(), 
                trimmedLane.getShapeLengths());
        }
    } else {
        // declare a vector to save segments to update
        std::vector<GNEGeometry::SegmentGeometry::SegmentToUpdate> segmentsToUpdate;
        // iterate over all segments
        for (auto segment = segmentGeometry.begin(); segment != segmentGeometry.end(); segment++) {
            if (segment->edge == edge) {
                // obtain segment index
                const int index = (int)(segment - segmentGeometry.begin());
                // add SegmentToUpdate in vector
                segmentsToUpdate.push_back(GNEGeometry::SegmentGeometry::SegmentToUpdate(index, segment->lane, nullptr));
                // check if we have to add the next segment (it correspond to a lane2lane
                if (((segment+1) != segmentGeometry.end()) && (segment+1)->junction) {
                    segmentsToUpdate.push_back(GNEGeometry::SegmentGeometry::SegmentToUpdate(index, segment->lane, (segment+1)->lane));
                }
            }
        }
        // iterate over segments to update
        for (const auto &segmentToUpdate : segmentsToUpdate) {
            // first check that lane shape isn't empty
            if (segmentToUpdate.lane->getLaneShape().size() > 0) {
                // check if first or last lane must be splitted
                if ((segmentToUpdate.index == 0) && (startPos != -1)) {
                    // filter start position
                    adjustStartPosGeometricPath(startPos, segmentToUpdate.lane, endPos, nullptr);
                    // declare a lane to be trimmed
                    Geometry frontTrimmedLane;
                    // update geometry
                    frontTrimmedLane.updateGeometryShape(segmentToUpdate.lane->getLaneShape(), startPos, -1, extraFirstPosition, Position::INVALID);
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.index,
                        frontTrimmedLane.getShape(), 
                        frontTrimmedLane.getShapeRotations(), 
                        frontTrimmedLane.getShapeLengths());
                } else if ((segmentToUpdate.index == (segmentGeometry.size() -1)) && (endPos != -1)) {
                    // filter end position
                    adjustStartPosGeometricPath(startPos, nullptr, endPos, segmentToUpdate.lane);
                    // declare a lane to be trimmed
                    Geometry backTrimmedLane;
                    // update geometry
                    backTrimmedLane.updateGeometryShape(segmentToUpdate.lane->getLaneShape(), -1, endPos, Position::INVALID, extraLastPosition);
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.index,
                        backTrimmedLane.getShape(), 
                        backTrimmedLane.getShapeRotations(), 
                        backTrimmedLane.getShapeLengths());
                }
            }
            // check that next lane exist
            if (segmentToUpdate.lane->getLane2laneConnections().connectionsMap.count(segmentToUpdate.nextLane) > 0) {
                // update lane2laneConnection shape
                segmentGeometry.updateLane2LaneSegment(segmentToUpdate.index, segmentToUpdate.lane, segmentToUpdate.nextLane); 
            }
        }
    }
}


void 
GNEGeometry::drawGeometry(const GNEViewNet *viewNet, const Geometry& geometry, const double width) {
    // first check if we're in draw for selecting cliking mode
    if (viewNet->getVisualisationSettings()->drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = geometry.getShape().positionAtOffset2D(geometry.getShape().nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width*width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings()->getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else {
        GLHelper::drawBoxLines(geometry.getShape(), geometry.getShapeRotations(), geometry.getShapeLengths(), width);
    }
}


void 
GNEGeometry::drawLaneGeometry(const GNEViewNet *viewNet, const PositionVector& shape, const std::vector<double>& rotations, 
    const std::vector<double>& lengths, const std::vector<RGBColor>& colors, double width) {
    // first check if we're in draw for selecting cliking mode
    if (viewNet->getVisualisationSettings()->drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width*width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings()->getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else if (colors.size() > 0) {
        // draw box lines with own colors
        GLHelper::drawBoxLines(shape, rotations, lengths, colors, width);
    } else {
        // draw box lines with current color
        GLHelper::drawBoxLines(shape, rotations, lengths, width);
    }
}


void
GNEGeometry::drawSegmentGeometry(const GNEViewNet *viewNet, const SegmentGeometry::Segment& segment, const double width) {
    // first check if we're in draw for selecting mode
    if (viewNet->getVisualisationSettings()->drawForRectangleSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = segment.getShape().positionAtOffset2D(segment.getShape().nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width*width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings()->getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else {
        // draw a boxline as usual
        GLHelper::drawBoxLines(segment.getShape(), segment.getShapeRotations(), segment.getShapeLengths(), width);
    }
}

/****************************************************************************/
