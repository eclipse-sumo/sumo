/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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


// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================

class GUIPostDrawing {

public:
    /// @brief constructor
    GUIPostDrawing();

    /// @brief execute post drawing tasks
    void executePostDrawingTasks();

    /// @brief add recompute carrier to recompute
    void addACToUpdate(GNEAttributeCarrier* AC);

    /// @brief recompute boundaries
    bool recomputeBoundaries = false;

    /// @brief marked Node (used in create edge mode, )
    const GUIGlObject* markedNode = nullptr;

protected:
    /// @brief ACs to update
    std::vector<GNEAttributeCarrier*> myACsToUpdate;

private:
    /// @brief set copy constructor private
    GUIPostDrawing(const GUIPostDrawing&) = default;

    /// @brief set assignment operator private
    GUIPostDrawing& operator=(const GUIPostDrawing&) = default;
};
