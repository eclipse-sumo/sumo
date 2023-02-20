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
/// @file    GUIGeometry.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2020
///
// File for geometry classes and functions
/****************************************************************************/
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "GUIGeometry.h"

#define CIRCLE_RESOLUTION (double)10 // inverse in degrees

// ===========================================================================
// static member definitions
// ===========================================================================
PositionVector GUIGeometry::myCircleCoords;

// ===========================================================================
// method definitions
// ===========================================================================

GUIGeometry::GUIGeometry() {
}


GUIGeometry::GUIGeometry(const PositionVector& shape) :
    myShape(shape) {
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
}


GUIGeometry::GUIGeometry(const PositionVector& shape, const std::vector<double>& shapeRotations,
                         const std::vector<double>& shapeLengths) :
    myShape(shape),
    myShapeRotations(shapeRotations),
    myShapeLengths(shapeLengths) {
}


void
GUIGeometry::updateGeometry(const PositionVector& shape) {
    // clear geometry
    clearGeometry();
    // update shape
    myShape = shape;
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
}


void
GUIGeometry::updateGeometry(const PositionVector& shape, const double posOverShape,
                            const double lateralOffset) {
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
GUIGeometry::updateGeometry(const PositionVector& shape, double starPosOverShape,
                            double endPosOverShape, const double lateralOffset) {
    // first clear geometry
    clearGeometry();
    // set new shape
    myShape = shape;
    // set lateral offset
    myShape.move2side(lateralOffset);
    // get shape length
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
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
}


void
GUIGeometry::updateGeometry(const PositionVector& shape, double beginTrimPosition, double endTrimPosition,
                            const Position& extraFirstPosition, const Position& extraLastPosition) {
    // first clear geometry
    clearGeometry();
    // set new shape
    myShape = shape;
    // check trim values
    if ((beginTrimPosition != -1) || (endTrimPosition != -1)) {
        // get shape length
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
    // calculate shape rotation and lengths
    calculateShapeRotationsAndLengths();
}


void
GUIGeometry::updateSinglePosGeometry(const Position& position, const double rotation) {
    // first clear geometry
    clearGeometry();
    // set position and rotation
    myShape.push_back(position);
    myShapeRotations.push_back(rotation);
}


void
GUIGeometry::moveGeometryToSide(const double amount) {
    // move shape
    myShape.move2side(amount);
}


void
GUIGeometry::scaleGeometry(const double scale) {
    // scale shape and lengths
    myShape.scaleRelative(scale);
    // scale lengths
    for (auto& shapeLength : myShapeLengths) {
        shapeLength *= scale;
    }
}


const PositionVector&
GUIGeometry::getShape() const {
    return myShape;
}


const std::vector<double>&
GUIGeometry::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<double>&
GUIGeometry::getShapeLengths() const {
    return myShapeLengths;
}


double
GUIGeometry::calculateRotation(const Position& first, const Position& second) {
    // return rotation (angle) of the vector constructed by points first and second
    return ((double)atan2((second.x() - first.x()), (first.y() - second.y())) * (double) 180.0 / (double)M_PI);
}


double
GUIGeometry::calculateLength(const Position& first, const Position& second) {
    // return 2D distance between two points
    return first.distanceTo2D(second);
}


void
GUIGeometry::adjustStartPosGeometricPath(double& startPos, const PositionVector& startLaneShape,
        double& endPos, const PositionVector& endLaneShape) {
    // adjust both, if start and end lane are the same
    if ((startLaneShape.size() > 0) &&
            (endLaneShape.size() > 0) &&
            (startLaneShape == endLaneShape) &&
            (startPos != -1) &&
            (endPos != -1)) {
        if (startPos >= endPos) {
            endPos = (startPos + POSITION_EPS);
        }
    }
    // adjust startPos
    if ((startPos != -1) && (startLaneShape.size() > 0)) {
        if (startPos < POSITION_EPS) {
            startPos = POSITION_EPS;
        }
        if (startPos > (startLaneShape.length() - POSITION_EPS)) {
            startPos = (startLaneShape.length() - POSITION_EPS);
        }
    }
    // adjust endPos
    if ((endPos != -1) && (endLaneShape.size() > 0)) {
        if (endPos < POSITION_EPS) {
            endPos = POSITION_EPS;
        }
        if (endPos > (endLaneShape.length() - POSITION_EPS)) {
            endPos = (endLaneShape.length() - POSITION_EPS);
        }
    }
}


void
GUIGeometry::drawGeometry(const GUIVisualizationSettings& s, const Position& mousePos,
                          const GUIGeometry& geometry, const double width, double offset) {
    // continue depending of draw for position selection
    if (s.drawForPositionSelection) {
        // obtain position over lane relative to mouse position
        const Position posOverLane = geometry.getShape().positionAtOffset2D(geometry.getShape().nearest_offset_to_point2D(mousePos));
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePos) <= (width * width)) {
            // push matrix
            GLHelper::pushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, s.getCircleResolution());
            // pop draw matrix
            GLHelper::popMatrix();
        }
    } else if (s.scale * width < 1) {
        // draw line (needed for zoom out)
        GLHelper::drawLine(geometry.getShape());
    } else {
        GLHelper::drawBoxLines(geometry.getShape(), geometry.getShapeRotations(), geometry.getShapeLengths(), width, 0, offset);
    }
}


