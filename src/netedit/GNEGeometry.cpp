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
#include <netedit/demandelements/GNEDemandElement.h>
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
GNEGeometry::calculateGeometricPath(GNEDemandElement* demandElement, GNEGeometry::DemandElementSegmentGeometry &segmentGeometry, 
        const std::vector<GNEEdge*> &edges, GNELane *fromLane, GNELane *toLane, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // clear geometry
    segmentGeometry.clearDemandElementSegmentGeometry();
    // first check that there is edge parents
    if (edges.size() > 0) {
        // calculate depending if both from and to edges are the same
        if (fromLane == toLane) {
            // if obtained lane is null, then force to use first lane
            if (fromLane == nullptr) {
                fromLane = edges.front()->getLanes().front();
            }
            // filter start and end pos
            adjustStartPosGeometricPath(startPos, fromLane, endPos, fromLane);
            // check if we have to define a new custom Segment, or we can use the commonLane shape
            if ((startPos != -1) || (endPos != -1) || (extraFirstPosition != Position::INVALID) || (extraLastPosition != Position::INVALID)) {
                // declare a lane to be trimmed
                GNENetElement::NetElementGeometry trimmedLane;
                // set shape lane
                trimmedLane.shape = fromLane->getLaneShape();
                // split depending of startPos and endPos
                if ((startPos != -1) && (endPos != -1)) {
                    // split lane
                    trimmedLane.shape = trimmedLane.shape.getSubpart(startPos, endPos);
                } else if (startPos != -1) {
                    // split lane
                    trimmedLane.shape = trimmedLane.shape.splitAt(startPos).second;
                } else if (endPos != -1) {
                    // split lane
                    trimmedLane.shape = trimmedLane.shape.splitAt(endPos).first;
                }
                // check if we have to add an extra first position
                if (extraFirstPosition != Position::INVALID) {
                    trimmedLane.shape.push_front(extraFirstPosition);
                }
                    // check if we have to add an extra last position
                if (extraLastPosition != Position::INVALID) {
                    trimmedLane.shape.push_back(extraLastPosition);
                }
                // calculate shape rotations and lenghts
                trimmedLane.calculateShapeRotationsAndLengths();
                // add sublane geometry
                segmentGeometry.insertCustomSegment(demandElement, fromLane,
                    trimmedLane.shape, 
                    trimmedLane.shapeRotations, 
                    trimmedLane.shapeLengths, true);
            } else {
                // add entire lane geometry geometry
                segmentGeometry.insertLaneSegment(demandElement, fromLane, true);
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
                } else if (demandElement->getTagProperty().isRide()) {
                    // obtain first disallowed lane (special case for rides)
                    allowedLane = (*edgeParent)->getLaneByDisallowedVClass(demandElement->getVClass());
                } else {
                    // obtain first allowed lane
                    allowedLane = (*edgeParent)->getLaneByAllowedVClass(demandElement->getVClass());
                }
                // if there isn't allowed lane, then use first lane
                if (allowedLane == nullptr) {
                    allowedLane = (*edgeParent)->getLanes().front();
                }
                // add it to lanes
                lanes.push_back(allowedLane);
            }
            // iterate over obtained lanes
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
                        GNENetElement::NetElementGeometry frontTrimmedLane;
                        // set shape lane
                        frontTrimmedLane.shape = lanes.at(i)->getLaneShape();
                        // split lane
                        if (startPos != -1) {
                            frontTrimmedLane.shape = frontTrimmedLane.shape.splitAt(startPos).second;
                        }
                        // check if we have to add an extra first position
                        if (extraFirstPosition != Position::INVALID) {
                            frontTrimmedLane.shape.push_front(extraFirstPosition);
                        }
                        // calculate shape rotations and lenghts
                        frontTrimmedLane.calculateShapeRotationsAndLengths();
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(demandElement, lane,
                            frontTrimmedLane.shape, 
                            frontTrimmedLane.shapeRotations, 
                            frontTrimmedLane.shapeLengths, true);
                    } else if ((lane == lanes.back()) && (endPos != -1)) {
                        // filter end position
                        adjustStartPosGeometricPath(startPos, nullptr, endPos, lane);
                        // declare a lane to be trimmed
                        GNENetElement::NetElementGeometry backTrimmedLane;
                        // set shape lane
                        backTrimmedLane.shape = lane->getLaneShape();
                        // split lane
                        if (endPos != -1) {
                            backTrimmedLane.shape = backTrimmedLane.shape.splitAt(endPos).first;
                        }
                        // check if we have to add an extra last position
                        if (extraLastPosition != Position::INVALID) {
                            backTrimmedLane.shape.push_back(extraLastPosition);
                        }
                        // calculate shape rotations and lenghts
                        backTrimmedLane.calculateShapeRotationsAndLengths();
                        // add sublane geometry
                        segmentGeometry.insertCustomSegment(demandElement, lane,
                            backTrimmedLane.shape, 
                            backTrimmedLane.shapeRotations, 
                            backTrimmedLane.shapeLengths, true);
                    } else {
                        // add entire lane geometry
                        segmentGeometry.insertLaneSegment(demandElement, lanes.at(i), true);
                    }
                }
                // now continue with connection
                if ((i+1) < (int)lanes.size()) {
                    // obtain next lane
                    const GNELane* nextLane = lanes.at(i+1);
                    // check that next lane exist
                    if (lane->getLane2laneConnections().shapesMap.count(nextLane) > 0) {
                        // add lane2laneConnection segment geometry
                        segmentGeometry.insertLane2LaneSegment(demandElement, lane, nextLane, true);
                    }
                }
            }
        }
    }
}


