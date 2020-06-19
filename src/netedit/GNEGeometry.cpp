/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEGeometry.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2019
///
// File for geometry classes and functions
/****************************************************************************/
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEGeometry.h"
#include "GNEViewNet.h"

#define CIRCLE_RESOLUTION (double)10 // inverse in degrees

// ===========================================================================
// static member definitions
// ===========================================================================
PositionVector GNEGeometry::myCircleCoords;

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGeometry::ExtremeGeometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::ExtremeGeometry::ExtremeGeometry() :
    laneStartPosition(-1),
    laneEndPosition(-1),
    viewStartPos(Position::INVALID),
    viewEndPos(Position::INVALID) {
}

// ---------------------------------------------------------------------------
// GNEGeometry::Geometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::Geometry::Geometry() :
    myPosition(Position::INVALID),
    myRotation(0),
    myLane(nullptr),
    myAdditional(nullptr) {
}


GNEGeometry::Geometry::Geometry(const PositionVector& shape, const std::vector<double>& shapeRotations, const std::vector<double>& shapeLengths) :
    myPosition(Position::INVALID),
    myRotation(0),
    myShape(shape),
    myShapeRotations(shapeRotations),
    myShapeLengths(shapeLengths),
    myLane(nullptr),
    myAdditional(nullptr) {
}


