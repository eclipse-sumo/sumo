/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/options/OptionsCont.h>

#include "GNEGeometry.h"
#include "GNENet.h"
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
// GNEGeometry::Geometry - methods
// ---------------------------------------------------------------------------

GNEGeometry::Geometry::Geometry() {
}


GNEGeometry::Geometry::Geometry(const PositionVector& shape) :
    myShape(shape) {
    // calculate shape rotation and lenghts
    calculateShapeRotationsAndLengths();
}


GNEGeometry::Geometry::Geometry(const PositionVector& shape, const std::vector<double>& shapeRotations, const std::vector<double>& shapeLengths) :
    myShape(shape),
    myShapeRotations(shapeRotations),
    myShapeLengths(shapeLengths) {
}


void
GNEGeometry::Geometry::updateGeometry(const PositionVector& shape) {
    // clear geometry
    clearGeometry();
    // update shape
    myShape = shape;
    // calculate shape rotation and lenghts
    calculateShapeRotationsAndLengths();
}


void
GNEGeometry::Geometry::updateGeometry(const PositionVector& shape, const double posOverShape, const double lateralOffset) {
    // first clear geometry
    clearGeometry();
    // get shape length
    const double shapeLength = shape.length();
    // calculate position and rotation
    if (posOverShape < 0) {
        myShape.push_back(shape.positionAtOffset(0, lateralOffset));
        myShapeRotations.push_back(shape.rotationDegreeAtOffset(0));
    } else if (posOverShape > shapeLength) {
        myShape.push_back(shape.positionAtOffset(shapeLength, lateralOffset));
        myShapeRotations.push_back(shape.rotationDegreeAtOffset(shapeLength));
    } else {
        myShape.push_back(shape.positionAtOffset(posOverShape, lateralOffset));
        myShapeRotations.push_back(shape.rotationDegreeAtOffset(posOverShape));
    }
}


void
GNEGeometry::Geometry::updateGeometry(const PositionVector& shape, double starPosOverShape, double endPosOverShape, const double lateralOffset) {
    // first clear geometry
    clearGeometry();
    // set new shape
    myShape = shape;
    // set lateral offset
    myShape.move2side(lateralOffset);
    // get shape lenght
    const double shapeLength = myShape.length2D();
    // set initial beginTrim value
    if (starPosOverShape < 0) {
        endPosOverShape = 0;
    }
    // set initial endtrim value
    if (starPosOverShape < 0) {
        endPosOverShape = shapeLength;
    }
    // check maximum beginTrim
    if (starPosOverShape > (shapeLength - POSITION_EPS)) {
        endPosOverShape = (shapeLength - POSITION_EPS);
    }
    // check maximum endTrim
    if ((endPosOverShape > shapeLength)) {
        endPosOverShape = shapeLength;
    }
    // check sub-vector
    if (endPosOverShape <= starPosOverShape) {
        endPosOverShape = endPosOverShape + POSITION_EPS;
    }
    // trim shape
    myShape = myShape.getSubpart2D(starPosOverShape, endPosOverShape);
    // calculate shape rotation and lenghts
    calculateShapeRotationsAndLengths();
}


void
GNEGeometry::Geometry::updateGeometry(const PositionVector& shape, double beginTrimPosition, double endTrimPosition,
                                      const Position& extraFirstPosition, const Position& extraLastPosition) {
    // first clear geometry
    clearGeometry();
    // set new shape
    myShape = shape;
    // check trim values
    if ((beginTrimPosition != -1) || (endTrimPosition != -1)) {
        // get shape lenght
        const double shapeLength = myShape.length2D();
        // set initial beginTrim value
        if (beginTrimPosition < 0) {
            beginTrimPosition = 0;
        }
        // set initial endtrim value
        if (endTrimPosition < 0) {
            endTrimPosition = shapeLength;
        }
        // check maximum beginTrim
        if (beginTrimPosition > (shapeLength - POSITION_EPS)) {
            beginTrimPosition = (shapeLength - POSITION_EPS);
        }
        // check maximum endTrim
        if ((endTrimPosition > shapeLength)) {
            endTrimPosition = shapeLength;
        }
        // check sub-vector
        if (endTrimPosition <= beginTrimPosition) {
            endTrimPosition = endTrimPosition + POSITION_EPS;
        }
        // trim shape
        myShape = myShape.getSubpart2D(beginTrimPosition, endTrimPosition);
        // add extra positions
        if (extraFirstPosition != Position::INVALID) {
            myShape.push_front_noDoublePos(extraFirstPosition);
        }
        if (extraLastPosition != Position::INVALID) {
            myShape.push_back_noDoublePos(extraLastPosition);
        }
    }
    // calculate shape rotation and lenghts
    calculateShapeRotationsAndLengths();
}


