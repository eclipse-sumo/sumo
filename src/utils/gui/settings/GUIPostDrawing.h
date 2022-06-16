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

// ===========================================================================
// class definitions
// ===========================================================================

class GUIPostDrawing {

public:
    /// @brief constructor
    GUIPostDrawing();

    /// @brief add recompute carrier to recompute
    void addACToUpdate(GNEAttributeCarrier* AC);

    /// @brief recompute and clear ACs
    void updateAndClearACs();

    /// @brief recompute boundaries
    bool recomputeBoundaries = false;

protected:
    /// @brief ACs to update
    std::vector<GNEAttributeCarrier*> myACsToUpdate;

private:
    /// @brief set copy constructor private
    GUIPostDrawing(const GUIPostDrawing&) = default;

    /// @brief set assignment operator private
    GUIPostDrawing& operator=(const GUIPostDrawing&) = default;
};
