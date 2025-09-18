/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveOffset.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Class used for define move offset
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveOffset {

public:
    /// @brief constructor
    GNEMoveOffset();

    /// @brief constructor for X-Y move
    GNEMoveOffset(const double x, const double y);

    /// @brief constructor for Z move
    GNEMoveOffset(const double z);

    /// @brief destructor
    ~GNEMoveOffset();

    /// @brief X
    const double x;

    /// @brief Y
    const double y;

    /// @brief Z
    const double z;

private:
    /// @brief Invalidated copy constructor.
    GNEMoveOffset(const GNEMoveOffset&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMoveOffset& operator=(const GNEMoveOffset&) = delete;
};