void 
GNEGeometry::updateGeometricPath(GNEDemandElement* demandElement, GNEGeometry::DemandElementSegmentGeometry &segmentGeometry, 
        const GNEEdge* edge, GNELane *fromLane, GNELane *toLane, double startPos, double endPos, 
    const Position &extraFirstPosition, const Position &extraLastPosition) {
    // calculate depending if both from and to edges are the same
    if ((segmentGeometry.size() == 1) && (segmentGeometry.front().edge == edge)) {
        // if obtained lane is null, then force to use first lane
        if (fromLane == nullptr) {
            fromLane = edge->getLanes().front();
        }
        // filter start and end pos
        adjustStartPosGeometricPath(startPos, fromLane, endPos, fromLane);
        // check if we have to define a new custom Segment, or we can use the commonLane shape
        if ((startPos != -1) || (endPos != -1) || (extraFirstPosition != Position::INVALID) || (extraLastPosition != Position::INVALID)) {
            // declare a lane to be trimmed
            GNENetElement::NetElementGeometry trimmedLane;
            // set shape lane
            trimmedLane.shape = fromLane->getLaneShape();
            // split depending of startPos and endPos
            if ((startPos != -1) && (endPos != -1)) {
                // split lane
                trimmedLane.shape = trimmedLane.shape.getSubpart(startPos, endPos);
            } else if (startPos != -1) {
                // split lane
                trimmedLane.shape = trimmedLane.shape.splitAt(startPos).second;
            } else if (endPos != -1) {
                // split lane
                trimmedLane.shape = trimmedLane.shape.splitAt(endPos).first;
            }
            // check if we have to add an extra first position
            if (extraFirstPosition != Position::INVALID) {
                trimmedLane.shape.push_front(extraFirstPosition);
            }
                // check if we have to add an extra last position
            if (extraLastPosition != Position::INVALID) {
                trimmedLane.shape.push_back(extraLastPosition);
            }
            // calculate shape rotations and lenghts
            trimmedLane.calculateShapeRotationsAndLengths();
            // add sublane geometry
            segmentGeometry.updateCustomSegment(0,
                trimmedLane.shape, 
                trimmedLane.shapeRotations, 
                trimmedLane.shapeLengths);
        }
    } else {
        // declare a vector to save segments to update
        std::vector<GNEGeometry::DemandElementSegmentGeometry::SegmentToUpdate> segmentsToUpdate;
        // iterate over all segments
        for (auto segment = segmentGeometry.begin(); segment != segmentGeometry.end(); segment++) {
            if (segment->edge == edge) {
                // obtain segment index
                const int index = (int)(segment - segmentGeometry.begin());
                // add SegmentToUpdate in vector
                segmentsToUpdate.push_back(GNEGeometry::DemandElementSegmentGeometry::SegmentToUpdate(index, segment->myLane, nullptr));
                // check if we have to add the next segment (it correspond to a lane2lane
                if (((segment+1) != segmentGeometry.end()) && (segment+1)->junction) {
                    segmentsToUpdate.push_back(GNEGeometry::DemandElementSegmentGeometry::SegmentToUpdate(index, segment->myLane, (segment+1)->myLane));
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
                    GNENetElement::NetElementGeometry frontTrimmedLane;
                    // set shape lane
                    frontTrimmedLane.shape = segmentToUpdate.lane->getLaneShape();
                    // split lane
                    if (startPos != -1) {
                        frontTrimmedLane.shape = frontTrimmedLane.shape.splitAt(startPos).second;
                    }
                    // check if we have to add an extra first position
                    if (extraFirstPosition != Position::INVALID) {
                        frontTrimmedLane.shape.push_front(extraFirstPosition);
                    }
                    // calculate shape rotations and lenghts
                    frontTrimmedLane.calculateShapeRotationsAndLengths();
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.index,
                        frontTrimmedLane.shape, 
                        frontTrimmedLane.shapeRotations, 
                        frontTrimmedLane.shapeLengths);
                } else if ((segmentToUpdate.index == (segmentGeometry.size() -1)) && (endPos != -1)) {
                    // filter end position
                    adjustStartPosGeometricPath(startPos, nullptr, endPos, segmentToUpdate.lane);
                    // declare a lane to be trimmed
                    GNENetElement::NetElementGeometry backTrimmedLane;
                    // set shape lane
                    backTrimmedLane.shape = segmentToUpdate.lane->getLaneShape();
                    // split lane
                    if (endPos != -1) {
                        backTrimmedLane.shape = backTrimmedLane.shape.splitAt(endPos).first;
                    }
                    // check if we have to add an extra last position
                    if (extraLastPosition != Position::INVALID) {
                        backTrimmedLane.shape.push_back(extraLastPosition);
                    }
                    // calculate shape rotations and lenghts
                    backTrimmedLane.calculateShapeRotationsAndLengths();
                    // update segment
                    segmentGeometry.updateCustomSegment(segmentToUpdate.index,
                        backTrimmedLane.shape, 
                        backTrimmedLane.shapeRotations, 
                        backTrimmedLane.shapeLengths);
                }
            }
            // check that next lane exist
            if (segmentToUpdate.lane->getLane2laneConnections().shapesMap.count(segmentToUpdate.nextLane) > 0) {
                // update lane2laneConnection shape
                segmentGeometry.updateLane2LaneSegment(segmentToUpdate.index, segmentToUpdate.lane, segmentToUpdate.nextLane); 
            }
        }
    }
}

/****************************************************************************/
