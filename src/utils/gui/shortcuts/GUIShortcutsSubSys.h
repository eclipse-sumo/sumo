/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    GUIShortcutsSubSys.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
///
// Helper for Shortcuts
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>

#include "GUIShortcuts.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GUIShortcutsSubSys {

public:
    /// @brief build accelerators
    static void buildAccelerators(FXAccelTable* accelTable, FXObject* target, const bool sumogui);

    /// @brief change acelerator (used for toogle dynamically binding space key with start/end simulation)
    static void changeAccelerator(FXAccelTable* accelTable, FXObject* target, GUIShortcut keysym, long msg);

private:
    // @brief parse GUIShortcut to int
    static int parseKey(GUIShortcut key);

    // @brief parse GUIShortcut to int with a modifier
    static int parseKey(GUIShortcut key, GUIShortcutModifier firstModifier);

    // @brief parse GUIShortcut to int with two modifiers
    static int parseKey(GUIShortcut key, GUIShortcutModifier firstModifier, GUIShortcutModifier secondModifier);
};