void
GNEGeometry::Geometry::updateSinglePosGeometry(const Position& position, const double rotation) {
    // first clear geometry
    clearGeometry();
    // set position and rotation
    myShape.push_back(position);
    myShapeRotations.push_back(rotation);
}


void
GNEGeometry::Geometry::scaleGeometry(const double scale) {
    // scale shape and lenghts
    myShape.scaleRelative(scale);
    // scale lenghts
    for (auto& shapeLength : myShapeLengths) {
        shapeLength *= scale;
    }
}


const PositionVector&
GNEGeometry::Geometry::getShape() const {
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


void GNEGeometry::Geometry::clearGeometry() {
    // clear geometry containers
    myShape.clear();
    myShapeRotations.clear();
    myShapeLengths.clear();
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
// GNEGeometry::DottedGeometryColor - methods
// ---------------------------------------------------------------------------

GNEGeometry::DottedGeometryColor::DottedGeometryColor(const GUIVisualizationSettings& settings) :
    mySettings(settings),
    myColorFlag(true) {}


const RGBColor&
GNEGeometry::DottedGeometryColor::getInspectedColor() {
    if (myColorFlag) {
        myColorFlag = false;
        return mySettings.dottedContourSettings.firstInspectedColor;
    } else {
        myColorFlag = true;
        return mySettings.dottedContourSettings.secondInspectedColor;
    }
}


const RGBColor&
GNEGeometry::DottedGeometryColor::getFrontColor() {
    if (myColorFlag) {
        myColorFlag = false;
        return mySettings.dottedContourSettings.firstFrontColor;
    } else {
        myColorFlag = true;
        return mySettings.dottedContourSettings.secondFrontColor;
    }
}


void
GNEGeometry::DottedGeometryColor::changeColor() {
    if (myColorFlag) {
        myColorFlag = false;
    } else {
        myColorFlag = true;
    }
}


void
GNEGeometry::DottedGeometryColor::reset() {
    myColorFlag = true;
}

// ---------------------------------------------------------------------------
// GNEGeometry::DottedGeometry - methods
// ---------------------------------------------------------------------------


GNEGeometry::DottedGeometry::Segment::Segment() :
    offset(-1) {
}


GNEGeometry::DottedGeometry::Segment::Segment(PositionVector newShape) :
    shape(newShape),
    offset(-1) {
}


GNEGeometry::DottedGeometry::DottedGeometry() :
    myWidth(0) {
}


#if defined(_MSC_VER) && _MSC_VER == 1800
#pragma warning(push)
#pragma warning(disable: 4100) // do not warn about "unused" parameters which get optimized away
#endif
GNEGeometry::DottedGeometry::DottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape) :
    myWidth(s.dottedContourSettings.segmentWidth) {
    // check if shape has to be closed
    if (closeShape && (shape.size() > 2)) {
        shape.closePolygon();
    }
    if (shape.size() > 1) {
        // get shape
        for (int i = 1; i < (int)shape.size(); i++) {
            myDottedGeometrySegments.push_back(Segment({shape[i - 1], shape[i]}));
        }
        // resample
        for (auto& segment : myDottedGeometrySegments) {
            segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
        }
        // calculate shape rotations and lenghts
        calculateShapeRotationsAndLengths();
    }
}


GNEGeometry::DottedGeometry::DottedGeometry(const GUIVisualizationSettings& s,
        const DottedGeometry& topDottedGeometry, const bool drawFirstExtrem,
        const DottedGeometry& botDottedGeometry, const bool drawLastExtrem) :
    myWidth(s.dottedContourSettings.segmentWidth) {
    // check size of both geometries
    if ((topDottedGeometry.myDottedGeometrySegments.size() > 0) &&
            (botDottedGeometry.myDottedGeometrySegments.size() > 0)) {
        // add extremes
        if (drawFirstExtrem &&
                (topDottedGeometry.myDottedGeometrySegments.front().shape.size() > 0) &&
                (botDottedGeometry.myDottedGeometrySegments.front().shape.size() > 0)) {
            // add first extreme
            myDottedGeometrySegments.push_back(Segment({
                topDottedGeometry.myDottedGeometrySegments.front().shape.front(),
                botDottedGeometry.myDottedGeometrySegments.front().shape.front()}));
        }
        if (drawLastExtrem &&
                (topDottedGeometry.myDottedGeometrySegments.back().shape.size() > 0) &&
                (botDottedGeometry.myDottedGeometrySegments.back().shape.size() > 0)) {
            // add last extreme
            myDottedGeometrySegments.push_back(Segment({
                topDottedGeometry.myDottedGeometrySegments.back().shape.back(),
                botDottedGeometry.myDottedGeometrySegments.back().shape.back()}));
            // invert offset of second dotted geometry
            myDottedGeometrySegments.back().offset *= -1;
        }
    }
    // resample
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
    }
    // calculate shape rotations and lenghts
    calculateShapeRotationsAndLengths();
}