void
GUIGeometry::drawContourGeometry(const GUIGeometry& geometry, const double width, const bool drawExtremes) {
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
GUIGeometry::drawGeometryPoints(const GUIVisualizationSettings& s, const Position& mousePos, const PositionVector& shape,
                                const RGBColor& geometryPointColor, const RGBColor& textColor, const double radius, const double exaggeration,
                                const bool editingElevation, const bool drawExtremeSymbols) {
    // get exaggeratedRadio
    const double exaggeratedRadio = (radius * exaggeration);
    // get radius squared
    const double exaggeratedRadioSquared = (exaggeratedRadio * exaggeratedRadio);
    // iterate over shape
    for (const auto& vertex : shape) {
        // if drawForPositionSelection is enabled, check distance between mouse and vertex
        if (!s.drawForPositionSelection || (mousePos.distanceSquaredTo2D(vertex) <= exaggeratedRadioSquared)) {
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
                if (editingElevation) {
                    // Push Z matrix
                    GLHelper::pushMatrix();
                    // draw Z (elevation)
                    GLHelper::drawText(toString(vertex.z()), vertex, 0.3, 0.7, textColor);
                    // pop Z matrix
                    GLHelper::popMatrix();
                } else if ((vertex == shape.front()) && drawDetail && drawExtremeSymbols) {
                    // push "S" matrix
                    GLHelper::pushMatrix();
                    // draw a "s" over first point
                    GLHelper::drawText("S", vertex, 0.3, 2 * exaggeratedRadio, textColor);
                    // pop "S" matrix
                    GLHelper::popMatrix();
                } else if ((vertex == shape.back()) && (shape.isClosed() == false) && drawDetail && drawExtremeSymbols) {
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
GUIGeometry::drawMovingHint(const GUIVisualizationSettings& s, const Position& mousePos, const PositionVector& shape,
                            const RGBColor& hintColor, const double radius, const double exaggeration) {
    // get exaggeratedRadio
    const double exaggeratedRadio = (radius * exaggeration);
    // obtain distance to shape
    const double distanceToShape = shape.distance2D(mousePos);
    // obtain squared radius
    const double squaredRadius = (radius * radius * exaggeration);
    // declare index
    int index = -1;
    // iterate over shape
    for (int i = 0; i < (int)shape.size(); i++) {
        // check distance
        if (shape[i].distanceSquaredTo2D(mousePos) <= squaredRadius) {
            index = i;
        }
    }
    // continue depending of distance to shape
    if ((distanceToShape < exaggeratedRadio) && (index == -1)) {
        // obtain position over lane
        const Position positionOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePos));
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


void
GUIGeometry::drawLaneGeometry(const GUIVisualizationSettings& s, const Position& mousePos, const PositionVector& shape,
                              const std::vector<double>& rotations, const std::vector<double>& lengths, const std::vector<RGBColor>& colors,
                              double width, const bool onlyContour, const double offset) {
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
    } else if (s.drawForPositionSelection) {
        // obtain position over lane relative to mouse position
        const Position posOverLane = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mousePos), offset);
        // if mouse is over segment
        if (posOverLane.distanceSquaredTo2D(mousePos) <= (width * width)) {
            // push matrix
            GLHelper::pushMatrix();
            // translate to position over lane
            glTranslated(posOverLane.x(), posOverLane.y(), 0);
            // Draw circle
            GLHelper::drawFilledCircle(width, s.getCircleResolution());
            // pop draw matrix
            GLHelper::popMatrix();
        }
    } else if (colors.size() > 0) {
        // draw box lines with own colors
        GLHelper::drawBoxLines(shape, rotations, lengths, colors, width, 0, offset);
    } else {
        // draw box lines with current color
        GLHelper::drawBoxLines(shape, rotations, lengths, width, 0, offset);
    }
}


void
GUIGeometry::drawParentLine(const GUIVisualizationSettings& s, const Position& parent, const Position& child,
                            const RGBColor& color, const bool drawEntire, const double lineWidth) {
    if (!s.drawForPositionSelection && !s.drawForRectangleSelection) {
        // calculate rotation
        const double rot = RAD2DEG(parent.angleTo2D(child)) + 90;
        // calculate distance between origin and destiny
        const double distanceSquared = parent.distanceSquaredTo2D(child);
        // Add a draw matrix for details
        GLHelper::pushMatrix();
        // move back
        glTranslated(0, 0, -1);
        // draw box line
        if (drawEntire) {
            // draw first box line
            GLHelper::setColor(color.changedBrightness(-50));
            GLHelper::drawBoxLine(parent, rot, sqrt(distanceSquared), lineWidth);
            // move front
            glTranslated(0, 0, 0.1);
            // draw second box line
            GLHelper::setColor(color);
            GLHelper::drawBoxLine(parent, rot, sqrt(distanceSquared), .04);
        } else if (distanceSquared > 25) {
            // draw first box line with length 4.9
            GLHelper::setColor(color.changedBrightness(-50));
            GLHelper::drawBoxLine(parent, rot, 4.9, lineWidth);
            glTranslated(0, 0, 0.1);
            // draw second box line with length 4.9
            GLHelper::setColor(color);
            GLHelper::drawBoxLine(parent, rot, 4.9, .04);
            // draw arrow depending of distanceSquared (10*10)
            if (distanceSquared > 100) {
                // calculate positionVector between both points
                const PositionVector vector = {parent, child};
                // draw first arrow at end
                GLHelper::setColor(color.changedBrightness(-50));
                GLHelper::drawTriangleAtEnd(parent,
                                            vector.positionAtOffset2D(5),
                                            s.additionalSettings.arrowWidth,
                                            s.additionalSettings.arrowLength,
                                            s.additionalSettings.arrowOffset);
                // move front
                glTranslated(0, 0, 0.1);
                // draw second arrow at end
                GLHelper::setColor(color);
                GLHelper::drawTriangleAtEnd(parent,
                                            vector.positionAtOffset2D(5),
                                            s.additionalSettings.arrowWidth - 0.01,
                                            s.additionalSettings.arrowLength - 0.01,
                                            s.additionalSettings.arrowOffset - 0.01);
            }
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


void
GUIGeometry::drawChildLine(const GUIVisualizationSettings& s, const Position& child, const Position& parent,
                           const RGBColor& color, const bool drawEntire, const double lineWidth) {
    if (!s.drawForPositionSelection && !s.drawForRectangleSelection) {
        // calculate distance between origin and destiny
        const double distanceSquared = child.distanceSquaredTo2D(parent);
        // calculate subline width
        const double sublineWidth = (lineWidth * 0.8);
        // calculate rotation
        const double rot = RAD2DEG(child.angleTo2D(parent)) + 90;
        // Add a draw matrix for details
        GLHelper::pushMatrix();
        // move back
        glTranslated(0, 0, -1);
        // set color
        GLHelper::setColor(color);
        // draw box line
        if (drawEntire || (distanceSquared < 25)) {
            // set color
            GLHelper::setColor(color);
            // draw first box line
            GLHelper::setColor(color.changedBrightness(-50));
            GLHelper::drawBoxLine(child, rot, sqrt(distanceSquared), lineWidth);
            // move front
            glTranslated(0, 0, 0.1);
            // draw second box line
            GLHelper::setColor(color);
            GLHelper::drawBoxLine(child, rot, sqrt(distanceSquared), sublineWidth);
        } else {
            // draw first box line with length 4.9
            GLHelper::setColor(color.changedBrightness(-50));
            GLHelper::drawBoxLine(child, rot, 4.9, lineWidth);
            glTranslated(0, 0, 0.1);
            // draw second box line with length
            GLHelper::setColor(color);
            GLHelper::drawBoxLine(child, rot, 4.9, sublineWidth);
            // draw arrow depending of distanceSquared (10*10)
            if (distanceSquared > 100) {
                // calculate positionVector between both points
                const PositionVector vector = {child, parent};
                // draw first arrow at end
                GLHelper::setColor(color.changedBrightness(-50));
                GLHelper::drawTriangleAtEnd(child,
                                            vector.positionAtOffset2D(5),
                                            s.additionalSettings.arrowWidth,
                                            s.additionalSettings.arrowLength,
                                            s.additionalSettings.arrowOffset);
                // move front
                glTranslated(0, 0, 0.1);
                // draw second arrow at end
                GLHelper::setColor(color);
                GLHelper::drawTriangleAtEnd(child,
                                            vector.positionAtOffset2D(5),
                                            s.additionalSettings.arrowWidth - 0.01,
                                            s.additionalSettings.arrowLength - 0.01,
                                            s.additionalSettings.arrowOffset - 0.01);
            }
        }
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


PositionVector
GUIGeometry::getVertexCircleAroundPosition(const Position& pos, const double width, const int steps) {
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
        const Position& vertex = myCircleCoords[GUIGeometry::angleLookup(i * inc)];
        vertexCircle.push_back(Position(vertex.x() * width, vertex.y() * width));
    }
    // move result using position
    vertexCircle.add(pos);
    return vertexCircle;
}


void
GUIGeometry::rotateOverLane(const double rot) {
    // rotate using rotation calculated in PositionVector
    glRotated((rot * -1) + 90, 0, 0, 1);
}


int
GUIGeometry::angleLookup(const double angleDeg) {
    const int numCoords = (int)myCircleCoords.size() - 1;
    int index = ((int)(floor(angleDeg * CIRCLE_RESOLUTION + 0.5))) % numCoords;
    if (index < 0) {
        index += numCoords;
    }
    assert(index >= 0);
    return (int)index;
}


void GUIGeometry::clearGeometry() {
    // clear geometry containers
    myShape.clear();
    myShapeRotations.clear();
    myShapeLengths.clear();
}


void
GUIGeometry::calculateShapeRotationsAndLengths() {
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

/****************************************************************************/
