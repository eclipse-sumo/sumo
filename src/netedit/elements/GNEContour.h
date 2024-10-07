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
    GNEContour();

    /// @brief destructor
    ~GNEContour();

    /// @brief get contour boundary
    Boundary getContourBoundary() const;

    /// @brief void clear contour
    void clearContour() const;

    /// @brief calculate contours
    /// @{

    /// @brief calculate contour (for closed shapes)
    void calculateContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const GUIGlObject* glObject, const PositionVector& shape, const double scale) const;

    /// @brief calculate contour extruded (used in elements formed by a central shape)
    void calculateContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                       const GUIGlObject* glObject, const PositionVector& shape, const double extrusionWidth,
                                       const double scale, const bool closeFirstExtrem, const bool closeLastExtrem,
                                       const double offset) const;

    /// @brief calculate contour (for rectangled elements)
    void calculateContourRectangleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                        const GUIGlObject* glObject, const Position& pos, const double width, const double height,
                                        const double offsetX, const double offsetY, const double rot, const double scale) const;

    /// @brief calculate contour (circle elements)
    void calculateContourCircleShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                     const GUIGlObject* glObject, const Position& pos, double radius, const double scale) const;

    /// @brief calculate contour edge
    void calculateContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                              const GNEEdge* edge, const GUIGlObject* elementToRegister, const bool closeFirstExtrem,
                              const bool closeLastExtrem) const;

    /// @brief calculate contour between two consecutive edges
    void calculateContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const GNEEdge* fromEdge, const GNEEdge* toEdge) const;

    /// @brief calculate contour for first geometry point
    void calculateContourFirstGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                            const GUIGlObject* glObject, const PositionVector& shape, const double radius,
                                            const double scale) const;

    /// @brief calculate contour for last geometry point
    void calculateContourLastGeometryPoint(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                           const GUIGlObject* glObject, const PositionVector& shape, const double radius,
                                           const double scale) const;

    /// @brief calculate contour for middle geometry point
    void calculateContourMiddleGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
            const GUIGlObject* glObject, const PositionVector& shape, const double radius,
            const double scale) const;

    /// @brief calculate contour for all geometry points
    void calculateContourAllGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                           const GUIGlObject* glObject, const PositionVector& shape, const double radius,
                                           const double scale, const bool calculatePosOverShape) const;

    /// @brief calculate contour for edge geometry points
    void calculateContourEdgeGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                            const GNEEdge* edge, const double radius, const bool calculatePosOverShape,
                                            const bool firstExtrem, const bool lastExtrem) const;
    /// @}

    /// @brief drawing contour functions
    /// @{

    /// @brief draw dotted contours (basics, select, delete, inspect...)
    void drawDottedContours(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const GNEAttributeCarrier* AC, const double lineWidth, const bool addOffset) const;

    /// @brief draw dotted contour for geometry points
    void drawDottedContourGeometryPoints(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                         const GNEAttributeCarrier* AC, const PositionVector& shape, const double radius,
                                         const double scale, const double lineWidth) const;

    /// @brief draw innen contour (currently used only in walkingAreas)
    void drawInnenContourClosed(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                const PositionVector& shape, const double scale, const double lineWidth) const;
    /// @}

private:
    /// @brief dotted geometries
    std::vector<GUIDottedGeometry>* myDottedGeometries;

    /// @brief contourboundary
    Boundary* myContourBoundary;

    /// @brief calculated shape
    PositionVector* myCalculatedShape;

    /// @brief dotted geometry color
    static GUIDottedGeometry::DottedGeometryColor myDottedGeometryColor;

    /// @name build contour functions
    /// @{

    /// @brief build contour around closed shape (ej. polygon)
    void buildContourClosedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                 const PositionVector& shape, const double scale) const;

    /// @brief build contour around extruded shape (ej. lane)
    void buildContourExtrudedShape(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                   const PositionVector& shape, const double extrusionWidth, const double scale,
                                   const bool closeFirstExtrem, const bool closeLastExtrem, const double offset) const;

    /// @brief build contour around rectangle
    void buildContourRectangle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const Position& pos, const double width, const double height, const double offsetX,
                               const double offsetY, const double rot, const double scale) const;

    /// @brief build contour aorund circle
    void buildContourCircle(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const Position& pos, double radius, const double scale) const;

    /// @brief build contour around edge
    void buildContourEdge(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                          const GNEEdge* edge, const bool closeFirstExtrem, const bool closeLastExtrem) const;

    /// @brief build contour between two from-to edgeds
    void buildContourEdges(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                           const GNEEdge* fromEdge, const GNEEdge* toEdge) const;
    /// @}

    /// @brief draw dotted contour
    void drawDottedContour(const GUIVisualizationSettings& s, GUIDottedGeometry::DottedContourType type,
                           const double lineWidth, const bool addOffset) const;

    /// @brief Invalidated copy constructor.
    GNEContour(const GNEContour&) = delete;

    /// @brief Invalidated assignment operator
    GNEContour& operator=(const GNEContour& src) = delete;
};
