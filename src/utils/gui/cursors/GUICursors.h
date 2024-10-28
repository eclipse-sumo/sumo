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
/// @file    GUICursors.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2018
///
// An enumeration of icons used by the gui applications
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUICursor
 * @brief An enumeration of cursors used by the gui applications
 */
enum class GUICursor {
    /// @brief default cursor
    DEFAULT,

    /// @brief move view cursor
    MOVEVIEW,

    /// @brief select cursor
    SELECT,

    /// @brief delete cursor
    DELETE_CURSOR,

    /// @brief select lanecursor
    SELECT_LANE,

    /// @brief inspect cursor
    INSPECT,

    /// @brief inspect lane cursor
    INSPECT_LANE,

    /// @brief move element cursor
    MOVEELEMENT,
};