void
GNEGeometry::Geometry::updateGeometry(const PositionVector& shape, double startPos, double endPos,
                                      const Position& extraFirstPosition, const Position& extraLastPosition) {
    // first clear geometry
    clearGeometry();
    // set new shape
    myShape = shape;
    // check if we have to split the lane
    if ((startPos != -1) || (endPos != -1)) {
        // check if both start and end position must be swapped
        if ((startPos != -1) && (endPos != -1) && (endPos < startPos)) {
            std::swap(startPos, endPos);
        }
        // check that split positions are correct
        if (startPos <= POSITION_EPS) {
            if (endPos == -1) {
                // leave shape unmodified
            } else if (endPos <= POSITION_EPS) {
                // use only first shape position
                myShape = PositionVector({myShape.front()});
            } else if (endPos < (shape.length() - POSITION_EPS)) {
                // split shape using end position and use left part
                myShape = myShape.splitAt(endPos).first;
            }
        } else if (startPos >= (shape.length() - POSITION_EPS)) {
            // use only last position
            myShape = PositionVector({myShape.back()});
        } else if (endPos == -1) {
            // split shape using start position and use the right part
            myShape = myShape.splitAt(startPos).second;
        } else if (endPos <= POSITION_EPS) {
            // use only first shape position
            myShape = PositionVector({myShape.front()});
        } else if (endPos >= (shape.length() - POSITION_EPS)) {
            // split shape using start position and use the right part
            myShape = myShape.splitAt(startPos).second;
        } else {
            // split shape using start and end position
            myShape = myShape.getSubpart(startPos, endPos);
        }
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
GNEGeometry::Geometry::updateGeometry(const Position& position, const double rotation) {
    // first clear geometry
    clearGeometry();
    // set position and rotation
    myPosition = position;
    myRotation = rotation;
}


void
GNEGeometry::Geometry::updateGeometry(const GNELane* lane, const double posOverLane) {
    // first clear geometry
    clearGeometry();
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
GNEGeometry::Geometry::updateGeometry(const GNELane* lane) {
    // first clear geometry
    clearGeometry();
    // set lane
    myLane = lane;
}


void
GNEGeometry::Geometry::updateGeometry(const GNEAdditional* additional) {
    // first clear geometry
    clearGeometry();
    // set additional
    myAdditional = additional;
}


void
GNEGeometry::Geometry::updateGeometry(const Geometry &geometry) {
    // first clear geometry
    clearGeometry();
    // set geometry
    myShape = geometry.getShape();
    myShapeRotations = geometry.getShapeRotations();
    myShapeLengths = geometry.getShapeLengths();
}


const Position&
GNEGeometry::Geometry::getPosition() const {
    return myPosition;
}


double
GNEGeometry::Geometry::getRotation() const {
    return myRotation;
}


const PositionVector&
GNEGeometry::Geometry::getShape() const {
    if (myLane) {
        return myLane->getLaneShape();
    } else if (myAdditional) {
        return myAdditional->getAdditionalGeometry().getShape();
    } else {
        return myShape;
    }
}


const std::vector<double>&
GNEGeometry::Geometry::getShapeRotations() const {
    if (myLane) {
        return myLane->getShapeRotations();
    } else if (myAdditional) {
        return myAdditional->getAdditionalGeometry().getShapeRotations();
    } else {
        return myShapeRotations;
    }
}


const std::vector<double>&
GNEGeometry::Geometry::getShapeLengths() const {
    if (myLane) {
        return myLane->getShapeLengths();
    } else if (myAdditional) {
        return myAdditional->getAdditionalGeometry().getShapeLengths();
    } else {
        return myShapeLengths;
    }
}


void GNEGeometry::Geometry::clearGeometry() {
    // clear single position
    myPosition.set(0, 0, 0);
    myRotation = 0;
    // clear shapes
    myShape.clear();
    myShapeRotations.clear();
    myShapeLengths.clear();
    // clear pointers
    myLane = nullptr;
    myAdditional = nullptr;
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
// GNEGeometry::DottedGeometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::DottedGeometry::DottedGeometry() :
    myRotation(0),
    myDottedGeometryDeprecated(true) {
}


void
GNEGeometry::DottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, const PositionVector& contourShape) {
    // disable rotation
    myRotation = 0;
    // obtain shape's centroid
    myCentroid = contourShape.getCentroid();
    // set new shape
    myShape = contourShape;
    // subs Centroid (to set myShape in 0,0. It's needed due scaling)
    myShape.sub(myCentroid);
    // set new resampled shape
    myShape = myShape.resample(s.dottedContourSettings.segmentLength);
    // resize shapeColors
    myShapeColors.resize(myShape.size());
    // iterate over shapeColors
    for (int i = 0; i < (int)myShapeColors.size(); i++) {
        // set first or second contour color
        if (i % 2 == 0) {
            myShapeColors.at(i) = s.dottedContourSettings.firstColor;
        } else {
            myShapeColors.at(i) = s.dottedContourSettings.secondColor;
        }
    }
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
    // set geometry updated
    myDottedGeometryDeprecated = false;
}


void
GNEGeometry::DottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, const PositionVector& lineShape, const double width) {
    if (width > 0) {
        // build contour using line shape
        PositionVector contourFront = lineShape;
        PositionVector contourback = contourFront;
        // move both to side
        contourFront.move2side(width);
        contourback.move2side(-width);
        // reverse contourback
        contourback = contourback.reverse();
        // append contourback into contourfront
        for (auto position : contourback) {
            contourFront.push_back(position);
        }
        // close contourFront
        contourFront.closePolygon();
        // updated dotted geometry, but now with a contour shape
        updateDottedGeometry(s, contourFront);
    } else {
        updateDottedGeometry(s, lineShape);
    }
}


void GNEGeometry::DottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, const Position& position, const double rotation, const double width, const double height) {
    // declare rectangle and adjust using width, height
    PositionVector rectangle({
        { width, -height},
        { width,  height},
        {-width,  height},
        {-width, -height}
    });
    // close rectangle
    rectangle.closePolygon();
    // move rectangle to position
    rectangle.add(position);
    // update dotted geometry using rectangle
    updateDottedGeometry(s, rectangle);
    // set rotation
    myRotation = rotation;
}


void
GNEGeometry::DottedGeometry::markDottedGeometryDeprecated() {
    myDottedGeometryDeprecated = true;
}


bool
GNEGeometry::DottedGeometry::isGeometryDeprecated() const {
    return myDottedGeometryDeprecated;
}


const Position&
GNEGeometry::DottedGeometry::getCentroid() const {
    return myCentroid;
}


double
GNEGeometry::DottedGeometry::getRotation() const {
    return myRotation;
}


const PositionVector&
GNEGeometry::DottedGeometry::getShape() const {
    return myShape;
}


const std::vector<double>&
GNEGeometry::DottedGeometry::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<double>&
GNEGeometry::DottedGeometry::getShapeLengths() const {
    return myShapeLengths;
}


const std::vector<RGBColor>&
GNEGeometry::DottedGeometry::getShapeColors() const {
    return myShapeColors;
}


void
GNEGeometry::DottedGeometry::calculateShapeRotationsAndLengths() {
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

GNEGeometry::SegmentGeometry::Segment::Segment(const GNELane* lane, const bool valid) :
    myLane(lane),
    myNextLane(nullptr),
    myValid(valid),
    myUseLaneShape(true) {
}


GNEGeometry::SegmentGeometry::Segment::Segment(const GNELane* lane, const Geometry &geometry, const bool valid) :
    myLane(lane),
    myNextLane(nullptr),
    myValid(valid),
    myUseLaneShape(false),
    mySegmentGeometry(geometry) {
}


GNEGeometry::SegmentGeometry::Segment::Segment(const GNELane* lane, const GNELane* nextLane, const bool valid) :
    myLane(lane),
    myNextLane(nextLane),
    myValid(valid),
    myUseLaneShape(false) {
}


void
GNEGeometry::SegmentGeometry::Segment::update(const GNEGeometry::Geometry &geometry) {
    // disable use lane shape
    myUseLaneShape = false;
    // update geometry
    mySegmentGeometry.updateGeometry(geometry);
}


void
GNEGeometry::SegmentGeometry::Segment::update(const GNELane* lane) {
    // enable use lane shape
    myUseLaneShape = true;
    // update geometry 
    mySegmentGeometry.updateGeometry(lane);
}


const PositionVector&
GNEGeometry::SegmentGeometry::Segment::getShape() const {
    if (myNextLane) {
        return myLane->getLane2laneConnections().getLane2laneGeometry(myNextLane).getShape();
    } else if (myUseLaneShape) {
        return myLane->getLaneShape();
    } else {
        return mySegmentGeometry.getShape();
    }
}


const std::vector<double>&
GNEGeometry::SegmentGeometry::Segment::getShapeRotations() const {
    if (myNextLane) {
        return myLane->getLane2laneConnections().getLane2laneGeometry(myNextLane).getShapeRotations();
    } else if (myUseLaneShape) {
        return myLane->getShapeRotations();
    } else {
        return mySegmentGeometry.getShapeRotations();
    }
}


const std::vector<double>&
GNEGeometry::SegmentGeometry::Segment::getShapeLengths() const {
    if (myNextLane) {
        return myLane->getLane2laneConnections().getLane2laneGeometry(myNextLane).getShapeLengths();
    } else if (myUseLaneShape) {
        return myLane->getShapeLengths();
    } else {
        return mySegmentGeometry.getShapeLengths();
    }
}


const GNELane* 
GNEGeometry::SegmentGeometry::Segment::getLane() const {
    return myLane;
}


const GNEJunction* 
GNEGeometry::SegmentGeometry::Segment::getJunction() const {
    return myLane->getParentEdge()->getSecondParentJunction();
}


bool
GNEGeometry::SegmentGeometry::Segment::getValid() const {
    return myValid;
}

// ---------------------------------------------------------------------------
// GNEGeometry::SegmentGeometry::SegmentToUpdate - methods
// ---------------------------------------------------------------------------

GNEGeometry::SegmentGeometry::SegmentToUpdate::SegmentToUpdate(const int segmentIndex, const GNELane* lane, const GNELane* nextLane) :
    mySegmentIndex(segmentIndex),
    myLane(lane),
    myNextLane(nextLane) {
}


int
GNEGeometry::SegmentGeometry::SegmentToUpdate::getSegmentIndex() const {
    return mySegmentIndex;
}


const GNELane* 
GNEGeometry::SegmentGeometry::SegmentToUpdate::getLane() const {
    return myLane;
}


const GNELane* 
GNEGeometry::SegmentGeometry::SegmentToUpdate::getNextLane() const {
    return myNextLane;
}

// ---------------------------------------------------------------------------
// GNEGeometry::Geometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::SegmentGeometry::SegmentGeometry() {}


void
GNEGeometry::SegmentGeometry::insertLaneSegment(const GNELane* lane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(lane, valid));
}


void
GNEGeometry::SegmentGeometry::insertCustomSegment(const GNELane* lane, const Geometry &geometry, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(lane, geometry, valid));
}