void
GNEGeometry::DottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, const GNELane* lane) {
    // update settings and width
    myWidth = s.dottedContourSettings.segmentWidth;
    // reset segments
    myDottedGeometrySegments.clear();
    // get shape
    for (int i = 1; i < (int)lane->getLaneShape().size(); i++) {
        myDottedGeometrySegments.push_back(Segment({lane->getLaneShape()[i - 1], lane->getLaneShape()[i]}));
    }
    // resample
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
    }
    // calculate shape rotations and lenghts
    calculateShapeRotationsAndLengths();
}


void
GNEGeometry::DottedGeometry::updateDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape) {
    // update settings and width
    myWidth = s.dottedContourSettings.segmentWidth;
    // reset segments
    myDottedGeometrySegments.clear();
    // check if shape has to be closed
    if (closeShape && (shape.size() > 2)) {
        shape.closePolygon();
    }
    if (shape.size() > 1) {
        // get shape
        for (int i = 1; i < (int)shape.size(); i++) {
            myDottedGeometrySegments.push_back(Segment({shape[i - 1], shape[i]}));
        }
        // resample
        for (auto& segment : myDottedGeometrySegments) {
            segment.shape = segment.shape.resample(s.dottedContourSettings.segmentLength, true);
        }
        // calculate shape rotations and lenghts
        calculateShapeRotationsAndLengths();
    }
}
#if defined(_MSC_VER) && _MSC_VER == 1800
#pragma warning(pop)
#endif


void
GNEGeometry::DottedGeometry::drawInspectedDottedGeometry(DottedGeometryColor& dottedGeometryColor) const {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        // iterate over shape
        for (int i = 0; i < ((int)segment.shape.size() - 1); i++) {
            // set color
            GLHelper::setColor(dottedGeometryColor.getInspectedColor());
            // draw box line
            GLHelper::drawBoxLine(segment.shape[i],
                                  segment.rotations.at(i),
                                  segment.lengths.at(i),
                                  myWidth, myWidth * segment.offset);
        }
    }
}


void
GNEGeometry::DottedGeometry::drawFrontDottedGeometry(DottedGeometryColor& dottedGeometryColor) const {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        // iterate over shape
        for (int i = 0; i < ((int)segment.shape.size() - 1); i++) {
            // set color
            GLHelper::setColor(dottedGeometryColor.getFrontColor());
            // draw box line
            GLHelper::drawBoxLine(segment.shape[i],
                                  segment.rotations.at(i),
                                  segment.lengths.at(i),
                                  myWidth, myWidth * segment.offset);
        }
    }
}


void
GNEGeometry::DottedGeometry::moveShapeToSide(const double value) {
    // move 2 side
    for (auto& segment : myDottedGeometrySegments) {
        segment.shape.move2side(value);
    }
}


double
GNEGeometry::DottedGeometry::getWidth() const {
    return myWidth;
}


void
GNEGeometry::DottedGeometry::setWidth(const double width) {
    myWidth = width;
}


void
GNEGeometry::DottedGeometry::invertOffset() {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        segment.offset *= -1;
    }
}


