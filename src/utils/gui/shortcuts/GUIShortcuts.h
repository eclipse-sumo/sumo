/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIShortcuts.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// An enumeration of shortcuts used by the gui applications
/****************************************************************************/
#ifndef GUIShortcuts_h
#define GUIShortcuts_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// enumerations
// ===========================================================================
/**
 * @enum GUIShortcut
 * @brief An enumeration of Shortcuts used by the gui applications
 */
enum GUIShortcut {
    /// @brief default Shortcut
    SUMOShortcut_DEFAULT,

    /// @brief move Shortcut
    SUMOShortcut_MOVE,

    /// @brief max number of Shortcuts
    Shortcut_MAX
};


#endif

/****************************************************************************/