void
GNEGeometry::SegmentGeometry::insertLane2LaneSegment(const GNELane* currentLane, const GNELane* nextLane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(currentLane, nextLane, valid));
}


void
GNEGeometry::SegmentGeometry::updateCustomSegment(const int segmentIndex, const Geometry &geometry) {
    myShapeSegments.at(segmentIndex).update(geometry);
}


void
GNEGeometry::SegmentGeometry::updateLane2LaneSegment(const int segmentIndex, const GNELane* lane, const GNELane* nextLane) {
    myShapeSegments.at(segmentIndex + 1).update(lane->getLane2laneConnections().getLane2laneGeometry(nextLane));
}


void
GNEGeometry::SegmentGeometry::clearSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


const Position&
GNEGeometry::SegmentGeometry::getFirstPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().getShape().size() > 0)) {
        return myShapeSegments.front().getShape().front();
    } else {
        return Position::INVALID;
    }
}

const Position&
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
    for (const auto& i : myShapeSegments) {
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

GNEGeometry::Lane2laneConnection::Lane2laneConnection(const GNELane* fromLane) :
    myFromLane(fromLane) {
}


void
GNEGeometry::Lane2laneConnection::updateLane2laneConnection() {
    // declare numPoints
    const int numPoints = 5;
    const int maximumLanes = 10;
    // clear connectionsMap
    myConnectionsMap.clear();
    // iterate over outgoingEdge's lanes
    for (const auto& outgoingEdge : myFromLane->getParentEdge()->getSecondParentJunction()->getGNEOutgoingEdges()) {
        for (const auto& outgoingLane : outgoingEdge->getLanes()) {
            // get NBEdges from and to
            const NBEdge* NBEdgeFrom = myFromLane->getParentEdge()->getNBEdge();
            const NBEdge* NBEdgeTo = outgoingLane->getParentEdge()->getNBEdge();
            // only create smooth shapes if Edge From has as maximum 10 lanes
            if ((NBEdgeFrom->getNumLanes() <= maximumLanes) && (NBEdgeFrom->getToNode()->getShape().area() > 4)) {
                // Calculate smooth shape
                myConnectionsMap[outgoingLane].updateGeometry(NBEdgeFrom->getToNode()->computeSmoothShape(
                            NBEdgeFrom->getLaneShape(myFromLane->getIndex()),
                            NBEdgeTo->getLaneShape(outgoingLane->getIndex()),
                            numPoints, NBEdgeFrom->getTurnDestination() == NBEdgeTo,
                            (double) numPoints * (double) NBEdgeFrom->getNumLanes(),
                            (double) numPoints * (double) NBEdgeTo->getNumLanes()));
            } else {
                // create a shape using lane shape extremes
                myConnectionsMap[outgoingLane].updateGeometry({
                    myFromLane->getLaneShape().back(),
                    outgoingLane->getLaneShape().front()});
            }
        }
    }
}


bool
GNEGeometry::Lane2laneConnection::exist(const GNELane* toLane) const {
    return (myConnectionsMap.count(toLane) > 0);
}


const GNEGeometry::Geometry&
GNEGeometry::Lane2laneConnection::getLane2laneGeometry(const GNELane* toLane) const {
    return myConnectionsMap.at(toLane);
}


GNEGeometry::Lane2laneConnection::Lane2laneConnection() :
    myFromLane(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEHierarchicalParentElements::ParentConnections - methods
// ---------------------------------------------------------------------------

GNEGeometry::ParentConnections::ParentConnections(GNEHierarchicalParentElements* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEGeometry::ParentConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& edge : myHierarchicalElement->getParentEdges()) {
        for (const auto& lane : edge->getLanes()) {
            std::pair<Position, double> posRot;
            // set position and length depending of shape's lengt
            if (lane->getLaneShape().length() - 6 > 0) {
                posRot.first = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
                posRot.second = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
            } else {
                posRot.first = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
                posRot.second = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
            }
            symbolsPositionAndRotation.push_back(posRot);
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& lane : myHierarchicalElement->getParentLanes()) {
        std::pair<Position, double> posRot;
        // set position and length depending of shape's lengt
        if (lane->getLaneShape().length() - 6 > 0) {
            posRot.first = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
            posRot.second = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
        } else {
            posRot.first = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
            posRot.second = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
        }
        symbolsPositionAndRotation.push_back(posRot);
    }
    // calculate position for every parent additional
    for (const auto& additional : myHierarchicalElement->getParentAdditionals()) {
        // check that position is different of position
        if (additional->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(additional->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            double B = std::abs(additional->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(additional->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > additional->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    posConnection.push_back(Position(additional->getPositionInView().x() + A, additional->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    posConnection.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(additional->getPositionInView().x() - A, additional->getPositionInView().y()));
                }
            }
            posConnection.push_back(myHierarchicalElement->getPositionInView());
            connectionPositions.push_back(posConnection);
        }
    }
    // calculate geometry for connections between parent and parents
    for (const auto& symbol : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(symbol.first.x() - myHierarchicalElement->getPositionInView().x());
        double B = std::abs(symbol.first.y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(symbol.first);
        if (myHierarchicalElement->getPositionInView().x() > symbol.first.x()) {
            if (myHierarchicalElement->getPositionInView().y() > symbol.first.y()) {
                posConnection.push_back(Position(symbol.first.x() + A, symbol.first.y()));
            } else {
                posConnection.push_back(Position(symbol.first.x(), symbol.first.y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > symbol.first.y()) {
                posConnection.push_back(Position(symbol.first.x(), symbol.first.y() + B));
            } else {
                posConnection.push_back(Position(symbol.first.x() - A, symbol.first.y()));
            }
        }
        posConnection.push_back(myHierarchicalElement->getPositionInView());
        connectionPositions.push_back(posConnection);
    }
}


void
GNEGeometry::ParentConnections::draw(const GUIVisualizationSettings& /* s */, const GUIGlObjectType parentType) const {
    // Iterate over myConnectionPositions
    for (const PositionVector& connectionPosition : connectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // translate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
        // Set color of the base
        GLHelper::setColor(GUIVisualizationColorSettings::childConnections);
        // iterate over connections
        for (PositionVector::const_iterator j = connectionPosition.begin(); (j + 1) != connectionPosition.end(); ++j) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
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
GNEGeometry::adjustStartPosGeometricPath(double& startPos, const GNELane* startLane, double& endPos, const GNELane* endLane) {
    // adjust both, if start and end lane are the same
    if (startLane && endLane && (startLane == endLane) && (startPos != -1) && (endPos != -1)) {
        if (startPos >= endPos) {
            endPos = (startPos + POSITION_EPS);
        }
    }
    // adjust startPos
    if ((startPos != -1) && startLane) {
        if (startPos < POSITION_EPS) {
            startPos = POSITION_EPS;
        }
        if (startPos > (startLane->getLaneShape().length() - POSITION_EPS)) {
            startPos = (startLane->getLaneShape().length() - POSITION_EPS);
        }
    }
    // adjust endPos
    if ((endPos != -1) && endLane) {
        if (endPos < POSITION_EPS) {
            endPos = POSITION_EPS;
        }
        if (endPos > (endLane->getLaneShape().length() - POSITION_EPS)) {
            endPos = (endLane->getLaneShape().length() - POSITION_EPS);
        }
    }
}


void
GNEGeometry::calculateLaneGeometricPath(GNEGeometry::SegmentGeometry& segmentGeometry, const std::vector<GNEPathElements::PathElement>& path, GNEGeometry::ExtremeGeometry &extremeGeometry) {
    // clear geometry
    segmentGeometry.clearSegmentGeometry();
    // first check that there is parent edges
    if (path.size() > 0) {
        // calculate depending if both from and to edges are the same
        if (path.size() == 1) {
            // filter start and end pos
            adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, path.front().getLane(), extremeGeometry.laneEndPosition, path.front().getLane());
            // check if we have to define a new custom Segment, or we can use the commonLane shape
            if ((extremeGeometry.laneStartPosition != -1) || (extremeGeometry.laneEndPosition != -1) || (extremeGeometry.viewStartPos != Position::INVALID) || (extremeGeometry.viewEndPos != Position::INVALID)) {
                // declare a lane to be trimmed
                Geometry trimmedLane;
                // update geometry
                trimmedLane.updateGeometry(path.front().getLane()->getLaneShape(), extremeGeometry.laneStartPosition, extremeGeometry.laneEndPosition, extremeGeometry.viewStartPos, extremeGeometry.viewEndPos);
                // add sublane geometry
                segmentGeometry.insertCustomSegment(path.front().getLane(), trimmedLane, true);
            } else {
                // add entire lane geometry geometry
                segmentGeometry.insertLaneSegment(path.front().getLane(), true);
            }
        } else {
            // iterate over path
            for (int i = 0; i < (int)path.size(); i++) {
                // get lane (only for code readability)
                const GNELane* lane = path.at(i).getLane();
                // first check that lane shape isn't empty
                if (lane->getLaneShape().size() > 0) {
                    // check if first or last lane must be splitted
                    if ((path.at(i).getLane() == path.front().getLane()) && (extremeGeometry.laneStartPosition != -1)) {
                        // filter start position
                        adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, path.at(i).getLane(), extremeGeometry.laneEndPosition, nullptr);
                        // declare a lane to be trimmed
                        Geometry frontTrimmedLane;
                        // update geometry
                        frontTrimmedLane.updateGeometry(path.at(i).getLane()->getLaneShape(), extremeGeometry.laneStartPosition, -1, extremeGeometry.viewStartPos, Position::INVALID);
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(lane, frontTrimmedLane, true);
                    } else if ((lane == path.back().getLane()) && (extremeGeometry.laneEndPosition != -1)) {
                        // filter end position
                        adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, nullptr, extremeGeometry.laneEndPosition, lane);
                        // declare a lane to be trimmed
                        Geometry backTrimmedLane;
                        // update geometry
                        backTrimmedLane.updateGeometry(path.at(i).getLane()->getLaneShape(), -1, extremeGeometry.laneEndPosition, Position::INVALID, extremeGeometry.viewEndPos);
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(lane, backTrimmedLane, true);
                    } else {
                        // add entire lane geometry
                        segmentGeometry.insertLaneSegment(path.at(i).getLane(), true);
                    }
                }
                // now continue with connection
                if ((i + 1) < (int)path.size()) {
                    // obtain next lane
                    const GNELane* nextLane = path.at(i + 1).getLane();
                    // check that next lane exist
                    if (lane->getLane2laneConnections().exist(nextLane)) {
                        // add lane2laneConnection segment geometry
                        segmentGeometry.insertLane2LaneSegment(lane, nextLane, true);
                    }
                }
            }
        }
    }
}


void
GNEGeometry::updateGeometricPath(GNEGeometry::SegmentGeometry& segmentGeometry, const GNELane* lane, GNEGeometry::ExtremeGeometry &extremeGeometry) {
    // calculate depending if both from and to edges are the same
    if ((segmentGeometry.size() == 1) && (segmentGeometry.front().getLane() == lane)) {
        // filter start and end pos
        adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, segmentGeometry.front().getLane(), extremeGeometry.laneEndPosition, segmentGeometry.front().getLane());
        // check if we have to define a new custom Segment, or we can use the commonLane shape
        if ((extremeGeometry.laneStartPosition != -1) || (extremeGeometry.laneEndPosition != -1) || (extremeGeometry.viewStartPos != Position::INVALID) || (extremeGeometry.viewEndPos != Position::INVALID)) {
            // declare a lane to be trimmed
            Geometry trimmedLane;
            // update geometry
            trimmedLane.updateGeometry(segmentGeometry.front().getLane()->getLaneShape(), extremeGeometry.laneStartPosition, extremeGeometry.laneEndPosition, extremeGeometry.viewStartPos, extremeGeometry.viewEndPos);
            // add sublane geometry
            segmentGeometry.updateCustomSegment(0, trimmedLane);
        }
    } else {
        // declare a vector to save segments to update
        std::vector<GNEGeometry::SegmentGeometry::SegmentToUpdate> segmentsToUpdate;
        // iterate over all segments
        for (auto segment = segmentGeometry.begin(); segment != segmentGeometry.end(); segment++) {
            if (segment->getLane() == lane) {
                // obtain segment index
                const int index = (int)(segment - segmentGeometry.begin());
                // add SegmentToUpdate in vector
                segmentsToUpdate.push_back(GNEGeometry::SegmentGeometry::SegmentToUpdate(index, segment->getLane(), nullptr));
                // check if we have to add the next segment (it correspond to a lane2lane
                if (((segment + 1) != segmentGeometry.end()) && (segment + 1)->getJunction()) {
                    segmentsToUpdate.push_back(GNEGeometry::SegmentGeometry::SegmentToUpdate(index, segment->getLane(), (segment + 1)->getLane()));
                }
            }
        }
        // iterate over segments to update
        for (const auto& segmentToUpdate : segmentsToUpdate) {
            // first check that lane shape isn't empty
            if (segmentToUpdate.getLane()->getLaneShape().size() > 0) {
                // check if first or last lane must be splitted
                if ((segmentToUpdate.getSegmentIndex() == 0) && (extremeGeometry.laneStartPosition != -1)) {
                    // filter start position
                    adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, segmentToUpdate.getLane(), extremeGeometry.laneEndPosition, nullptr);
                    // declare a lane to be trimmed
                    Geometry frontTrimmedLane;
                    // update geometry
                    frontTrimmedLane.updateGeometry(segmentToUpdate.getLane()->getLaneShape(), extremeGeometry.laneStartPosition, -1, extremeGeometry.viewStartPos, Position::INVALID);
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.getSegmentIndex(), frontTrimmedLane);
                } else if ((segmentToUpdate.getSegmentIndex() == (segmentGeometry.size() - 1)) && (extremeGeometry.laneEndPosition != -1)) {
                    // filter end position
                    adjustStartPosGeometricPath(extremeGeometry.laneStartPosition, nullptr, extremeGeometry.laneEndPosition, segmentToUpdate.getLane());
                    // declare a lane to be trimmed
                    Geometry backTrimmedLane;
                    // update geometry
                    backTrimmedLane.updateGeometry(segmentToUpdate.getLane()->getLaneShape(), -1, extremeGeometry.laneEndPosition, Position::INVALID, extremeGeometry.viewEndPos);
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.getSegmentIndex(), backTrimmedLane);
                }
            }
            // check that next lane exist
            if (segmentToUpdate.getLane()->getLane2laneConnections().exist(segmentToUpdate.getNextLane())) {
                // update lane2laneConnection shape
                segmentGeometry.updateLane2LaneSegment(segmentToUpdate.getSegmentIndex(), segmentToUpdate.getLane(), segmentToUpdate.getNextLane());
            }
        }
    }
}


void
GNEGeometry::drawGeometry(const GNEViewNet* viewNet, const Geometry& geometry, const double width) {
    // first check if we're in draw for selecting cliking mode
    if (viewNet->getVisualisationSettings().drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = geometry.getShape().positionAtOffset2D(geometry.getShape().nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width * width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings().getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else {
        GLHelper::drawBoxLines(geometry.getShape(), geometry.getShapeRotations(), geometry.getShapeLengths(), width);
    }
}


void
GNEGeometry::drawLaneGeometry(const GNEViewNet* viewNet, const PositionVector& shape, const std::vector<double>& rotations,
                              const std::vector<double>& lengths, const std::vector<RGBColor>& colors, double width) {
    // first check if we're in draw for selecting cliking mode
    if (viewNet->getVisualisationSettings().drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width * width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings().getCircleResolution());
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
GNEGeometry::drawSegmentGeometry(const GNEViewNet* viewNet, const SegmentGeometry::Segment& segment, const double width) {
    // first check if we're in draw for selecting mode
    if (viewNet->getVisualisationSettings().drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = segment.getShape().positionAtOffset2D(segment.getShape().nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width * width)) {
            // push matrix
            glPushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings().getCircleResolution());
            // pop draw matrix
            glPopMatrix();
        }
    } else {
        // draw a boxline as usual
        GLHelper::drawBoxLines(segment.getShape(), segment.getShapeRotations(), segment.getShapeLengths(), width);
    }
}


void
GNEGeometry::drawShapeDottedContour(const GUIVisualizationSettings& s, const double typeLayer, const double exaggeration, const DottedGeometry& dottedGeometry) {
    // first check that given shape isn't empty
    if (!s.drawForRectangleSelection && !s.drawForPositionSelection && (dottedGeometry.getShape().size() > 0)) {
        // push matrix
        glPushMatrix();
        // Move to Centroid
        glTranslated(dottedGeometry.getCentroid().x(), dottedGeometry.getCentroid().y(), typeLayer + 2);
        // scale matrix depending of the exaggeration
        if (exaggeration != 1) {
            glScaled(exaggeration, exaggeration, 1);
        }
        // rotate depending of rotation
        if (dottedGeometry.getRotation() != 0) {
            glRotated(dottedGeometry.getRotation(), 0, 0, 1);
        }
        // draw box lines
        GLHelper::drawBoxLines(dottedGeometry.getShape(),
                               dottedGeometry.getShapeRotations(),
                               dottedGeometry.getShapeLengths(),
                               dottedGeometry.getShapeColors(),
                               s.dottedContourSettings.segmentWidth);
        // pop matrix
        glPopMatrix();
    }
}


PositionVector
GNEGeometry::getVertexCircleAroundPosition(const Position& pos, const double width, const int steps) {
    // first check if we have to fill myCircleCoords (only once)
    if (myCircleCoords.size() == 0) {
        for (int i = 0; i <= (int)(360 * CIRCLE_RESOLUTION); ++i) {
            const double x = (double) sin(DEG2RAD(i / CIRCLE_RESOLUTION));
            const double y = (double) cos(DEG2RAD(i / CIRCLE_RESOLUTION));
            myCircleCoords.push_back(Position(x, y));
        }
    }
    PositionVector vertexCircle;
    const double inc = 360 / (double)steps;
    // obtain all vertices
    for (int i = 0; i <= steps; ++i) {
        const Position& vertex = myCircleCoords[GNEGeometry::angleLookup(i * inc)];
        vertexCircle.push_back(Position(vertex.x() * width, vertex.y() * width));
    }
    // move result using position
    vertexCircle.add(pos);
    return vertexCircle;
}


int
GNEGeometry::angleLookup(const double angleDeg) {
    const int numCoords = (int)myCircleCoords.size() - 1;
    int index = ((int)(floor(angleDeg * CIRCLE_RESOLUTION + 0.5))) % numCoords;
    if (index < 0) {
        index += numCoords;
    }
    assert(index >= 0);
    return (int)index;
}


/****************************************************************************/