void
GNEGeometry::DottedGeometry::calculateShapeRotationsAndLengths() {
    // iterate over all segments
    for (auto& segment : myDottedGeometrySegments) {
        // Get number of parts of the shape
        int numberOfSegments = (int)segment.shape.size() - 1;
        // If number of segments is more than 0
        if (numberOfSegments >= 0) {
            // Reserve memory (To improve efficiency)
            segment.rotations.reserve(numberOfSegments);
            segment.lengths.reserve(numberOfSegments);
            // Calculate lengths and rotations for every shape
            for (int i = 0; i < numberOfSegments; i++) {
                segment.rotations.push_back(calculateRotation(segment.shape[i], segment.shape[i + 1]));
                segment.lengths.push_back(calculateLength(segment.shape[i], segment.shape[i + 1]));
            }
        }
    }
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
    for (const auto& outgoingEdge : myFromLane->getParentEdge()->getParentJunctions().back()->getGNEOutgoingEdges()) {
        for (const auto& outgoingLane : outgoingEdge->getLanes()) {
            // get NBEdges from and to
            const NBEdge* NBEdgeFrom = myFromLane->getParentEdge()->getNBEdge();
            const NBEdge* NBEdgeTo = outgoingLane->getParentEdge()->getNBEdge();
            // declare shape
            PositionVector shape;
            // only create smooth shapes if Edge From has as maximum 10 lanes
            if ((NBEdgeFrom->getNumLanes() <= maximumLanes) && (NBEdgeFrom->getToNode()->getShape().area() > 4)) {
                // calculate smoot shape
                shape = NBEdgeFrom->getToNode()->computeSmoothShape(
                            NBEdgeFrom->getLaneShape(myFromLane->getIndex()),
                            NBEdgeTo->getLaneShape(outgoingLane->getIndex()),
                            numPoints, NBEdgeFrom->getTurnDestination() == NBEdgeTo,
                            (double) numPoints * (double) NBEdgeFrom->getNumLanes(),
                            (double) numPoints * (double) NBEdgeTo->getNumLanes());
            } else {
                // create a shape using lane shape extremes
                shape = {myFromLane->getLaneShape().back(), outgoingLane->getLaneShape().front()};
            }
            // update connection map
            myConnectionsMap[outgoingLane].first.updateGeometry(shape);
            if (myFromLane->getNet()->getViewNet()) {
                myConnectionsMap[outgoingLane].second.updateDottedGeometry(myFromLane->getNet()->getViewNet()->getVisualisationSettings(), shape, false);
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
    return myConnectionsMap.at(toLane).first;
}


const GNEGeometry::DottedGeometry&
GNEGeometry::Lane2laneConnection::getLane2laneDottedGeometry(const GNELane* toLane) const {
    return myConnectionsMap.at(toLane).second;
}


GNEGeometry::Lane2laneConnection::Lane2laneConnection() :
    myFromLane(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEGeometry::HierarchicalConnections - methods
// ---------------------------------------------------------------------------

GNEGeometry::HierarchicalConnections::ConnectionGeometry::ConnectionGeometry(GNELane* lane) :
    myLane(lane),
    myRotation(0) {
    // set position and length depending of shape's lengt
    if (lane->getLaneShape().length() - 6 > 0) {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
    } else {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
    }
}


const GNELane*
GNEGeometry::HierarchicalConnections::ConnectionGeometry::getLane() const {
    return myLane;
}


const Position&
GNEGeometry::HierarchicalConnections::ConnectionGeometry::getPosition() const {
    return myPosition;
}


double
GNEGeometry::HierarchicalConnections::ConnectionGeometry::getRotation() const {
    return myRotation;
}


GNEGeometry::HierarchicalConnections::ConnectionGeometry::ConnectionGeometry() :
    myLane(nullptr),
    myRotation(0) {
}


GNEGeometry::HierarchicalConnections::HierarchicalConnections(GNEHierarchicalElement* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEGeometry::HierarchicalConnections::update() {
    // first clear containers
    connectionsGeometries.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& edge : myHierarchicalElement->getChildEdges()) {
        for (const auto& lane : edge->getLanes()) {
            symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& lane : myHierarchicalElement->getChildLanes()) {
        symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
    }
    // calculate position for every child additional
    for (const auto& additional : myHierarchicalElement->getChildAdditionals()) {
        // check that additional position is different of parent position
        if (additional->getTagProperty().isDrawable() && (additional->getPositionInView() != myHierarchicalElement->getPositionInView())) {
            // create connection shape
            std::vector<Position> connectionShape;
            const double A = std::abs(additional->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            const double B = std::abs(additional->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            connectionShape.push_back(additional->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > additional->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x() + A, additional->getPositionInView().y()));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() + B));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x() - A, additional->getPositionInView().y()));
                }
            }
            connectionShape.push_back(myHierarchicalElement->getPositionInView());
            // declare Geometry
            GNEGeometry::Geometry geometry;
            // update geometry with connectino shape
            geometry.updateGeometry(connectionShape);
            // add geometry in connectionsGeometry
            connectionsGeometries.push_back(geometry);
        }
    }
    // calculate geometry for connections between parent and children
    for (const auto& symbol : symbolsPositionAndRotation) {
        // create connection shape
        std::vector<Position> connectionShape;
        const double A = std::abs(symbol.getPosition().x() - myHierarchicalElement->getPositionInView().x());
        const double B = std::abs(symbol.getPosition().y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        connectionShape.push_back(symbol.getPosition());
        if (myHierarchicalElement->getPositionInView().x() > symbol.getPosition().x()) {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x() + A, symbol.getPosition().y()));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() + B));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x() - A, symbol.getPosition().y()));
            }
        }
        connectionShape.push_back(myHierarchicalElement->getPositionInView());
        // declare Geometry
        GNEGeometry::Geometry geometry;
        // update geometry with connectino shape
        geometry.updateGeometry(connectionShape);
        // add geometry in connectionsGeometry
        connectionsGeometries.push_back(geometry);
    }
}


