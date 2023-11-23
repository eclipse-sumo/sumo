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
/// @file    GNEContour.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// class used for show contour elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIDottedGeometry.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNENet;
class GNEEdge;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEContour {

public:
    /// @brief Constructor
    GNEContour(GNEAttributeCarrier* AC);

    /// @brief destructor
    ~GNEContour();

    /// @brief reset dotted contour
    void reset() const;

    /// @brief dotted contours
    /// @{

    /// @brief draw dotted contour (for closed shapes)
    void drawDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                 const double scale, const bool addOffset, const double lineWidth) const;

    /// @brief draw dotted contour extruded (used in elements formed by a central shape)
    void drawDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape,
                                   const double extrusionWidth, const double scale, const bool drawFirstExtrem,
                                   const bool drawLastExtrem, const double lineWidth) const;

    /// @brief draw dotted contour (for rectangled elements)
    void drawDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width,
                                    const double height, const double offsetX, const double offsetY, const double rot,
                                    const double scale, const double lineWidth) const;

    /// @brief draw dotted contour (circle)
    void drawDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius,
                                 const double scale, const double lineWidth) const;

    /// @brief draw dotted contour edge
    void drawDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem,
                               const bool drawLastExtrem, const double lineWidth) const;

    /// @brief draw dotted contour between two edges
    void drawDottedContourEdges(const GUIVisualizationSettings& s, const GNEEdge* fromEdge, const GNEEdge* toEdge,
                                const double lineWidth) const;

    /// @}

    /// @brief innen contours
    /// @{

    /// @brief draw innen contour (for closed shapes)
    void drawInnenContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape,
                                const double scale, const double lineWidth) const;
    /// @}

private:
    /// @brief pointer to AC
    GNEAttributeCarrier* myAC;

    /// @brief pointer to cached shape
    std::vector<PositionVector>* myCachedShapes;

    /// @brief width, height, rot, scale
    std::vector<double>* myCachedDoubles;

    /// @brief dotted geometries
    std::vector<GUIDottedGeometry>* myDottedGeometries;

    /// @brief dotted geometry color
    static GUIDottedGeometry::DottedGeometryColor myDottedGeometryColor;

    /// @brief build dotted contour shape
    void buildDottedContourClosed(const GUIVisualizationSettings& s, const PositionVector& shape, const double scale) const;

    /// @brief build and draw dotted contour extruded
    void buildDottedContourExtruded(const GUIVisualizationSettings& s, const PositionVector& shape, const double extrusionWidth, const double scale,
                                    const bool drawFirstExtrem, const bool drawLastExtrem) const;

    /// @brief build dotted contour rectangle
    void buildDottedContourRectangle(const GUIVisualizationSettings& s, const Position& pos, const double width, const double height, const double offsetX,
                                     const double offsetY, const double rot, const double scale) const;

    /// @brief build dotted contour circle
    void buildDottedContourCircle(const GUIVisualizationSettings& s, const Position& pos, double radius, const double scale) const;

    /// @brief build dotted contour edge
    void buildDottedContourEdge(const GUIVisualizationSettings& s, const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const;

    /// @brief build dotted contour edges
    void buildDottedContourEdges(const GUIVisualizationSettings& s, const GNEEdge* fromEdge, const GNEEdge* toEdge) const;

    /// @brief draw dotted contours
    void drawDottedContours(const GUIVisualizationSettings& s, const double scale, const bool addOffset, const double lineWidth) const;

    /// @brief draw dotted contour
    void drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type, const bool addOffset, const double lineWidth) const;

    /// @brief invalidate default constructor
    GNEContour() = delete;

    /// @brief Invalidated copy constructor.
    GNEContour(const GNEContour&) = delete;

    /// @brief Invalidated assignment operator
    GNEContour& operator=(const GNEContour& src) = delete;
};
