/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEMove.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for elements that own a movable shape
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGeometry.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElement;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief move element
class GNEMoveOperation {

public:
    /// @brief constructor for entire geometries
    GNEMoveOperation(GNEMoveElement *_moveElement,
        GNEGeometry::Geometry _originalGeometry,
        std::vector<int> _geometryPointsToMove) :
        moveElement(_moveElement),
        originalGeometry(_originalGeometry),
        clickedIndex(-1),
        geometryToMove(_originalGeometry),
        geometryPointsToMove({}) {}

    /// @brief constructor
    GNEMoveOperation(GNEMoveElement *_moveElement,
        GNEGeometry::Geometry _originalGeometry,
        GNEGeometry::Geometry _geometryToMove,
        const int _clickedIndex,
        std::vector<int> _geometryPointsToMove) :
        moveElement(_moveElement),
        originalGeometry(_originalGeometry),
        clickedIndex(_clickedIndex),
        geometryToMove(_geometryToMove),
        geometryPointsToMove(_geometryPointsToMove) {}

    /// @brief destructor
    ~GNEMoveOperation() {}

    const GNEMoveElement *moveElement;

    /// @brief original geometry
    const GNEGeometry::Geometry originalGeometry;

    /// @brief geometry to move (note: it can be different of originalGeometry, for example due a new Geometry point)
    const GNEGeometry::Geometry geometryToMove;

    /// @brief clicked index (of geometryToMove)
    const int clickedIndex;

    /// @brief geometry points to move (of geometryToMove)
    const std::vector<int> geometryPointsToMove;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveOperation(const GNEMoveOperation&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveOperation& operator=(const GNEMoveOperation&) = delete;
};


class GNEMoveElement {

public:

    /// @brief constructor
    GNEMoveElement() {}

    virtual GNEMoveOperation* getMoveOperation(const double shapeOffset) = 0;


    static void moveElement(GNEMoveOperation* moveOperation, const Position &offset);

    static void commitMove(GNEMoveOperation* moveOperation, const Position &offset);

protected:
    virtual void setMoveGeometry(const GNEGeometry::Geometry& newGeometry) = 0;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveElement(const GNEMoveElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveElement& operator=(const GNEMoveElement&) = delete;
};
