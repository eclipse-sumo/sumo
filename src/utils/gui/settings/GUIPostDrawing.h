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
/// @file    GUIPostDrawing.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 22
///
// Operations that must be applied after drawGL()
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEJunction;
class GNEEdge;
class GNELane;
class GNERoute;

// ===========================================================================
// class definitions
// ===========================================================================

class GUIPostDrawing {

public:
    /// @brief typedefs
    typedef std::vector<std::pair<const GUIGlObject*, std::vector<int> > > GLObjectsContainer;

    /// @brief constructor
    GUIPostDrawing();

    /// @brief clear elements
    void clearElements();

    /// @brief check if element is under cursor
    bool isElementUnderCursor(const GUIGlObject* GLObject) const;

    /// @brief check if geometry point is under cursor
    bool isGeometryPointUnderCursor(const GUIGlObject* GLObject, const int geometryPoint) const;

    /// @brief check if mouse is within elements geometry (for circles)
    bool positionWithinCircle(const GUIGlObject* GLObject, const Position &pos, const Position &center, const double radius);

    /// @brief check if mouse is within geometry point
    bool positionWithinGeometryPoint(const GUIGlObject* GLObject, const Position &pos, const int index, const Position &center, const double radius);

    /// @brief check if mouse is within closed shapes (for filled shapes)
    bool positionWithinShape(const GUIGlObject* GLObject, const Position &pos, const PositionVector &shape);

    /// @brief get all elements under cursor
    const GLObjectsContainer& getElementsUnderCursor() const;

    /// @brief recompute boundaries
    GUIGlObjectType recomputeBoundaries = GLO_NETWORK;

    /// @brief marked edge (used in create edge mode, for splitting)
    const GNEEdge* markedEdge = nullptr;

    /// @brief marked lane (used in create edge mode, for splitting)
    const GNELane* markedLane = nullptr;

    /// @brief marked TAZ (used in create TAZRel mode)
    const GUIGlObject* markedTAZ = nullptr;

    /// @brief marked route (used in create vehicle mode)
    const GNERoute* markedRoute = nullptr;

    /// @brief marked first geometry point (used for moving/delete geometry points)
    const GUIGlObject* markedFirstGeometryPoint = nullptr;

    /// @brief marked first geometry point (used for moving/delete geometry points)
    const GUIGlObject* markedSecondGeometryPoint = nullptr;

protected:
    /// @brief elements under cursor and their geometry point indexes
    GLObjectsContainer myElementsUnderCursor;

    /// @brief add element into list of elements under cursor
    bool addElementUnderCursor(const GUIGlObject* GLObject);

    /// @brief add geometryPoint into list of elements under cursor
    bool addGeometryPointUnderCursor(const GUIGlObject* GLObject, const int newIndex);

private:
    /// @brief set copy constructor private
    GUIPostDrawing(const GUIPostDrawing&) = default;

    /// @brief set assignment operator private
    GUIPostDrawing& operator=(const GUIPostDrawing&) = default;
};
