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
/// @file    GUIDottedGeometry.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2021
///
// File for dotted geometry classes and functions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/PositionVector.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GUIDottedGeometry {

public:
    /// @enum for dotted cotour type
    enum class DottedContourType {
        INSPECT,
        INSPECT_SMALL,
        REMOVE,
        SELECT,
        FRONT,
        FRONT_SMALL,
        MOVE,
        GREEN,
        MAGENTA,
        ORANGE,
        YELLOW,
        FROMTAZ,
        TOTAZ,
        NOTHING
    };

    /// @brief class for pack all variables related with GUIDottedGeometry color
    class DottedGeometryColor {

    public:
        /// @brief constructor
        DottedGeometryColor(const GUIVisualizationSettings& settings);

        /// @brief get inspected color (and change flag)
        const RGBColor getColor(DottedContourType type);

        /// @brief change color
        void changeColor();

        /// @brief rest Dotted Geometry Color
        void reset();

    private:
        /// @brief pointer to GUIVisualizationSettings
        const GUIVisualizationSettings& mySettings;

        /// @brief flag to get color
        bool myColorFlag;

        /// @brief Invalidated assignment operator
        DottedGeometryColor& operator=(const DottedGeometryColor& other) = delete;
    };

    /// @brief dotted geometry segment
    struct Segment {
        /// @brief default constructor
        Segment();

        /// @brief constructor for a given shape
        Segment(PositionVector newShape);

        /// @brief shape
        PositionVector shape;

        /// @brief rotations
        std::vector<double> rotations;

        /// @brief lengths
        std::vector<double> lengths;

        /// @brief drawing offset (-1 or 1 only)
        double offset;
    };

    /// @brief constructor
    GUIDottedGeometry();

    /// @brief constructor for shapes
    GUIDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape);

    /// @brief constructor for extremes
    GUIDottedGeometry(const GUIVisualizationSettings& s,
                      const GUIDottedGeometry& topDottedGeometry, const bool drawFirstExtrem,
                      const GUIDottedGeometry& botDottedGeometry, const bool drawLastExtrem);

    /// @brief update GUIDottedGeometry (using lane shape)
    void updateDottedGeometry(const GUIVisualizationSettings& s, const PositionVector& laneShape);

    /// @brief update GUIDottedGeometry (using shape)
    void updateDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape);

    /// @brief draw inspected dottedShape
    void drawDottedGeometry(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                            DottedGeometryColor& dottedGeometryColor, const double customWidth = 1) const;

    /// @brief move shape to side
    void moveShapeToSide(const double value);

    /// @brief invert offset of all segments
    void invertOffset();

    /// @name draw functions
    /// @{

    /// @brief draw dotted contour for the given closed shape (used by Juctions, shapes and TAZs)
    static void drawDottedContourClosedShape(const GUIVisualizationSettings& s, const DottedContourType type, const PositionVector& shape,
            const double exaggeration, const double customWidth = 1);

    /// @brief draw dotted contour for the given shape (used by additionals)
    static void drawDottedContourShape(const GUIVisualizationSettings& s, const DottedContourType type, const PositionVector& shape,
                                       const double width, const double exaggeration, const bool drawFirstExtrem, const bool drawLastExtrem);

    /// @brief draw dotted contour for the given Position and radius (used by Juctions and POIs)
    static void drawDottedContourCircle(const GUIVisualizationSettings& s, const DottedContourType type, const Position& pos,
                                        const double radius, const double exaggeration);

    /// @brief draw dotted squared contour (used by additionals and demand elements)
    static void drawDottedSquaredShape(const GUIVisualizationSettings& s, const DottedContourType type, const Position& pos,
                                       const double width, const double height, const double offsetX, const double offsetY,
                                       const double rot, const double exaggeration);
    /// @}

private:
    /// @brief calculate shape rotations and lengths
    void calculateShapeRotationsAndLengths();

    /// @brief dotted element shape (note: It's centered in 0,0 due scaling)
    std::vector<GUIDottedGeometry::Segment> myDottedGeometrySegments;
};
