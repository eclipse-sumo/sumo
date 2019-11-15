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
#include <utils/common/StringTokenizer.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>

#include "GNEGeometry.h"
#include "GNENet.h"


// ===========================================================================
// static members
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================


// ---------------------------------------------------------------------------
// GNEGeometry::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::DemandElementGeometry::DemandElementGeometry() {}


void
GNEGeometry::DemandElementGeometry::clearGeometry() {
    shape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
}


void
GNEGeometry::DemandElementGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // Calculate lengths and rotations for every shape
        for (int i = 0; i < numberOfSegments; i++) {
            shapeRotations.push_back(calculateRotation(shape[i], shape[i + 1]));
            shapeLengths.push_back(calculateLength(shape[i], shape[i + 1]));
        }
    }
}

// ---------------------------------------------------------------------------
// GNEGeometry::DemandElementSegmentGeometry::Segment - methods
// ---------------------------------------------------------------------------

GNEGeometry::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNELane* lane, const bool _valid) :
    element(_element),
    edge(&lane->getParentEdge()),
    myLane(lane),
    junction(nullptr),
    valid(_valid),
    myUseLaneShape(true) {
}


GNEGeometry::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNELane* lane,
        const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths, const bool _valid) :
    element(_element),
    edge(&lane->getParentEdge()),
    myLane(lane),
    junction(nullptr),
    valid(_valid),
    myUseLaneShape(false),
    mySegmentShape(shape),
    mySegmentRotations(shapeRotations),
    mySegmentLengths(shapeLengths) {
}


GNEGeometry::DemandElementSegmentGeometry::Segment::Segment(const GNEDemandElement* _element, const GNELane* currentLane, const GNELane* nextLane, const bool _valid) :
    element(_element),
    edge(nullptr),
    myLane(nextLane),
    junction(currentLane->getParentEdge().getGNEJunctionDestiny()),
    valid(_valid),
    myUseLaneShape(false),
    mySegmentShape(currentLane->getLane2laneConnections().shapesMap.at(nextLane)),
    mySegmentRotations(currentLane->getLane2laneConnections().shapeRotationsMap.at(nextLane)),
    mySegmentLengths(currentLane->getLane2laneConnections().shapeLengthsMap.at(nextLane)) {
}


void 
GNEGeometry::DemandElementSegmentGeometry::Segment::update(const PositionVector& shape, const std::vector<double> &shapeRotations, const std::vector<double> &shapeLengths) {
    mySegmentShape = shape;
    mySegmentRotations = shapeRotations;
    mySegmentLengths = shapeLengths;
}


GNEGeometry::DemandElementSegmentGeometry::SegmentToUpdate::SegmentToUpdate(const int _index, const GNELane* _lane, const GNELane* _nextLane) :
    index(_index),
    lane(_lane),
    nextLane(_nextLane) {
}


const PositionVector&
GNEGeometry::DemandElementSegmentGeometry::Segment::getShape() const {
    if (myUseLaneShape) {
        return myLane->getLaneShape();
    } else {
        return mySegmentShape;
    }
}


const std::vector<double>&
GNEGeometry::DemandElementSegmentGeometry::Segment::getShapeRotations() const {
    if (myUseLaneShape) {
        return myLane->getShapeRotations();
    } else {
        return mySegmentRotations;
    }
}
            

const std::vector<double>&
GNEGeometry::DemandElementSegmentGeometry::Segment::getShapeLengths() const {
    if (myUseLaneShape) {
        return myLane->getShapeLengths();
    } else {
        return mySegmentLengths;
    }
}

// ---------------------------------------------------------------------------
// GNEGeometry::DemandElementGeometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::DemandElementSegmentGeometry::DemandElementSegmentGeometry() {}


void 
GNEGeometry::DemandElementSegmentGeometry::insertLaneSegment(const GNEDemandElement* element, const GNELane* lane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, lane, valid));
}


void 
GNEGeometry::DemandElementSegmentGeometry::insertCustomSegment(const GNEDemandElement* element, const GNELane* lane,
    const PositionVector& laneShape, const std::vector<double> &laneShapeRotations, const std::vector<double> &laneShapeLengths, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, lane, laneShape, laneShapeRotations, laneShapeLengths, valid));
}


void
GNEGeometry::DemandElementSegmentGeometry::insertLane2LaneSegment(const GNEDemandElement* element, const GNELane* currentLane, const GNELane* nextLane, const bool valid) {
    // add segment in myShapeSegments
    myShapeSegments.push_back(Segment(element, currentLane, nextLane, valid));
}


void 
GNEGeometry::DemandElementSegmentGeometry::updateCustomSegment(const int segmentIndex, const PositionVector& newLaneShape, const std::vector<double> &newLaneShapeRotations, const std::vector<double> &newLaneShapeLengths) {
    myShapeSegments.at(segmentIndex).update(newLaneShape, newLaneShapeRotations, newLaneShapeLengths);
}


void 
GNEGeometry::DemandElementSegmentGeometry::updateLane2LaneSegment(const int segmentIndex, const GNELane* lane, const GNELane* nextLane) {
    myShapeSegments.at(segmentIndex+1).update(
        lane->getLane2laneConnections().shapesMap.at(nextLane), 
        lane->getLane2laneConnections().shapeRotationsMap.at(nextLane), 
        lane->getLane2laneConnections().shapeLengthsMap.at(nextLane));
}


void
GNEGeometry::DemandElementSegmentGeometry::clearDemandElementSegmentGeometry() {
    // clear segments
    myShapeSegments.clear();
}


const Position &
GNEGeometry::DemandElementSegmentGeometry::getFirstPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().getShape().size() > 0)) {
        return myShapeSegments.front().getShape().front();
    } else {
        return Position::INVALID;
    }
}

const Position &
GNEGeometry::DemandElementSegmentGeometry::getLastPosition() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.back().getShape().size() > 0)) {
        return myShapeSegments.back().getShape().back();
    } else {
        return Position::INVALID;
    }
}


double 
GNEGeometry::DemandElementSegmentGeometry::getFirstRotation() const {
    if ((myShapeSegments.size() > 0) && (myShapeSegments.front().getShapeRotations().size() > 0)) {
        return myShapeSegments.front().getShapeRotations().front();
    } else {
        return 0;
    }
}


std::vector<GNEGeometry::DemandElementSegmentGeometry::Segment>::const_iterator
GNEGeometry::DemandElementSegmentGeometry::begin() const {
    return myShapeSegments.cbegin();
}


std::vector<GNEGeometry::DemandElementSegmentGeometry::Segment>::const_iterator
GNEGeometry::DemandElementSegmentGeometry::end() const {
    return myShapeSegments.cend();
}


const GNEGeometry::DemandElementSegmentGeometry::Segment&
GNEGeometry::DemandElementSegmentGeometry::front() const {
    return myShapeSegments.front();
}


const GNEGeometry::DemandElementSegmentGeometry::Segment&
GNEGeometry::DemandElementSegmentGeometry::back() const {
    return myShapeSegments.back();
}


int 
GNEGeometry::DemandElementSegmentGeometry::size() const {
    return (int)myShapeSegments.size();
}


double 
GNEGeometry::calculateRotation(const Position& first, const Position& second) {
    // return rotation (angle) of the vector constructed by points first and second
    return ((double)atan2((second.x() - first.x()), (first.y() - second.y())) * (double) 180.0 / (double)M_PI);
}


double 
GNEGeometry::calculateLength(const Position& first, const Position& second) {
    // return distance between two points
    return first.distanceTo2D(second);
}

/****************************************************************************/