void
GNEGeometry::HierarchicalConnections::drawConnection(const GUIVisualizationSettings& s, const GNEAttributeCarrier* AC, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Set color
        if (AC->isAttributeCarrierSelected()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor.changedBrightness(-32));
        } else {
            GLHelper::setColor(s.colorSettings.childConnections);
        }
        // Draw box lines
        GLHelper::drawBoxLines(connectionGeometry.getShape(), connectionGeometry.getShapeRotations(), connectionGeometry.getShapeLengths(), exaggeration * 0.1);
        // Pop draw matrix
        GLHelper::popMatrix();
    }
}


void
GNEGeometry::HierarchicalConnections::drawDottedConnection(const DottedContourType type, const GUIVisualizationSettings& s, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // calculate dotted geometry
        GNEGeometry::DottedGeometry dottedGeometry(s, connectionGeometry.getShape(), false);
        // Add a draw matrix
        GLHelper::pushMatrix();
        // traslate back
        if (type == DottedContourType::INSPECT) {
            glTranslated(0, 0, (-1 * GLO_DOTTEDCONTOUR_INSPECTED) - 0.01);
        } else if (type == DottedContourType::FRONT) {
            glTranslated(0, 0, (-1 * GLO_DOTTEDCONTOUR_FRONT) - 0.01);
        }
        // change default width
        dottedGeometry.setWidth(0.1);
        // use drawDottedContourGeometry to draw it
        GNEGeometry::drawDottedContourGeometry(type, s, dottedGeometry, exaggeration * 0.1, false, false);
        // Pop draw matrix
        GLHelper::popMatrix();
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
GNEGeometry::drawGeometry(const GNEViewNet* viewNet, const Geometry& geometry, const double width) {
    // continue depending of draw for position selection
    if (viewNet->getVisualisationSettings().drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = geometry.getShape().positionAtOffset2D(geometry.getShape().nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width * width)) {
            // push matrix
            GLHelper::pushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings().getCircleResolution());
            // pop draw matrix
            GLHelper::popMatrix();
        }
    } else if (viewNet->getVisualisationSettings().scale * width < 1) {
        // draw line (needed for zoom out)
        GLHelper::drawLine(geometry.getShape());
    } else {
        GLHelper::drawBoxLines(geometry.getShape(), geometry.getShapeRotations(), geometry.getShapeLengths(), width);
    }
}


void
GNEGeometry::drawContourGeometry(const Geometry& geometry, const double width, const bool drawExtremes) {
    // get shapes
    PositionVector shapeA = geometry.getShape();
    PositionVector shapeB = geometry.getShape();
    // move both shapes
    shapeA.move2side((width - 0.1));
    shapeB.move2side((width - 0.1) * -1);
    // check if we have to drawn extremes
    if (drawExtremes) {
        // reverse shape B
        shapeB = shapeB.reverse();
        // append shape B to shape A
        shapeA.append(shapeB, 0);
        // close shape A
        shapeA.closePolygon();
        // draw box lines using shapeA
        GLHelper::drawBoxLines(shapeA, 0.1);
    } else {
        // draw box lines using shapeA
        GLHelper::drawBoxLines(shapeA, 0.1);
        // draw box lines using shapeA
        GLHelper::drawBoxLines(shapeB, 0.1);
    }
}


