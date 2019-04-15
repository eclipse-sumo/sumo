/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIShortcutsSubSys.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// Helper for Shortcuts
/****************************************************************************/
#ifndef GUIShortcutsSubSys_h
#define GUIShortcutsSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include "GUIShortcuts.h"

// ===========================================================================
// class declaration
// ==========================================================================
class GNEApplicationWindow;
class GUIApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================
class GUIShortcutsSubSys {
public:
    /// @brief build SUMO Accelerators
    static void buildSUMOAccelerators(GUIApplicationWindow* GUIApp);

    /// @brief build NETEDIT Accelerators
    static void buildNETEDITAccelerators(GNEApplicationWindow* GNEApp);

private:
    // @brief parse GUIShortcut to int
    static int parseKey(GUIShortcut key);

    // @brief parse GUIShortcut to int with a modifier
    static int parseKey(GUIShortcut key, GUIShortcutModifier firstModifier);

    // @brief parse GUIShortcut to int with two modifiers
    static int parseKey(GUIShortcut key, GUIShortcutModifier firstModifier, GUIShortcutModifier secondModifier);
};


#endif

/****************************************************************************/

