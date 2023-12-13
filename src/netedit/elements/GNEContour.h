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
    /// @brief enum for check geometryPoints
    enum class GeometryPoint {
        FROM,   // Geometry point from
        TO,     // Geometry point to
        MIDDLE, // All geometry points except from-to
        ALL     // All geometry points
    };

    /// @brief Constructor
    GNEContour(GNEAttributeCarrier* AC);

    /// @brief destructor
    ~GNEContour();

    /// @brief get contour boundary
    Boundary getContourBoundary() const;

    /// @brief calculate contours
    /// @{

    /// @brief draw dotted contour (for closed shapes)
    void calculateContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const PositionVector& shape, const double scale, const bool addOffset,
                                     const double lineWidth) const;

    /// @brief draw dotted contour extruded (used in elements formed by a central shape)
    void calculateContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                       const PositionVector& shape, const double extrusionWidth, const double scale,
                                       const bool drawFirstExtrem, const bool drawLastExtrem, const double offset,
                                       const double lineWidth) const;

    /// @brief draw dotted contour (for rectangled elements)
    void calculateContourRectangleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const Position& pos, const double width, const double height, const double offsetX,
                                        const double offsetY, const double rot, const double scale, const double lineWidth) const;

    /// @brief draw dotted contour (circle elements)
    void calculateContourCircleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const Position& pos, double radius, const double scale, const double lineWidth) const;

    /// @brief draw dotted contour (geometry points elements)
    void calculateContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const PositionVector& shape, GeometryPoint geometryPoints, double radius, const double scale,
                                        const double lineWidth) const;

    /// @brief draw dotted contour edge
    void calculateContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                              const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem,
                              const double lineWidth) const;

    /// @brief draw dotted contour between two consecutive edges
    void calculateContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const GNEEdge* fromEdge, const GNEEdge* toEdge, const double lineWidth) const;

    /// @}

    /// @brief innen contours
    /// @{

    /// @brief draw innen contour (for closed shapes)
    void drawInnenContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                const PositionVector& shape, const double scale, const double lineWidth) const;
    /// @}

private:
    /// @brief pointer to AC
    GNEAttributeCarrier* myAC;

    /// @brief dotted geometries
    std::vector<GUIDottedGeometry>* myDottedGeometries;

    /// @brief contourboundary
    Boundary *myContourBoundary;

    /// @brief dotted geometry color
    static GUIDottedGeometry::DottedGeometryColor myDottedGeometryColor;

    /// @name build contour functions
    /// @{

    /// @brief build contour around closed shape (ej. polygon)
    PositionVector buildContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                           const PositionVector& shape, const double scale) const;

    /// @brief build contour around extruded shape (ej. lane)
    PositionVector buildContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                             const PositionVector& shape, const double extrusionWidth, const double scale,
                                             const bool drawFirstExtrem, const bool drawLastExtrem, const double offset) const;

    /// @brief build contour around rectangle
    PositionVector buildContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, 
                                         const Position& pos, const double width, const double height, const double offsetX,
                                         const double offsetY, const double rot, const double scale) const;

    /// @brief build contour aorund circle
    PositionVector buildContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                      const Position& pos, double radius, const double scale) const;

    /// @brief build contour around edge
    PositionVector buildContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                    const GNEEdge* edge, const bool drawFirstExtrem, const bool drawLastExtrem) const;

    /// @brief build contour between two from-to edgeds
    PositionVector buildContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const GNEEdge* fromEdge, const GNEEdge* toEdge) const;
    
    /// @brief update contour boundary
    void updateContourBondary() const;

    /// @}

    /// @name drawing functions
    /// @{

    /// @brief draw dotted contours (basics, select, delete, inspect...)
    void drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double lineWidth, const bool addOffset) const;

    /// @brief draw dotted contour
    void drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                           const double lineWidth, const bool addOffset) const;

    /// @}

    /// @brief invalidate default constructor
    GNEContour() = delete;

    /// @brief Invalidated copy constructor.
    GNEContour(const GNEContour&) = delete;

    /// @brief Invalidated assignment operator
    GNEContour& operator=(const GNEContour& src) = delete;
};