void
GNEGeometry::drawGeometryPoints(const GUIVisualizationSettings& s, const GNEViewNet* viewNet, const PositionVector& shape,
                                const RGBColor& geometryPointColor, const RGBColor& textColor, const double radius, const double exaggeration) {
    // get mouse position
    const Position mousePosition = viewNet->getPositionInformation();
    // get exaggeratedRadio
    const double exaggeratedRadio = (radius * exaggeration);
    // get radius squared
    const double exaggeratedRadioSquared = (exaggeratedRadio * exaggeratedRadio);
    // iterate over shape
    for (const auto& vertex : shape) {
        // if drawForPositionSelection is enabled, check distance between mouse and vertex
        if (!s.drawForPositionSelection || (mousePosition.distanceSquaredTo2D(vertex) <= exaggeratedRadioSquared)) {
            // push geometry point matrix
            GLHelper::pushMatrix();
            // move to vertex
            glTranslated(vertex.x(), vertex.y(), 0.2);
            // set color
            GLHelper::setColor(geometryPointColor);
            // draw circle
            GLHelper::drawFilledCircle(exaggeratedRadio, s.getCircleResolution());
            // pop geometry point matrix
            GLHelper::popMatrix();
            // draw elevation or special symbols (Start, End and Block)
            if (!s.drawForRectangleSelection && !s.drawForPositionSelection) {
                // get draw detail
                const bool drawDetail = s.drawDetail(s.detailSettings.geometryPointsText, exaggeration);
                // draw text
                if (viewNet->getNetworkViewOptions().editingElevation()) {
                    // Push Z matrix
                    GLHelper::pushMatrix();
                    // draw Z (elevation)
                    GLHelper::drawText(toString(vertex.z()), vertex, 0.3, 0.7, textColor);
                    // pop Z matrix
                    GLHelper::popMatrix();
                } else if ((vertex == shape.front()) && drawDetail) {
                    // push "S" matrix
                    GLHelper::pushMatrix();
                    // draw a "s" over first point
                    GLHelper::drawText("S", vertex, 0.3, 2 * exaggeratedRadio, textColor);
                    // pop "S" matrix
                    GLHelper::popMatrix();
                } else if ((vertex == shape.back()) && (shape.isClosed() == false) && drawDetail) {
                    // push "E" matrix
                    GLHelper::pushMatrix();
                    // draw a "e" over last point if polygon isn't closed
                    GLHelper::drawText("E", vertex, 0.3, 2 * exaggeratedRadio, textColor);
                    // pop "E" matrix
                    GLHelper::popMatrix();
                }
            }
        }
    }
}


void
GNEGeometry::drawMovingHint(const GUIVisualizationSettings& s, const GNEViewNet* viewNet, const PositionVector& shape,
                            const RGBColor& hintColor, const double radius, const double exaggeration) {
    // first NetworkEditMode
    if (viewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) {
        // get mouse position
        const Position mousePosition = viewNet->getPositionInformation();
        // get exaggeratedRadio
        const double exaggeratedRadio = (radius * exaggeration);
        // obtain distance to shape
        const double distanceToShape = shape.distance2D(mousePosition);
        // obtain squared radius
        const double squaredRadius = (radius * radius * exaggeration);
        // declare index
        int index = -1;
        // iterate over shape
        for (int i = 0; i < (int)shape.size(); i++) {
            // check distance
            if (shape[i].distanceSquaredTo2D(mousePosition) <= squaredRadius) {
                index = i;
            }
        }
        // continue depending of distance to shape
        if ((distanceToShape < exaggeratedRadio) && (index == -1)) {
            // obtain position over lane
            const Position positionOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePosition));
            // calculate hintPos
            const Position hintPos = shape.size() > 1 ? positionOverLane : shape[0];
            // push hintPos matrix
            GLHelper::pushMatrix();
            // translate to hintPos
            glTranslated(hintPos.x(), hintPos.y(), 0.2);
            // set color
            GLHelper::setColor(hintColor);
            // draw filled circle
            GLHelper:: drawFilledCircle(exaggeratedRadio, s.getCircleResolution());
            // pop hintPos matrix
            GLHelper::popMatrix();
        }
    }
}


