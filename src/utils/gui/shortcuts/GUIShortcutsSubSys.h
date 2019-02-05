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
// class definitions
// ===========================================================================
class GUIShortcutsSubSys {
public:
    /**@brief Initiate GUIShortcutsSubSys
     * @param[in] a FOX Toolkit APP
     */
    static void initShortcuts(FXApp* a);

    /// @brief close GUIShortcutsSubSys
    static void close();

private:
    /// @brief constructor is private because is called by the static function init(FXApp* a)
    GUIShortcutsSubSys(FXApp* a);

    /// @brief destructor
    ~GUIShortcutsSubSys();

    /// @brief instance of GUIShortcutsSubSys
    static GUIShortcutsSubSys* myInstance;

    /// @brief vector with the icons
    // FXShortcut* myShortcuts[Shortcut_MAX];
};


#endif

/****************************************************************************/

