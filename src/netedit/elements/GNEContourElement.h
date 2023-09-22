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
/// @file    GNEContourElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A abstract class contour elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIDottedGeometry.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNENet;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEContourElement {

protected:
    /// @brief Constructor
    GNEContourElement(GNEAttributeCarrier* AC);

    /// @brief destructor
    ~GNEContourElement();

    /// @brief draw dotted contour (used in element extended over a shape)
    void drawDottedContour(const GNENet *net, const PositionVector &shape, double width, double exaggeration,
                           const bool drawFirstExtrem, const bool drawLastExtrem) const;

private:
    /// @brief pointer to AC
    GNEAttributeCarrier* myAC;

    /// @brief pointer to cached shape
    PositionVector *myCachedShape;

    /// @brief pointer to dotted geometry A
    GUIDottedGeometry *myDottedGeometryA;

    /// @brief pointer to dotted geometry B
    GUIDottedGeometry *myDottedGeometryB;

    /// @brief pointer to dotted geometry C
    GUIDottedGeometry *myDottedGeometryC;

    /// @brief draw dotted contour rectangle
    void buildAndDrawDottedContourRectangle(const GUIVisualizationSettings& s, const GUIDottedGeometry::DottedContourType type,
                                            const PositionVector &shape, double width, double exaggeration,
                                            const bool drawFirstExtrem, const bool drawLastExtrem) const;
};