void
GNEGeometry::drawLaneGeometry(const GNEViewNet* viewNet, const PositionVector& shape, const std::vector<double>& rotations,
                              const std::vector<double>& lengths, const std::vector<RGBColor>& colors, double width, const bool onlyContour) {
    // first check if we're in draw a contour or for selecting cliking mode
    if (onlyContour) {
        // get shapes
        PositionVector shapeA = shape;
        PositionVector shapeB = shape;
        // move both shapes
        shapeA.move2side((width - 0.1));
        shapeB.move2side((width - 0.1) * -1);
        // reverse shape B
        shapeB = shapeB.reverse();
        // append shape B to shape A
        shapeA.append(shapeB, 0);
        // close shape A
        shapeA.closePolygon();
        // draw box lines using shapeA
        GLHelper::drawBoxLines(shapeA, 0.1);
    } else if (viewNet->getVisualisationSettings().drawForPositionSelection) {
        // obtain mouse Position
        const Position mousePosition = viewNet->getPositionInformation();
        // obtain position over lane relative to mouse position
        const Position posOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePosition));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePosition) <= (width * width)) {
            // push matrix
            GLHelper::pushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, viewNet->getVisualisationSettings().getCircleResolution());
            // pop draw matrix
            GLHelper::popMatrix();
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
GNEGeometry::drawDottedContourGeometry(const DottedContourType type, const GUIVisualizationSettings& s, const DottedGeometry& dottedGeometry, const double width, const bool drawFirstExtrem, const bool drawLastExtrem) {
    // declare DottedGeometryColor
    DottedGeometryColor dottedGeometryColor(s);
    // make a copy of dotted geometry
    DottedGeometry topDottedGeometry = dottedGeometry;
    DottedGeometry botDottedGeometry = dottedGeometry;
    // move geometries
    topDottedGeometry.moveShapeToSide(width);
    botDottedGeometry.moveShapeToSide(width * -1);
    // invert offset of top dotted geometry
    topDottedGeometry.invertOffset();
    // calculate extremes
    DottedGeometry extremes(s, topDottedGeometry, drawFirstExtrem, botDottedGeometry, drawLastExtrem);
    // Push draw matrix
    GLHelper::pushMatrix();
    // draw inspect or front dotted contour
    if (type == DottedContourType::INSPECT) {
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
        // draw top dotted geometry
        topDottedGeometry.drawInspectedDottedGeometry(dottedGeometryColor);
        // reset color
        dottedGeometryColor.reset();
        // draw top dotted geometry
        botDottedGeometry.drawInspectedDottedGeometry(dottedGeometryColor);
        // change color
        dottedGeometryColor.changeColor();
        // draw extrem dotted geometry
        extremes.drawInspectedDottedGeometry(dottedGeometryColor);
    } else if (type == DottedContourType::FRONT) {
        // translate to front
        glTranslated(0, 0, GLO_DOTTEDCONTOUR_FRONT);
        // draw top dotted geometry
        topDottedGeometry.drawFrontDottedGeometry(dottedGeometryColor);
        // reset color
        dottedGeometryColor.reset();
        // draw top dotted geometry
        botDottedGeometry.drawFrontDottedGeometry(dottedGeometryColor);
        // change color
        dottedGeometryColor.changeColor();
        // draw extrem dotted geometry
        extremes.drawFrontDottedGeometry(dottedGeometryColor);
    }
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEGeometry::drawDottedContourEdge(const DottedContourType type, const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFrontExtreme, const bool drawBackExtreme) {
    if (edge->getLanes().size() == 1) {
        GNELane::LaneDrawingConstants laneDrawingConstants(s, edge->getLanes().front());
        GNEGeometry::drawDottedContourGeometry(type, s, edge->getLanes().front()->getDottedLaneGeometry(), laneDrawingConstants.halfWidth, drawFrontExtreme, drawBackExtreme);
    } else {
        // set left hand flag
        const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
        // obtain lanes
        const GNELane* topLane =  lefthand ? edge->getLanes().back() : edge->getLanes().front();
        const GNELane* botLane = lefthand ? edge->getLanes().front() : edge->getLanes().back();
        // obtain a copy of both geometries
        GNEGeometry::DottedGeometry dottedGeometryTop = topLane->getDottedLaneGeometry();
        GNEGeometry::DottedGeometry dottedGeometryBot = botLane->getDottedLaneGeometry();
        // obtain both LaneDrawingConstants
        GNELane::LaneDrawingConstants laneDrawingConstantsFront(s, topLane);
        GNELane::LaneDrawingConstants laneDrawingConstantsBack(s, botLane);
        // move shapes to side
        dottedGeometryTop.moveShapeToSide(laneDrawingConstantsFront.halfWidth);
        dottedGeometryBot.moveShapeToSide(laneDrawingConstantsBack.halfWidth * -1);
        // invert offset of top dotted geometry
        dottedGeometryTop.invertOffset();
        // declare DottedGeometryColor
        DottedGeometryColor dottedGeometryColor(s);
        // calculate extremes
        DottedGeometry extremes(s, dottedGeometryTop, drawFrontExtreme, dottedGeometryBot, drawBackExtreme);
        // Push draw matrix
        GLHelper::pushMatrix();
        // draw inspect or front dotted contour
        if (type == DottedContourType::INSPECT) {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
            // draw top dotted geometry
            dottedGeometryTop.drawInspectedDottedGeometry(dottedGeometryColor);
            // reset color
            dottedGeometryColor.reset();
            // draw top dotted geometry
            dottedGeometryBot.drawInspectedDottedGeometry(dottedGeometryColor);
            // change color
            dottedGeometryColor.changeColor();
            // draw extrem dotted geometry
            extremes.drawInspectedDottedGeometry(dottedGeometryColor);
        } else if (type == DottedContourType::FRONT) {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_FRONT);
            // draw top dotted geometry
            dottedGeometryTop.drawFrontDottedGeometry(dottedGeometryColor);
            // reset color
            dottedGeometryColor.reset();
            // draw top dotted geometry
            dottedGeometryBot.drawFrontDottedGeometry(dottedGeometryColor);
            // change color
            dottedGeometryColor.changeColor();
            // draw extrem dotted geometry
            extremes.drawFrontDottedGeometry(dottedGeometryColor);
        }
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEGeometry::drawDottedContourClosedShape(const DottedContourType type, const GUIVisualizationSettings& s, const PositionVector& shape, const double exaggeration) {
    if (exaggeration > 0) {
        // declare DottedGeometryColor
        DottedGeometryColor dottedGeometryColor(s);
        // scale shape using exaggeration and default dotted geometry width
        PositionVector scaledShape = shape;
        // scale exaggeration
        scaledShape.scaleRelative(exaggeration);
        // calculate dotted geometry
        GNEGeometry::DottedGeometry dottedGeometry(s, scaledShape, true);
        // Push draw matrix
        GLHelper::pushMatrix();
        // draw inspect or front dotted contour
        if (type == DottedContourType::INSPECT) {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_INSPECTED);
            // draw dotted geometry
            dottedGeometry.drawInspectedDottedGeometry(dottedGeometryColor);
        } else {
            // translate to front
            glTranslated(0, 0, GLO_DOTTEDCONTOUR_FRONT);
            // draw dotted geometry
            dottedGeometry.drawFrontDottedGeometry(dottedGeometryColor);
        }
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEGeometry::drawDottedContourShape(const DottedContourType type, const GUIVisualizationSettings& s, const PositionVector& shape, const double width, const double exaggeration) {
    // calculate dotted geometry
    GNEGeometry::DottedGeometry dottedGeometry(s, shape, false);
    // use drawDottedContourGeometry to draw it
    drawDottedContourGeometry(type, s, dottedGeometry, width * exaggeration, true, true);
}


void
GNEGeometry::drawDottedContourCircle(const DottedContourType type, const GUIVisualizationSettings& s, const Position& pos, const double radius, const double exaggeration) {
    // continue depending of exaggeratedRadio
    if ((radius * exaggeration) < 2) {
        drawDottedContourClosedShape(type, s, getVertexCircleAroundPosition(pos, radius, 8), exaggeration);
    } else {
        drawDottedContourClosedShape(type, s, getVertexCircleAroundPosition(pos, radius, 16), exaggeration);
    }
}


void
GNEGeometry::drawDottedSquaredShape(const DottedContourType type, const GUIVisualizationSettings& s, const Position& pos,
                                    const double width, const double height, const double offsetX, const double offsetY, const double rot, const double exaggeration) {
    // create shape
    PositionVector shape;
    // make rectangle
    shape.push_back(Position(0 + width, 0 + height));
    shape.push_back(Position(0 + width, 0 - height));
    shape.push_back(Position(0 - width, 0 - height));
    shape.push_back(Position(0 - width, 0 + height));
    // move shape
    shape.add(offsetX, offsetY, 0);
    // rotate shape
    shape.rotate2D(DEG2RAD((rot * -1) + 90));
    // move to position
    shape.add(pos);
    // draw using drawDottedContourClosedShape
    drawDottedContourClosedShape(type, s, shape, exaggeration);
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


void
GNEGeometry::rotateOverLane(const double rot) {
    // rotate using rotation calculated in PositionVector
    glRotated((rot * -1) + 90, 0, 0, 1);
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
