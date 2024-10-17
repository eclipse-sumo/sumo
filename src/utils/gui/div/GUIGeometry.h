/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIGeometry.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2020
///
// File for geometry classes and functions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GUIGeometry {

public:
    /// @brief default constructor
    GUIGeometry();

    /// @brief parameter constructor
    GUIGeometry(const PositionVector& shape);

    /// @brief parameter constructor
    GUIGeometry(const PositionVector& shape, const std::vector<double>& shapeRotations,
                const std::vector<double>& shapeLengths);

    /// @brief update entire geometry
    void updateGeometry(const PositionVector& shape);

    /// @brief update geometry (using a shape, a position over shape and a lateral offset)
    void updateGeometry(const PositionVector& shape, const double posOverShape, const double lateralOffset);

    /// @brief update geometry (using a shape, a starPos over shape, a endPos and a lateral offset)
    void updateGeometry(const PositionVector& shape, double starPosOverShape, double endPosOverShape,
                        const double lateralOffset);

    /// @brief update geometry (using a shape to be trimmed)
    void updateGeometry(const PositionVector& shape, double beginTrimPosition, const Position& extraFirstPosition,
                        double endTrimPosition, const Position& extraLastPosition);

    /// @brief update position and rotation
    void updateSinglePosGeometry(const Position& position, const double rotation);

    /// @brief clear geometry
    void clearGeometry();

    /// @brief move current shape to side
    void moveGeometryToSide(const double amount);

    /// @brief scale geometry
    void scaleGeometry(const double scale);

    /// @brief The shape of the additional element
    const PositionVector& getShape() const;

    /// @brief The rotations of the single shape parts
    const std::vector<double>& getShapeRotations() const;

    /// @brief The lengths of the single shape parts
    const std::vector<double>& getShapeLengths() const;

    /// @name calculation functions
    /// @{

    /// @brief return angle between two points (used in geometric calculations)
    static double calculateRotation(const Position& first, const Position& second);

    /// @brief return length between two points (used in geometric calculations)
    static double calculateLength(const Position& first, const Position& second);

    /// @brief adjust start and end positions in geometric path
    static void adjustStartPosGeometricPath(double& startPos, const PositionVector& startLaneShape, double& endPos,
                                            const PositionVector& endLaneShape);

    /// @}

    /// @name draw functions
    /// @{

    /// @brief draw geometry
    static void drawGeometry(const GUIVisualizationSettings::Detail d, const GUIGeometry& geometry,
                             const double width, double offset = 0);

    /// @brief draw colored geometry
    static void drawGeometry(const GUIVisualizationSettings::Detail d, const GUIGeometry& geometry,
                             const std::vector<RGBColor>& colors, const double width, double offset = 0);

    /// @brief draw contour geometry
    static void drawContourGeometry(const GUIGeometry& geometry, const double width, const bool drawExtremes = false);

    /// @brief draw geometry points
    static void drawGeometryPoints(const GUIVisualizationSettings::Detail d, const PositionVector& shape,
                                   const RGBColor& color, const double radius, const double exaggeration,
                                   const bool editingElevation);

    /// @brief draw line between parent and children (used in netedit)
    static void drawParentLine(const GUIVisualizationSettings& s, const Position& parent, const Position& child,
                               const RGBColor& color, const bool drawEntire, const double lineWidth);

    /// @brief draw line between child and parent (used in netedit)
    static void drawChildLine(const GUIVisualizationSettings& s, const Position& child, const Position& parent,
                              const RGBColor& color, const bool drawEntire, const double lineWidth);

    /// @brief get a circle around the given position
    static PositionVector getVertexCircleAroundPosition(const Position& pos, const double width, const int steps = 8);

    /// @brief rotate over lane (used by Lock icons, detector logos, etc.)
    static void rotateOverLane(const double rot);

    /// @}

protected:
    /// @brief calculate shape rotations and lengths
    void calculateShapeRotationsAndLengths();

    /// @brief element shape
    PositionVector myShape;

    /// @brief The rotations of the shape (note: Always size = myShape.size()-1)
    std::vector<double> myShapeRotations;

    /// @brief The lengths of the shape (note: Always size = myShape.size()-1)
    std::vector<double> myShapeLengths;

private:
    /// @brief Storage for precomputed sin/cos-values describing a circle
    static PositionVector myCircleCoords;

    /// @brief normalize angle for lookup in myCircleCoords
    static int angleLookup(const double angleDeg);
};
