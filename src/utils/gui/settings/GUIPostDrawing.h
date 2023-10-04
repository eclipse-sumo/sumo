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
    /// @brief constructor
    GUIPostDrawing();

    /// @brief execute post drawing tasks
    void executePostDrawingTasks();

    /// @brief mark GLObject to update (usually the geometry)
    void markGLObjectToUpdate(GUIGlObject* GLObject);

    /// @brief add element into list of elements under cursor
    void addElementUnderCursor(const GUIGlObject* GLObject);

    /// @brief check if element is under cursor
    bool isElementUnderCursor(const GUIGlObject* GLObject) const;

    /// @brief get all elements under cursor
    const std::vector<const GUIGlObject*>& getElementUnderCursor() const;

    /// @brief recompute boundaries
    GUIGlObjectType recomputeBoundaries = GLO_NETWORK;

    /// @brief mouse position before rendering elements
    Position mousePos = Position::INVALID;

    /// @brief elements marked for drawing over contour (used in netedit)
    const GUIGlObject* markedElementOverContour;

    /// @brief elements marked for drawing delete contour (used in netedit)
    const GUIGlObject* markedElementDeleteContour;

    /// @brief elements marked for drawing select contour (used in netedit)
    const GUIGlObject* markedElementSelectContour;

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
    /// @brief GLObjects to update
    std::vector<GUIGlObject*> myGLObjectsToUpdate;

    /// @brief elements under cursor
    std::vector<const GUIGlObject*> myElementsUnderCursor;

private:
    /// @brief set copy constructor private
    GUIPostDrawing(const GUIPostDrawing&) = default;

    /// @brief set assignment operator private
    GUIPostDrawing& operator=(const GUIPostDrawing&) = default;
};
