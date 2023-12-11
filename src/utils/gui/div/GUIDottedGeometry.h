/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
        INSPECT,    // Inspecting element
        REMOVE,     // Mouse over element to remove
        SELECT,     // Mouse over element to select
        MOVE,       // Mouse over element to move
        FRONT,      // Element marked as "front element"
        OVER,       // Mouse over element (orange)
        FROM,       // Element marked as from (green)
        TO,         // Element marked as to (magenta)
        RELATED,    // Element marked as related (cyan)
        NOTHING
    };

    /// @brief class for pack all variables related with GUIDottedGeometry color
    class DottedGeometryColor {

    public:
        /// @brief constructor
        DottedGeometryColor();

        /// @brief get inspected color (and change flag)
        const RGBColor getColor(const GUIVisualizationSettings& settings, DottedContourType type);

        /// @brief change color
        void changeColor();

        /// @brief rest Dotted Geometry Color
        void reset();

    private:
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
    };

    /// @brief constructor
    GUIDottedGeometry();

    /// @brief constructor for shapes
    GUIDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape, const bool resample);

    /// @brief update GUIDottedGeometry (using lane shape)
    void updateDottedGeometry(const GUIVisualizationSettings& s, const PositionVector& laneShape, const bool resample);

    /// @brief update GUIDottedGeometry (using shape)
    void updateDottedGeometry(const GUIVisualizationSettings& s, PositionVector shape, const bool closeShape, const bool resample);

    /// @brief draw dotted geometry
    void drawDottedGeometry(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                            DottedGeometryColor& dottedGeometryColor, const bool addOffset, const double lineWidth) const;

    /// @brief draw innen geometry
    void drawInnenGeometry(const double lineWidth) const;

    /// @brief move shape to side
    void moveShapeToSide(const double value);

    /// @brief get front position
    Position getFrontPosition() const;

    /// @brief get back position
    Position getBackPosition() const;

    /// @brief get simple shape (the shape without resampling)
    const PositionVector &getUnresampledShape() const;

private:
    /// @brief calculate shape rotations and lengths
    void calculateShapeRotationsAndLengths();

    /// @brief shape without resampling
    PositionVector myUnresampledShape;

    /// @brief dotted element shape (note: It's centered in 0,0 due scaling)
    std::vector<GUIDottedGeometry::Segment> myDottedGeometrySegments;
};
